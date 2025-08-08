#include "ppu.hpp"
#include "nes.hpp"
#include "colors.hpp"
#include "util.hpp"
#include <cassert>
#include <cstring> //For memset

namespace nes{

Ppu::Ppu(){
  colors = get_colors();
}

auto Ppu::init_renderer() -> void{
  renderer.init(ScreenSize);
}

auto Ppu::mem_read(const Nes& nes, u16 address) const -> u8{
  const auto cardridge_data = nes.cardridge.read_pattern(address);
  if (cardridge_data != std::nullopt){
    return cardridge_data.value(); 
  }
  else if (in_range(address, Ppu::NametablesAddressRange)){
    address &= 0x0FFF;
    auto nametable_index = 0;

    if (nes.cardridge.mirroring() == Cardridge::Mirroring::Vertical){
      if (
        in_range(address, Ppu::TopRightNametableAddressRange) ||
        in_range(address, Ppu::BottomRightNametableAddressRange)
      ){
        nametable_index = 1;
      }
    }
    else if (nes.cardridge.mirroring() == Cardridge::Mirroring::Horizontal){
      if (
        in_range(address, Ppu::BottomLeftNametableAddressRange) ||
        in_range(address, Ppu::BottomRightNametableAddressRange)
      ){
        nametable_index = 1;
      }
    }

    return nametables[nametable_index][address & 0x03FF];
  }
  else if (in_range(address, Ppu::PalettesAddressRange)){
    address &= 0x001F; 
    if (address >= 0x0010 && (address & 3) == 0){
      address -= 16;
    }

    return palettes[address];
  }

  return 0x00;
}

auto Ppu::mem_write(Nes& nes, u16 address, u8 value) -> void{
  if (nes.cardridge.write_pattern(address, value)){
  }
  else if (in_range(address, Ppu::NametablesAddressRange)){
    address &= 0x0FFF;
    auto nametable_index = 0;

    if (nes.cardridge.mirroring() == Cardridge::Mirroring::Vertical){
      if (
        in_range(address, Ppu::TopRightNametableAddressRange) ||
        in_range(address, Ppu::BottomRightNametableAddressRange)
      ){
        nametable_index = 1;
      }
    }
    else if (nes.cardridge.mirroring() == Cardridge::Mirroring::Horizontal){
      if (
        in_range(address, Ppu::BottomLeftNametableAddressRange) ||
        in_range(address, Ppu::BottomRightNametableAddressRange)
      ){
        nametable_index = 1;
      }
    }

    nametables[nametable_index][address & 0x03FF] = value;
  }
  else if (in_range(address, Ppu::PalettesAddressRange)){
    palettes_started_loading = true;

    address &= 0x001F; 
    if (address >= 0x0010 && (address & 3) == 0){
      address -= 16;
    }

    palettes[address] = value;
  }
}

static auto ppu_increment_address(Ppu& ppu){
  const auto increment_mode = ppu.control & Ppu::Control::IncrementMode;
  ppu.vram_address.data += increment_mode ? 32 : 1;
}

auto Ppu::cpu_read(const Nes& nes, u16 address) -> u8{
  address &= 0x0007;

  switch(address){
    case CpuStatusPort:{
      const auto status = (this->status & 0b11100000) | (data_buffer & 0b00011111);
      this->status &= ~Ppu::Status::VBlank;
      address_latch = Ppu::AddressLatch::MSB;
      return status;
    }

    case CpuOAMDataPort:
      return reinterpret_cast<u8*>(oam)[oam_address];

    case CpuDataPort:{
      auto data = data_buffer;
      data_buffer = mem_read(nes, vram_address.data); 

      if (vram_address.data >= Ppu::PalettesAddressRange.first){
        data = data_buffer;
      }

      ppu_increment_address(*this);
      return data;
    }
  }

  return 0x00;
}

auto Ppu::cpu_write(Nes& nes, u16 address, u8 value) -> void{
  address &= 0x0007;
  switch(address){
    case CpuControlPort:
      control = value;
      tram_address.props.nametable_x = control & Control::NametableX;
      tram_address.props.nametable_y = control & Control::NametableY;
      break;

    case CpuMaskPort:
      mask = value;
      break;

    case CpuOAMAddressPort:
      oam_address = value;
      break;

    case CpuOAMDataPort:
      reinterpret_cast<u8*>(oam)[oam_address] = value;
      break;

    case CpuScrollPort:
      if (address_latch == Ppu::AddressLatch::LSB){
        address_latch = Ppu::AddressLatch::MSB;

        tram_address.props.cell_scroll_y = value & 0x07;
        tram_address.props.scroll_y = value >> 3;

      }
      else{
        address_latch = Ppu::AddressLatch::LSB;

        cell_scroll_x = value & 0x07;
        tram_address.props.scroll_x = value >> 3;
      }
      break;

    case CpuAddressPort: {
      if (address_latch == Ppu::AddressLatch::LSB){
        tram_address.data = (tram_address.data & 0xFF00) | value;
        vram_address.data = tram_address.data;
        address_latch = Ppu::AddressLatch::MSB;
      }
      else{
        tram_address.data = (tram_address.data & 0x00FF) | u16((value & 0x3F) << 8);
        address_latch = Ppu::AddressLatch::LSB;
      }
      break;
    }
    case CpuDataPort:
      mem_write(nes, vram_address.data, value);
      ppu_increment_address(*this);
      break;
  }
}

static auto ppu_increment_scroll_x(Ppu& ppu){
  if (ppu.mask & Ppu::Mask::RenderBackground || ppu.mask & Ppu::Mask::RenderSprites){
    auto& vram = ppu.vram_address.props;
    if (vram.scroll_x == 31){
      vram.scroll_x = 0;
      vram.nametable_x ^= 1;
    }
    else{
      vram.scroll_x++;
    }
  }
}

static auto ppu_increment_scroll_y(Ppu& ppu){
  if ((ppu.mask & Ppu::Mask::RenderBackground) || (ppu.mask & Ppu::Mask::RenderSprites)){
    auto& vram = ppu.vram_address.props;
    if (vram.cell_scroll_y < 7){
      vram.cell_scroll_y++;
      return;
    }

    vram.cell_scroll_y = 0;

    if (vram.scroll_y == 29){
      vram.scroll_y = 0;
      vram.nametable_y ^= 1;
    }
    else if (vram.scroll_y == 31){
      vram.scroll_y = 0;
    }
    else{
      vram.scroll_y++;
    }
  }
}

static auto ppu_transfer_address_x(Ppu& ppu){
  if (ppu.mask & Ppu::Mask::RenderBackground || ppu.mask & Ppu::Mask::RenderSprites){
    auto& vram = ppu.vram_address.props;
    auto& tram = ppu.tram_address.props;

    vram.nametable_x = tram.nametable_x;
    vram.scroll_x = tram.scroll_x;
  }
}

static auto ppu_transfer_address_y(Ppu& ppu){
  if (ppu.mask & Ppu::Mask::RenderBackground || ppu.mask & Ppu::Mask::RenderSprites){
    auto& vram = ppu.vram_address.props;
    auto& tram = ppu.tram_address.props;

    vram.cell_scroll_y = tram.cell_scroll_y;
    vram.nametable_y = tram.nametable_y;
    vram.scroll_y = tram.scroll_y;
  }
}

static auto ppu_load_shifters(Ppu& ppu){
  ppu.bg_shifter_pattern_low = (ppu.bg_shifter_pattern_low & 0xFF00) | ppu.bg_next_tile_lsb;
  ppu.bg_shifter_pattern_high = (ppu.bg_shifter_pattern_high & 0xFF00) | ppu.bg_next_tile_msb;

  ppu.bg_shifter_attribute_low = (ppu.bg_shifter_attribute_low & 0xFF00) | ((ppu.bg_next_tile_attribute & 0b01) ? 0xFF : 0x00);
  ppu.bg_shifter_attribute_high = (ppu.bg_shifter_attribute_high & 0xFF00) | ((ppu.bg_next_tile_attribute & 0b10) ? 0xFF : 0x00);
}

static auto ppu_update_shifters(Ppu& ppu){
  if (ppu.mask & Ppu::Mask::RenderBackground){
    ppu.bg_shifter_pattern_low <<= 1;
    ppu.bg_shifter_pattern_high <<= 1;
    ppu.bg_shifter_attribute_low <<= 1;
    ppu.bg_shifter_attribute_high <<= 1;
  }

  if ((ppu.mask & Ppu::Mask::RenderSprites) && in_range(ppu.cycles, std::make_pair(1, Ppu::ScreenSize.x + 1))){
    for (auto i : gfm::range(ppu.scanline_sprites_count)){
      if (ppu.sprites_on_scanline[i].x > 0){
        ppu.sprites_on_scanline[i].x--;
      }
      else{
        ppu.sprite_shifter_pattern_low[i] <<= 1;
        ppu.sprite_shifter_pattern_high[i] <<= 1;
      }
    }
  }
}

auto Ppu::clock(const Nes& nes) -> void{
  frame_complete = false;

  if (in_range(scanline, std::make_pair(-1, ScreenSize.y - 1))){
    if (scanline == -1 && cycles == 1){
      status &= ~Status::VBlank;
      status &= ~Status::SpriteOverflow;

      for (auto i : gfm::range(MaxSpritesOnScanline)){
        sprite_shifter_pattern_low[i] = 0;
        sprite_shifter_pattern_high[i] = 0;
      }
    }

    if (
      in_range(cycles, std::make_pair(2, 257)) ||
      in_range(cycles, std::make_pair(321, 337))
    ){
      ppu_update_shifters(*this);

      const auto background_pattern = (control & Control::BackgroundPattern) > 0;
      switch((cycles - 1) % 8){
        case 0:
          ppu_load_shifters(*this);
          bg_next_tile_id = mem_read(nes, 
            NametablesAddressRange.first |
            (vram_address.data & 0x0FFF)
          );

          break;
        case 2:
          bg_next_tile_attribute = mem_read(nes, 
            (NametablesAddressRange.first + 32 * 30)
            | (vram_address.props.nametable_y << 11)
            | (vram_address.props.nametable_x << 10)
            | ((vram_address.props.scroll_y >> 2) << 3)
            | (vram_address.props.scroll_x >> 2)
          );

          if (vram_address.props.scroll_y & 0x02) bg_next_tile_attribute >>= 4;
          if (vram_address.props.scroll_x & 0x02) bg_next_tile_attribute >>= 2;
          bg_next_tile_attribute &= 0x03;

          break;
        case 4:
          bg_next_tile_lsb = mem_read(nes, 
            (background_pattern << 12) +
            (u16(bg_next_tile_id) << 4) +
            vram_address.props.cell_scroll_y
          );
          break;
        case 6:
          bg_next_tile_msb = mem_read(nes, 
            (background_pattern << 12) +
            (u16(bg_next_tile_id) << 4) +
            vram_address.props.cell_scroll_y + 8
          );
          break;
        case 7:
          ppu_increment_scroll_x(*this);
          break;
      }
    }

    if (cycles == ScreenSize.x){
      ppu_increment_scroll_y(*this);
    }

    if (cycles == ScreenSize.x + 1){
      ppu_load_shifters(*this);
      ppu_transfer_address_x(*this);
    }

    if (scanline == -1 && in_range(cycles, std::make_pair(280, 304))){
      ppu_transfer_address_y(*this);
    }

    //Rendering Foreground

    if (cycles == 257 && scanline >= 0){
      std::memset(sprites_on_scanline, 0xFF, sizeof(sprites_on_scanline));
      scanline_sprites_count = 0;

      for (int i = 0; i < 64 && scanline_sprites_count < 9; ++i){
        const auto y_diff = (scanline - i16(oam[i].y));

        const auto sprite_height = (control & Control::SpriteSize) > 0 ? 16 : 8;
        if (in_range(y_diff, std::make_pair(0, sprite_height - 1))){
          if (scanline_sprites_count < 8){
            sprites_on_scanline[scanline_sprites_count] = oam[i];
          }
          if (scanline_sprites_count < 9){
            scanline_sprites_count++;
          }
        }
      }

      if (scanline_sprites_count > 8){
        status |= Status::SpriteOverflow;
        scanline_sprites_count = 8;
      }
      else{
        status &= ~Status::SpriteOverflow;
      }

    }

    if (cycles == 340){
      u16 sprite_pattern_address_low = 0;

      for (auto i : gfm::range(scanline_sprites_count)){
        if ((control & Control::SpriteSize) == 0){
          const auto sprite_pattern = (control & Control::SpritePattern) > 0;
          //Check if not flipped:
          if ((sprites_on_scanline[i].attribute & 0x80) == 0){
            sprite_pattern_address_low = 
                (sprite_pattern << 12) 
              | (sprites_on_scanline[i].id << 4)
              | (scanline - sprites_on_scanline[i].y);
          }
          else{
            sprite_pattern_address_low = 
                (sprite_pattern << 12) 
              | (sprites_on_scanline[i].id << 4)
              | (7 - (scanline - sprites_on_scanline[i].y));
          }
        }
        else{
          if ((sprites_on_scanline[i].attribute & 0x80) == 0){
            //Top half:
            if (scanline - sprites_on_scanline[i].y < 8){
              sprite_pattern_address_low = 
                  ((sprites_on_scanline[i].id & 0x01) << 12)
                | (((sprites_on_scanline[i].id & 0xFE)) << 4)
                | ((scanline - sprites_on_scanline[i].y) & 0x07);
            }
            //Bottom half:
            else{
              sprite_pattern_address_low = 
                  ((sprites_on_scanline[i].id & 0x01) << 12)
                | (((sprites_on_scanline[i].id & 0xFE) + 1) << 4)
                | ((scanline - sprites_on_scanline[i].y) & 0x07);
            }
          }
          else{
            //Top half:
            if (scanline - sprites_on_scanline[i].y < 8){
              sprite_pattern_address_low = 
                  ((sprites_on_scanline[i].id & 0x01) << 12)
                | (((sprites_on_scanline[i].id & 0xFE) + 1) << 4)
                | (7 - (scanline - sprites_on_scanline[i].y) & 0x07);
            }
            //Bottom half:
            else{
              sprite_pattern_address_low = 
                  ((sprites_on_scanline[i].id & 0x01) << 12)
                | (((sprites_on_scanline[i].id & 0xFE)) << 4)
                | (7 - (scanline - sprites_on_scanline[i].y) & 0x07);
            }
          }
        }

        u16 sprite_pattern_address_high = sprite_pattern_address_low + 8;
        u8 sprite_pattern_data_low = mem_read(nes, sprite_pattern_address_low);
        u8 sprite_pattern_data_high = mem_read(nes, sprite_pattern_address_high);

        if (sprites_on_scanline[i].attribute & 0x40){
          sprite_pattern_data_low = flip_byte(sprite_pattern_data_low);
          sprite_pattern_data_high = flip_byte(sprite_pattern_data_high);
        }

        sprite_shifter_pattern_low[i] = sprite_pattern_data_low;
        sprite_shifter_pattern_high[i] = sprite_pattern_data_high;
      }


    }
  }

  if (in_range(scanline, std::make_pair(ScreenSize.y + 1, MaxScanlines - 1))){
    if (scanline == ScreenSize.y + 1 && cycles == 1){
      status |= Ppu::Status::VBlank;

      if (control & Control::EnableNmi){
        nmi = true;
      }
    }
  }

  u8 bg_palette = 0;
  u8 bg_pixel = 0;
  if (mask & Mask::RenderBackground){
    assert(cell_scroll_x < 8);
    u16 bit_mux = 0x8000 >> cell_scroll_x;

    u8 p0_pixel = (bg_shifter_pattern_low & bit_mux) > 0;
    u8 p1_pixel = (bg_shifter_pattern_high & bit_mux) > 0;
    bg_pixel = (p1_pixel << 1) | p0_pixel;

    u8 palette0 = (bg_shifter_attribute_low & bit_mux) > 0;
    u8 palette1 = (bg_shifter_attribute_high & bit_mux) > 0;
    bg_palette = (palette1 << 1) | palette0;
  }

  u8 fg_pixel = 0;
  u8 fg_palette = 0;
  u8 fg_priority = 0;

  if (mask & Mask::RenderSprites){
    for (auto i : gfm::range(scanline_sprites_count)){
      if (sprites_on_scanline[i].x == 0){
        u8 fg_pixel_low = (sprite_shifter_pattern_low[i] & 0x80) > 0;
        u8 fg_pixel_high = (sprite_shifter_pattern_high[i] & 0x80) > 0;
        fg_pixel = (fg_pixel_high << 1) | fg_pixel_low;

        fg_palette = (sprites_on_scanline[i].attribute & 0x03) + 0x04; 
        fg_priority = (sprites_on_scanline[i].attribute & 0x20) == 0;

        if (fg_pixel != 0) break;
      }
    }
  }

  u8 pixel = 0;
  u8 palette = 0;

  if (fg_pixel == 0 && bg_pixel == 0){
    pixel = 0;
    palette = 0;
  }
  else if (fg_pixel > 0 && bg_pixel == 0){
    pixel = fg_pixel;
    palette = fg_palette;
  }
  else if (fg_pixel == 0 && bg_pixel > 0){
    pixel = bg_pixel;
    palette = bg_palette;
  }
  else if (fg_pixel > 0 && bg_pixel > 0){
    if (fg_priority){
      pixel = fg_pixel;
      palette = fg_palette;
    }
    else{
      pixel = bg_pixel;
      palette = bg_palette;
    }
  }

  renderer.set_pixel(
    gfm::vec2(cycles - 1, scanline), 
    colors[mem_read(nes, PalettesAddressRange.first + (palette << 2) + pixel) & 0x3F]
  );

  cycles++;

  if (cycles > Ppu::CyclesPerScanline){
    cycles = 0;
    scanline++;

    if (scanline > Ppu::MaxScanlines){
      scanline = -1;
      frame_complete = true;
    }
  }

}

} //namespace nes
