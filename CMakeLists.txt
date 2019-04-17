cmake_minimum_required(VERSION 3.13)
project(lab3)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -pipe -flto")
set(CMAKE_CXX_STANDARD 17)

add_executable(lab3 main.cpp MPMCQueue.hpp SemapQueue.hpp Semaphore.hpp Matrix.hpp Operation.hpp Channel.hpp)