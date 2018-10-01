cmake_minimum_required(VERSION 3.11)
project(BKThread C)

set(CMAKE_C_STANDARD 99)

add_executable(BKThread  Thread.c Thread.h main.c)