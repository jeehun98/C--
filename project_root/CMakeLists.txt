cmake_minimum_required(VERSION 3.24)
project(CPPPlayground LANGUAGES CXX)

# C++ 표준 지정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 소스 파일
file(GLOB SRC_FILES src/*.cpp)

# 실행 파일 빌드
add_executable(run ${SRC_FILES})

# 헤더 경로 추가
target_include_directories(run PRIVATE include)

# 경고 옵션 (디버깅용)
if (MSVC)
    target_compile_options(run PRIVATE /W4)
else()
    target_compile_options(run PRIVATE -Wall -Wextra -O2)
endif()
