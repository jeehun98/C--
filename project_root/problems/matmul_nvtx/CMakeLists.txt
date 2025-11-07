# problems/matmul_nvtx/CMakeLists.txt
cmake_minimum_required(VERSION 3.24)

# 이 파일이 '최상위'로 호출된 경우에만 project() 선언
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
  project(MatmulNVTX LANGUAGES CXX)

  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)

  if (MSVC)
    add_compile_options(/W4 /utf-8)
  else()
    add_compile_options(-Wall -Wextra)
  endif()

  # 독립 실행 시 NVTX 옵션/헤더 탐색도 여기서 처리
  option(USE_NVTX "Enable NVTX instrumentation" ON)
  if (USE_NVTX)
    set(_NVTX_HINTS
      "$ENV{CUDA_PATH}/include"
      "$ENV{CUDA_PATH_V12_6}/include"
      "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/include"
      "/usr/local/cuda/include"
      "/opt/cuda/include"
    )
    find_path(NVTX_INCLUDE_DIR NAMES nvtx3/nvToolsExt.h HINTS ${_NVTX_HINTS})
    if (NVTX_INCLUDE_DIR)
      message(STATUS "[NVTX] headers: ${NVTX_INCLUDE_DIR}")
      add_library(nvtx_headers INTERFACE)
      target_include_directories(nvtx_headers INTERFACE "${NVTX_INCLUDE_DIR}")
      target_compile_definitions(nvtx_headers INTERFACE USE_NVTX)
    else()
      message(WARNING "[NVTX] not found; building without NVTX")
      add_library(nvtx_headers INTERFACE) # empty
    endif()
  else()
    add_library(nvtx_headers INTERFACE)   # empty
  endif()
endif()

# 상위 프로젝트(루트)에서 이미 nvtx_headers를 만든 경우 재사용
if (NOT TARGET nvtx_headers)
  add_library(nvtx_headers INTERFACE)     # 상위가 없을 때 안전망
endif()

add_executable(matmul_nvtx
  src/matmul_nvtx.cpp
)
target_link_libraries(matmul_nvtx PRIVATE nvtx_headers)

if (MSVC)
  target_compile_options(matmul_nvtx PRIVATE /O2)
else()
  target_compile_options(matmul_nvtx PRIVATE -O2)
endif()
