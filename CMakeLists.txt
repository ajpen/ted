cmake_minimum_required(VERSION 3.22)
project(ted C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(src/buffer)

add_executable(teditor app/main.c)
target_link_libraries(Buffer)
