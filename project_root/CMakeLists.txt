cmake_minimum_required(VERSION 3.24)
project(CPP_MultiProblems LANGUAGES CXX)

# 공통 C++ 표준
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 전역 경고 옵션
if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# 공통 라이브러리
add_subdirectory(common)

# 개별 문제(실행 타겟들)
add_subdirectory(problems/quant)
add_subdirectory(problems/conv2d)
add_subdirectory(problems/memcpy_opt)
