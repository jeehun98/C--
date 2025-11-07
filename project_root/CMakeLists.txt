cmake_minimum_required(VERSION 3.24)
project(CPP_MultiProblems LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra)
endif()

# ===== NVTX 옵션 (CPU 코드에서 헤더만 쓰는 용도) =====
option(USE_NVTX "Enable NVTX instrumentation (requires nvtx3/nvToolsExt.h)" ON)

# NVTX 헤더 탐색 (CUDA 설치경로/표준경로 스캔)
if (USE_NVTX)
  # 힌트 경로들
  set(_NVTX_HINTS
    "$ENV{CUDA_PATH}/include"
    "$ENV{CUDA_PATH_V12_6}/include"
    "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6/include"
    "/usr/local/cuda/include"
    "/opt/cuda/include"
  )

  find_path(NVTX_INCLUDE_DIR
    NAMES nvtx3/nvToolsExt.h
    HINTS ${_NVTX_HINTS}
  )

  if (NVTX_INCLUDE_DIR)
    message(STATUS "[NVTX] found headers at: ${NVTX_INCLUDE_DIR}")
    add_library(nvtx_headers INTERFACE)
    target_include_directories(nvtx_headers INTERFACE "${NVTX_INCLUDE_DIR}")
    target_compile_definitions(nvtx_headers INTERFACE USE_NVTX)  # 코드에서 #ifdef USE_NVTX
  else()
    message(WARNING "[NVTX] headers NOT found. Building without NVTX. (Set USE_NVTX=OFF or install CUDA/NVTX headers)")
    add_library(nvtx_headers INTERFACE)  # 빈 타겟 (문제없이 링크되도록)
  endif()
else()
  add_library(nvtx_headers INTERFACE)    # 옵션 OFF일 때도 빈 타겟 준비
endif()

# ===== 기존 공용/문제 디렉토리 =====
add_subdirectory(common)

add_subdirectory(problems/quant)
add_subdirectory(problems/conv2d)
add_subdirectory(problems/memcpy_opt)

# ===== 새로 추가: NVTX 포함 행렬 연산 문제 =====
add_subdirectory(problems/matmul_nvtx)
