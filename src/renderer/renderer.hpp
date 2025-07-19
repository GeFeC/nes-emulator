#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "math.hpp"
#include "vertex_shader.hpp"
#include "fragment_shader.hpp"

#include <string>
#include <stdexcept>
#include <array>
#include <tuple>

namespace nes::renderer{

namespace gfm = gf::math;

struct Pixel{
  gfm::vec2 position; 
  gfm::vec4 color;
};

struct Renderer{
  enum class ShaderType{
    Vertex = GL_VERTEX_SHADER,
    Fragment = GL_FRAGMENT_SHADER
  };

  static constexpr auto VaoStride = 4;
  GLuint vertex_shader, fragment_shader;
  GLuint shader_program;
  GLuint vao, vbo;
};

inline auto create_shader(const char* script, Renderer::ShaderType type){
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

inline auto set_uniform(const Renderer& renderer, const std::string& name, const gfm::mat4& matrix){
  glUniformMatrix4fv(
    glGetUniformLocation(renderer.shader_program, name.c_str()), 
    1, 
    GL_FALSE, 
    &matrix[0][0]
  );
}

inline auto set_uniform(const Renderer& renderer, const std::string& name, const gfm::vec4& vec){
  glUniform4fv(
    glGetUniformLocation(renderer.shader_program, name.c_str()), 
    1, 
    &vec.x
  );
}

static auto vao_add_attrib_ptr(GLuint index, GLuint size, GLuint position) noexcept -> void{
  glVertexAttribPointer(
    index, 
    size, 
    GL_FLOAT, 
    GL_FALSE, 
    Renderer::VaoStride * sizeof(float), 
    (void*)(position * sizeof(float))
  );

  glEnableVertexAttribArray(index);
}

inline auto create_renderer(const gfm::vec2& buffer_size){
  auto renderer = Renderer();

  //Create VAO
  auto vbo_data = std::array{
    0.f, 0.f, 0.f, 0.f,
    1.f, 0.f, 1.f, 0.f,
    1.f, 1.f, 1.f, 1.f,
    0.f, 0.f, 0.f, 0.f,
    0.f, 1.f, 0.f, 1.f,
    1.f, 1.f, 1.f, 1.f
  };

  glGenVertexArrays(1, &renderer.vao);
  glBindVertexArray(renderer.vao);

  glGenBuffers(1, &renderer.vbo);
  glBindBuffer(GL_ARRAY_BUFFER, renderer.vbo);

  glBufferData(
    GL_ARRAY_BUFFER, 
    vbo_data.size() * sizeof(float), 
    vbo_data.data(), GL_STATIC_DRAW
  );

  vao_add_attrib_ptr(0, 2, 0);
  vao_add_attrib_ptr(1, 2, 2);

  //Create ShaderProgram
  renderer.vertex_shader = create_shader(vertex_shader_script, Renderer::ShaderType::Vertex);
  renderer.fragment_shader = create_shader(fragment_shader_script, Renderer::ShaderType::Fragment);

  renderer.shader_program = glCreateProgram();
  glAttachShader(renderer.shader_program, renderer.vertex_shader);
  glAttachShader(renderer.shader_program, renderer.fragment_shader);

  glLinkProgram(renderer.shader_program);

  auto success = GLint{};
  glGetProgramiv(renderer.shader_program, GL_LINK_STATUS, &success);
  
  if (!success){
    static constexpr auto ErrorMessageBufferSize = 512;
    auto msg = std::array<char, ErrorMessageBufferSize>{};
    glGetProgramInfoLog(renderer.shader_program, ErrorMessageBufferSize, nullptr, msg.data());

    throw std::runtime_error("Failed to link shader with error:\n" + std::string{ msg.data() } );
  }

  glDeleteShader(renderer.vertex_shader);
  glDeleteShader(renderer.fragment_shader);
  glUseProgram(renderer.shader_program);

  set_uniform(renderer, "projection", gfm::ortho(0.f, buffer_size.x, 0.f, buffer_size.y, 0.1f, 1000.f));

  return renderer;
}

inline auto destroy_renderer(Renderer* renderer){
  glDeleteVertexArrays(1, &renderer->vao);
  glDeleteBuffers(1, &renderer->vbo);
  glDeleteProgram(renderer->shader_program);
}

inline auto draw(const Renderer& renderer, const Pixel& pixel){
  const auto model = gfm::translation(pixel.position.as_vec<3>(0.f));
  set_uniform(renderer, "model", model);
  set_uniform(renderer, "color", pixel.color);

  glDrawArrays(GL_TRIANGLES, 0, 6);
}

} //namespace nes::renderer
