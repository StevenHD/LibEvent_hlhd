cmake_minimum_required(VERSION 3.19)
project(libEvent_C C)

set(CMAKE_C_STANDARD 11)

link_directories( /usr/lib/x86_64-linux-gnu/libevent.so )
include_directories( /usr/lib/x86_64-linux-gnu/libevent.a )

add_executable(libEvent_C main.c all.h funcs/event_assign.c funcs/event_priority.c funcs/event_loop.c Tcp/server.c Tcp/client.c Tcp/bufferEventServer.c Tcp/bufferEventClient.c)

target_link_libraries(libEvent_C event)