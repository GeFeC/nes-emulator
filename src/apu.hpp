#pragma once

#include "aliases.hpp"
#include "audio.hpp"

namespace nes{

struct Nes;

struct Sequencer{
  u16 reload = 0;
  u16 timer = 0;

  auto clock(bool enabled){
    if(!enabled) return;

    timer--;
    if (timer == 0xFFFF){
      timer = reload;
    }
  }
};

struct Envelope{
  bool const_volume = false;
  bool loop = false;
  bool started = false;

  union{
    u8 max_volume = 0;
    u8 delay;
  };

  u8 volume = 0;
  u8 delay_countdown = 0;

  auto clock(){
    if (delay_countdown){
      delay_countdown--;
      return;
    }

    delay_countdown = delay; 

    if (volume > 0){
      volume--;
    }
    else if (loop){
      volume = 15;
    }
  }

  auto output(){
    return const_volume ? max_volume : volume;
  }

  auto start(){
    volume = 15;
    delay_countdown = delay;
  }
};

struct Sweep{
  bool enabled = false;
  u8 shift = 0;
  bool negate = false;
  u8 period = 0;
  i16 counter = 0;
  bool disable_channel = false;

  auto clock(i16 timer, bool pulse1 = false) -> i16{
    if (!enabled) return timer;
    if (disable_channel) return timer;

    counter--;
    if (counter == 0){
      counter = period;

      auto change = timer >> shift;
      if (timer + change < 8) return timer;
      if (timer + change > 0x7FF) return timer;

      if (negate){
        timer -= change;

        if (!pulse1){
          timer++;
        }
      }
      else timer += change;
    }

    return timer;
  }

  auto reload(){
    counter = period;
  }
};

struct LengthCounter{
  u8 counter = 0;

  auto clock(bool enabled, bool stop){
    if (!enabled){
      counter = 0;
      return counter;
    }

    if (counter > 0 && !stop){
      counter--;
    }

    return counter;
  }
};

struct PulseChannel{
  Sequencer sequencer;
  Envelope envelope;
  Sweep sweep;
  LengthCounter length_counter;
  bool enabled = false;
  double duty;

  bool pulse1 = false;

  auto square_wave(float time){
    const auto freq = 1789773.0 / (16.0 * double(sequencer.reload + 1));
    if (length_counter.counter == 0) return 0.f;
    if (sequencer.timer < 8) return 0.f;
    if (envelope.output() <= 2) return 0.f;
    if (!enabled) return 0.f;

    return (nes::square_wave(time, freq, duty) * envelope.output() / 16.f);
  }

  auto clock(bool quarter_frame_clock, bool half_frame_clock){
    if (quarter_frame_clock){
      envelope.clock();
    }

    if (half_frame_clock){
      sequencer.reload = sweep.clock(sequencer.reload, pulse1);
      length_counter.clock(enabled, envelope.loop);
    }

    sequencer.clock(enabled);
  }

  auto update_duty(u8 data){
    switch(data >> 6){
      case 0x00: duty = 0.125; break;
      case 0x01: duty = 0.25; break;
      case 0x02: duty = 0.5; break;
      case 0x03: duty = 0.75; break;
    }
  }

  auto update_envelope(u8 data){
    envelope.const_volume = data & 0x10;
    envelope.max_volume = data & 0x0F;
    envelope.loop = data & 0x20;
  }

  auto update_sweep(u8 data){
    sweep.enabled = data & 0x80;
    sweep.period = (data & 0x70) >> 4;
    sweep.negate = data & 0x08;
    sweep.shift = data & 0x07;
    sweep.reload();
  }

  auto update_timer_first_8_bits(u8 data){
    sequencer.reload = (sequencer.reload & 0xFF00) | data;
  }

  auto update_timer_last_3_bits(u8 data){
    sequencer.reload = (sequencer.reload & 0x00FF) | (u16(data & 7) << 8);
  }

  auto update_length_counter(u8 data){
    static constexpr u8 LengthTable[32] = {
      10, 254, 20,  2, 40,  4, 80,  6,
      160, 8, 60, 10, 14, 12, 26, 14,
      12, 16, 24, 18, 48, 20, 96, 22,
      192, 24, 72, 26, 16, 28, 32, 30
    };

    length_counter.counter = LengthTable[data >> 3];
  }
};

struct Apu{
  Apu(Nes& nes){
    sound.init(nes);
    pulse1.pulse1 = true;
  }

  Sound<Nes> sound;
  PulseChannel pulse1;
  PulseChannel pulse2;
  u32 cycles = 0;
  u32 frame_cycles = 0;

  auto cpu_read(u16 address){
    switch(address){
      case 0x4015:
        u8 status = 0;

        if (pulse1.length_counter.counter > 0) status |= 0x01;
        if (pulse2.length_counter.counter > 0) status |= 0x02;

        return status;
    }

    return u8(0);
  }

  auto cpu_write(u16 address, u8 data){
    switch(address){
      case 0x4000:
        pulse1.update_duty(data);
        pulse1.update_envelope(data);
        break;

      case 0x4001:
        pulse1.update_sweep(data);
        break;

      case 0x4002:
        pulse1.update_timer_first_8_bits(data);
        break;

      case 0x4003:
        pulse1.update_timer_last_3_bits(data);
        pulse1.envelope.start();
        pulse1.update_length_counter(data);
        break;

      case 0x4004:
        pulse2.update_duty(data);
        pulse2.update_envelope(data);
        break;

      case 0x4005:
        pulse2.update_sweep(data);
        break;

      case 0x4006:
        pulse2.update_timer_first_8_bits(data);
        break;

      case 0x4007:
        pulse2.update_timer_last_3_bits(data);
        pulse2.envelope.start();
        pulse2.update_length_counter(data);
        break;

      case 0x4015:
        pulse1.enabled = data & 1;
        pulse2.enabled = data & 2;
        
        if (!pulse1.enabled) pulse1.length_counter.counter = 0;
        if (!pulse2.enabled) pulse2.length_counter.counter = 0;
        break;

      case 0x400F:
        break;
    }
  }

  auto clock(){
    bool quarter_frame_clock = false;
    bool half_frame_clock = false;

    if (cycles % 6 == 0){
      frame_cycles++;

      switch(frame_cycles){
        case 14916:
          frame_cycles = 0;
        case 7457:
          half_frame_clock = true;
        case 3729:
        case 11186:
          quarter_frame_clock = true;
      }

      pulse1.clock(quarter_frame_clock, half_frame_clock);
      pulse2.clock(quarter_frame_clock, half_frame_clock);
    }
    
    cycles++;
  }

  auto play(Sound<Nes>::callback_t callback){
    sound.play(callback);
  }

};

} //namespace nes
