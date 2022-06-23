cmake_minimum_required(VERSION 3.22)
project(teditor C)

set(CMAKE_C_STANDARD 99)

add_executable(teditor main.c gapbuf.c gapbuf.h)
