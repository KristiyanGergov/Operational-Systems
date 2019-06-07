cmake_minimum_required(VERSION 3.13)
project(Homework02 C)

set(CMAKE_C_STANDARD 99)

add_executable(Homework02 main.c SegmentType.h ArgumentType.h Constants.h Parameter.h CommandHandler.h FileManager.h Position.h OutputHandler.h BitsManipulation.h)