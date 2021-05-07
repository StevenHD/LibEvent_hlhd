cmake_minimum_required(VERSION 3.19)
project(libEvent_C C)

set(CMAKE_C_STANDARD 11)

link_directories( /usr/lib/x86_64-linux-gnu/libevent.so )
include_directories( /usr/lib/x86_64-linux-gnu/libevent.a )

add_executable(libEvent_C main.c)

target_link_libraries(libEvent_C event)