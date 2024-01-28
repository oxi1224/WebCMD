cmake_minimum_required(VERSION 3.10)
set (CMAKE_CXX_STANDARD 17)

project(WebCMD)
add_executable(WebCMD main.cpp "http/http.hpp")