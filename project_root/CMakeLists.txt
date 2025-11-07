cmake_minimum_required(VERSION 3.24)
project(CPP_MultiProblems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /utf-8)
else()
  add_compile_options(-Wall -Wextra)
endif()

add_subdirectory(common)

add_subdirectory(problems/quant)
add_subdirectory(problems/conv2d)
add_subdirectory(problems/memcpy_opt)