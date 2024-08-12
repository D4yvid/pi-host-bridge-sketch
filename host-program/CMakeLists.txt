cmake_minimum_required(VERSION 3.27)

project(host-client C)

add_executable(client
    src/message/parser.c
    src/serial/serial.c
    src/main.c)

target_link_libraries(client
    ncurses)