cmake_minimum_required(VERSION 3.13)
project(Homework03)

set(CMAKE_CXX_STANDARD 14)

add_executable(Homework03 client.c comm.h server.c sem_service.c file_handler.c)