#pragma once

#include "aliases.hpp"
#include "util.hpp"
#include <miniaudio.h>
#include <stdexcept>
#include <functional>

namespace nes{

inline auto square_wave(double step, double frequency, float duty){
  static constexpr auto Harmonics = 20;
  auto y1 = 0.f;
  for (auto i : range(1, Harmonics)){
    y1 += fast_sin(step * frequency * Pi * 2 * i) / i;
  }

  auto y2 = 0.f;
  for (auto i : range(1, Harmonics)){
    y2 += fast_sin((step * frequency - duty) * Pi * 2 * i) / i;
  }

  return 0.5f * (y1 - y2);
}

template<typename Data>
struct AudioPlayer{
  using callback_t = std::function<float(Data&)>; 
  inline static callback_t current_callback;
  inline static auto cycles = u32(0);

  ma_device_config config;
  ma_device device;

  AudioPlayer(double sample_rate){
    config = ma_device_config_init(ma_device_type_playback);

    config.playback.format = ma_format_f32;
    config.playback.channels = 1;
    config.sampleRate = static_cast<ma_uint32>(sample_rate);

    config.dataCallback = [](ma_device* device, void* output, const void*, ma_uint32 frame_count){
      auto out = static_cast<float*>(output);
      auto& data = *static_cast<Data*>(device->pUserData);

      for (ma_uint32 i = 0; i < frame_count; i++) {
        *out++ = AudioPlayer::current_callback(data);
        cycles++;
      }
    };
  }

  auto init(Data& data){
    config.pUserData = &data;

    if (ma_device_init(NULL, &config, &device) != MA_SUCCESS) {
      throw std::runtime_error("Unable to initialise miniaudio device");
    }
  }

  AudioPlayer() : AudioPlayer(44100.0) {}

  auto play(callback_t callback){
    AudioPlayer::current_callback = callback;
    
    ma_device_start(&device);
  }

  auto stop(){
    ma_device_stop(&device);
  }

  ~AudioPlayer(){
    stop();
    ma_device_uninit(&device);
  }
};

} //namespace nes
