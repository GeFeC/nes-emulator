cmake_minimum_required(VERSION 3.15)

project(nes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_NAME} 
  src/main.cpp
)

add_executable(${PROJECT_NAME}_test
  test/test.cpp
)

file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/nestest.nes 
  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build
)

file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}/test/nestest.log
  DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/build
)
