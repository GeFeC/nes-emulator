#pragma once

#include <glad/glad.h>
#include "math.hpp"
#include "vertex_shader.hpp"
#include "fragment_shader.hpp"
#include "../util.hpp"

#include <string>
#include <stdexcept>
#include <array>
#include <iostream>

namespace nes{

namespace gfm = gf::math;

struct Renderer{
  enum class ShaderType{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
  };

  using pixel_color = gfm::vec<u8, 3>;

  GLuint vertex_shader, fragment_shader;
  GLuint shader_program;
  GLuint vao, vbo;
  GLuint screen_texture;
  bool initialised = false;

  gfm::vec2 buffer_size;
  std::vector<pixel_color> pixels;
  std::vector<pixel_color> pixels_buffer;

  static auto create_shader(const char* script, Renderer::ShaderType type){
    const auto shader = glCreateShader(static_cast<int>(type));
    glShaderSource(shader, 1, &script, nullptr);
    glCompileShader(shader);

    auto success = int{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success) {
      return shader;
    }

    static constexpr auto ErrorMessageBufferSize = 512;

    auto msg = std::array<char, ErrorMessageBufferSize>{};
    glGetShaderInfoLog(shader, ErrorMessageBufferSize, NULL, msg.data());
    
    throw std::runtime_error("Failed to compile shader with error:\n" + std::string(msg.data()));

    return 0u;
  }

  Renderer() = default;

  auto init(const gfm::vec2& buffer_size){
    static_assert(sizeof(gfm::vec<u8, 3>) == 3);

    this->buffer_size = buffer_size;
    pixels.resize(buffer_size.x * buffer_size.y);
    pixels_buffer.resize(pixels.size());
    
    //Create VAO
    auto vbo_data = std::array{
      0.f, 0.f, 0.f, 0.f,
      1.f, 0.f, 1.f, 0.f,
      1.f, 1.f, 1.f, 1.f,
      0.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 1.f,
      1.f, 1.f, 1.f, 1.f
    };

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glBufferData(
      GL_ARRAY_BUFFER, 
      vbo_data.size() * sizeof(float), 
      vbo_data.data(), GL_STATIC_DRAW
    );

    vao_add_attrib_ptr(0, 2, 0, 4);
    vao_add_attrib_ptr(1, 2, 2, 4);

    glGenTextures(1, &screen_texture);
    glBindTexture(GL_TEXTURE_2D, screen_texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    auto vec = std::vector<u8>(256 * 240 * 3, 255);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 256, 240, 0, GL_RGB, GL_UNSIGNED_BYTE, vec.data());

    //Create ShaderProgram
    vertex_shader = create_shader(shaders::vertex_shader_script, Renderer::ShaderType::Vertex);
    fragment_shader = create_shader(shaders::fragment_shader_script, Renderer::ShaderType::Fragment);

    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);

    glLinkProgram(shader_program);

    auto success = GLint{};
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    
    if (!success){
      static constexpr auto ErrorMessageBufferSize = 512;
      auto msg = std::array<char, ErrorMessageBufferSize>{};
      glGetProgramInfoLog(shader_program, ErrorMessageBufferSize, nullptr, msg.data());

      throw std::runtime_error("Failed to link shader with error:\n" + std::string{ msg.data() } );
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);
    glUseProgram(shader_program);

    set_uniform("projection", gfm::ortho(0.f, buffer_size.x, 0.f, buffer_size.y, 0.1f, 1000.f));
    set_uniform("model", gfm::scale(gfm::vec3(buffer_size.x, buffer_size.y, 1.f)));

    initialised = true;
  }

  ~Renderer(){
    if (!initialised) return;

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shader_program);
  }

  auto set_uniform(const std::string& name, const gfm::mat4& matrix) -> void{
    glUniformMatrix4fv(
      glGetUniformLocation(shader_program, name.c_str()), 
      1, 
      GL_FALSE, 
      &matrix[0][0]
    );
  }

  auto set_uniform(const std::string& name, const gfm::vec4& vec) -> void{
    glUniform4fv(
      glGetUniformLocation(shader_program, name.c_str()), 
      1, 
      &vec.x
    );
  }

  auto set_pixel(const gfm::vec2& position, const pixel_color& color){
    const auto [x, y] = position;

    if (!in_range(x, std::make_pair(0, buffer_size.x - 1))) return;
    if (!in_range(y, std::make_pair(0, buffer_size.y - 1))) return;

    auto& pixel = pixels_buffer[y * buffer_size.x + x] = color;
  }

  auto update_pixels(){
    pixels = pixels_buffer;
  }

  auto render(){
    glBindTexture(GL_TEXTURE_2D, screen_texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 256, 240, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

private:
  auto vao_add_attrib_ptr(GLuint index, GLuint size, GLuint position, GLuint stride) noexcept -> void{
    glVertexAttribPointer(
      index, 
      size, 
      GL_FLOAT, 
      GL_FALSE, 
      stride * sizeof(float), 
      (void*)(position * sizeof(float))
    );

    glEnableVertexAttribArray(index);
  }

};
} //namespace nes
