cmake_minimum_required(VERSION 3.15)

project(nes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CPP_FILES 
  src/cpu.cpp
  src/ppu.cpp
  vendor/glad.c
  vendor/stb.cpp
  vendor/miniaudio.cpp
)

add_executable(${PROJECT_NAME} 
  src/main.cpp
  ${CPP_FILES}
)

add_executable(${PROJECT_NAME}_test
  test/test.cpp
  ${CPP_FILES}
)

set(include_dirs
  vendor/glfw/include 
  vendor/include
  vendor/include/glad
)

set(libs 
  glfw
  pthread
)

target_include_directories(${PROJECT_NAME} PUBLIC ${include_dirs})
target_include_directories(${PROJECT_NAME}_test PUBLIC ${include_dirs})
target_link_libraries(${PROJECT_NAME} PUBLIC ${libs})
target_link_libraries(${PROJECT_NAME}_test PUBLIC ${libs})

target_compile_options(${PROJECT_NAME} PUBLIC ${FLAGS})
target_compile_options(${PROJECT_NAME}_test PUBLIC ${FLAGS})

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/glfw)
add_dependencies(${PROJECT_NAME} glfw)
add_dependencies(${PROJECT_NAME}_test glfw)

file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/nestest.nes 
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)

file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/nestest.log
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
