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
  struct Pixel{
    gfm::vec2 position;
    gfm::vec3 color;
  };

  enum class ShaderType{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
  };

  GLuint vertex_shader, fragment_shader;
  GLuint shader_program;
  GLuint vao, vbo, vbo_instanced;
  bool initialised = false;

  gfm::vec2 buffer_size;
  std::vector<Pixel> pixels;

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
    this->buffer_size = buffer_size;
    pixels.resize(buffer_size.x * buffer_size.y);
    for (auto y : gfm::range(buffer_size.y)){
      for (auto x : gfm::range(buffer_size.x)){
        pixels[y * buffer_size.x + x].position = gfm::vec2(x, y);
      }
    }
    
    //Create VAO
    auto vbo_data = std::array{
      0.f, 0.f, 
      1.f, 0.f,
      1.f, 1.f,
      0.f, 0.f,
      0.f, 1.f,
      1.f, 1.f,
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

    vao_add_attrib_ptr(0, 2, 0, 2);

    //Instanced array
    glGenBuffers(1, &vbo_instanced);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instanced);
    

    vao_add_attrib_ptr(1, 2, 0, 5);
    vao_add_attrib_ptr(2, 3, 2, 5);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);

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

    initialised = true;
  }

  ~Renderer(){
    if (!initialised) return;

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &vbo_instanced);
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

  auto set_pixel(const gfm::vec2& position, const gfm::vec3& color){
    const auto [x, y] = position;

    if (!in_range(x, std::make_pair(0, buffer_size.x - 1))) return;
    if (!in_range(y, std::make_pair(0, buffer_size.y - 1))) return;

    pixels[y * buffer_size.x + x].color = color;
  }

  auto render(){
    glBindBuffer(GL_ARRAY_BUFFER, vbo_instanced);
    glBufferData(
      GL_ARRAY_BUFFER, 
      sizeof(Pixel) * pixels.size(),
      pixels.data(),
      GL_STATIC_DRAW
    );

    glDrawArraysInstanced(GL_TRIANGLES, 0, 6, pixels.size());
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
