cmake_minimum_required(VERSION 3.31.6)

project(posea)

add_executable(${PROJECT_NAME} main.cpp)

target_sources(${PROJECT_NAME} PRIVATE
  src/lexer.cpp
  src/token.cpp
  src/parser.cpp
  src/node.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)