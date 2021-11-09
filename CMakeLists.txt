cmake_minimum_required(VERSION 3.20)
project(pageTable)

set(CMAKE_CXX_STANDARD 17)

add_executable(pageTable VirtualMemory.h VirtualMemory.cpp PhysicalMemory.h PhysicalMemory.cpp MemoryConstants.h SimpleTest.cpp main.cpp)
