#pragma once

#include "aliases.hpp"
#include "audio.hpp"

namespace nes{

struct Nes;

struct Sequencer{
  u16 reload = 0;
  u16 timer = 0;
  u16 sequence = 0x0;

  template<typename Callable>
  auto clock(bool enabled, Callable callable){
    if(!enabled) return;

    timer--;
    if (timer == 0xFFFF){
      sequence = callable(sequence);
      timer = reload;
    }
  }

  auto clock(bool enabled){
    return clock(enabled, [](auto e){ return e; });
  }

  auto update_timer_first_8_bits(u8 data){
    reload = (reload & 0xFF00) | data;
  }

  auto update_timer_last_3_bits(u8 data){
    reload = (reload & 0x00FF) | (u16(data & 7) << 8);
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

  auto update(u8 data){
    const_volume = data & 0x10;
    max_volume = data & 0x0F;
    loop = data & 0x20;
  }

  auto clock(){
    if (delay_countdown){
      delay_countdown--;
      return;
    }

    delay_countdown = delay;

    if (volume){
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

  auto update(u8 data){
    enabled = data & 0x80;
    period = (data & 0x70) >> 4;
    negate = data & 0x08;
    shift = data & 0x07;
    reload();
  }

  auto clock(i16 reload, bool pulse1 = false) -> i16{
    if (counter == 0 && enabled && shift > 0){
      auto change = reload >> shift;
      if (reload < 8) { return reload; }
      if (change > 0x7FF) { return reload; }

      if (negate){
        reload -= change - pulse1;
      }
      else reload += change;
    }

    if (enabled){
      if (counter == 0){
        counter = period;
      }
      else counter--;
    }

    return reload;
  }

  auto reload() -> void{
    counter = period;
  }
};

struct LengthCounter{
  u8 counter = 0;

  auto update(u8 data){
    static constexpr u8 LengthTable[32] = {
      10, 254, 20,  2, 40,  4, 80,  6,
      160, 8, 60, 10, 14, 12, 26, 14,
      12, 16, 24, 18, 48, 20, 96, 22,
      192, 24, 72, 26, 16, 28, 32, 30
    };

    counter = LengthTable[data >> 3];
  }

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

  auto output(float time){
    const auto freq = 1789773.0 / (16.0 * double(sequencer.reload + 1));
    if (length_counter.counter == 0) return 0.f;
    if (sequencer.timer < 8) return 0.f;
    if (envelope.output() <= 2) return 0.f;
    if (!enabled) return 0.f;

    return (nes::square_wave(time, freq, duty) * envelope.output());
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
};

struct NoiseChannel{
  Envelope envelope;
  LengthCounter length_counter;
  Sequencer sequencer;
  bool enabled = false;
  bool mode = 0;

  NoiseChannel(){
    sequencer.sequence = 0x7FFF;
  }

  auto clock(bool quarter_frame_clock, bool half_frame_clock){
    if (quarter_frame_clock){
      envelope.clock();
    }

    if (half_frame_clock){
      length_counter.clock(enabled, envelope.loop);
    }

    sequencer.clock(enabled, [](u16 sequence){
      const auto bit0 = sequence & 1;
      const auto bit1 = (sequence & 2) >> 1;

      return ((bit0 ^ bit1) << 14) | (sequence >> 1);
    });
  }

  auto output(){
    if (!enabled || length_counter.counter == 0) return 0.f;
    return ((~sequencer.sequence) & 1 ? 1.f : -1.f) * envelope.output();
  }
};

struct Apu{
  AudioPlayer<Nes> sound;
  PulseChannel pulse1;
  PulseChannel pulse2;
  NoiseChannel noise;
  u32 cycles = 0;
  u32 frame_cycles = 0;

  Apu(Nes& nes){
    sound.init(nes);
    pulse1.pulse1 = true;
  }

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
        pulse1.envelope.update(data);
        break;

      case 0x4001:
        pulse1.sweep.update(data);
        break;

      case 0x4002:
        pulse1.sequencer.update_timer_first_8_bits(data);
        break;

      case 0x4003:
        pulse1.sequencer.update_timer_last_3_bits(data);
        pulse1.envelope.start();
        pulse1.length_counter.update(data);
        break;

      case 0x4004:
        pulse2.update_duty(data);
        pulse2.envelope.update(data);
        break;

      case 0x4005:
        pulse2.sweep.update(data);
        break;

      case 0x4006:
        pulse2.sequencer.update_timer_first_8_bits(data);
        break;

      case 0x4007:
        pulse2.sequencer.update_timer_last_3_bits(data);
        pulse2.envelope.start();
        pulse2.length_counter.update(data);
        break;

      case 0x400C:
        noise.envelope.update(data);
        break;

      case 0x400E: {
        u16 noise_reload_table[16] = {
          4, 8, 16, 32, 64, 96, 128, 160,
          202, 254, 380, 508, 1016, 2034, 4068, 0
        };

        noise.sequencer.reload = noise_reload_table[data & 0x0F];
        break;
      }

      case 0x4015:
        pulse1.enabled = data & 1;
        pulse2.enabled = data & 2;
        noise.enabled = data & 4;

        if (!pulse1.enabled) pulse1.length_counter.counter = 0;
        if (!pulse2.enabled) pulse2.length_counter.counter = 0;
        if (!noise.enabled) noise.length_counter.counter = 0;
        break;

      case 0x400F:
        noise.length_counter.update(data);
        noise.envelope.start();
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
      noise.clock(quarter_frame_clock, half_frame_clock);
    }
    
    cycles++;
  }

  auto play(AudioPlayer<Nes>::callback_t callback){
    sound.play(callback);
  }

};

} //namespace nes
