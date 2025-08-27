#pragma once

#include "texture.hpp"
#include "math.hpp"
#include "vertex_shader.hpp"
#include "fragment_shader.hpp"
#include "../util.hpp"

#include <glad/glad.h>
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

  GLuint vertex_shader, fragment_shader;
  GLuint shader_program;
  GLuint vao, vbo;
  bool initialised = false;

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

  Renderer(const gfm::vec2& view_size){
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

    set_uniform("projection", gfm::ortho(0.f, view_size.x, 0.f, view_size.y, 0.1f, 1000.f));

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

  auto render_texture(const Texture& texture, const gfm::vec2& position){
    const auto [x, y] = position;
    const auto [w, h] = texture.size;
    const auto model = gfm::scale(gfm::vec3(w, h, 1.f)) * gfm::translation(gfm::vec3(x, y, 0.f));
    set_uniform("model", model);

    glBindTexture(GL_TEXTURE_2D, texture.id);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, texture.pixels.data());
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
