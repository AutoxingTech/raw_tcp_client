cmake_minimum_required(VERSION 3.1)
project(raw_tcp_client)

include_directories(include src)

file(GLOB SRC_FILES
  src/main.cpp
  # src/laser_converter.cpp
)

add_executable(${PROJECT_NAME} ${SRC_FILES})
