cmake_minimum_required(VERSION 3.19)
project(dirlucky C)

set(CMAKE_C_STANDARD 11)

add_executable(dirlucky src/main.c src/common/directory_queue.c src/common/directory_queue.h src/posix/threading.h src/windows/threading.h)