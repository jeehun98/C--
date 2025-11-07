🧭 C/C++ 코딩 테스트 실험환경 문서
1) 프로젝트 개요

목적
여러 문제를 독립 실행 타겟으로 관리하면서,
공통 유틸을 공유 라이브러리(common) 로 재사용할 수 있도록 구성합니다.

빌드 시스템
CMake (MSVC, Ninja, VS Code/Visual Studio 호환)

장점

한 번의 CMake 구성 후 모든 문제를 일괄 빌드 가능

공통 코드(common/)를 한 곳에서 유지, 중복 제거

각 문제를 단독으로 빌드/실행할 수도 있음

새 문제 추가 시 폴더 추가 + 한 줄 등록으로 확장

2) 디렉터리 레이아웃
project_root/
├─ CMakeLists.txt                # 루트(총괄) CMake
├─ common/                       # 공통 유틸 라이브러리
│  ├─ CMakeLists.txt
│  ├─ include/common/utils.h
│  ├─ include/common/timer.h
│  └─ src/utils.cpp
└─ problems/                     # 문제 모음(각각 독립 실행파일)
   ├─ quant/
   │  ├─ CMakeLists.txt
   │  ├─ include/quant.h
   │  └─ src/{main.cpp, quant.cpp}
   ├─ conv2d/
   │  ├─ CMakeLists.txt
   │  ├─ include/conv.h
   │  └─ src/{main.cpp, conv.cpp}
   └─ memcpy_opt/
      ├─ CMakeLists.txt
      ├─ include/memcpy_opt.h
      └─ src/{main.cpp, memcpy_opt.cpp}

3) 루트 CMakeLists.txt의 역할

전체 빌드의 총괄 관리자로서,
공통 컴파일 옵션, 하위 디렉터리 등록, 타깃 의존 순서를 관리합니다.

cmake_minimum_required(VERSION 3.24)
project(CPP_MultiProblems LANGUAGES CXX)

# C++ 표준 및 경고
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /utf-8 /MP)
else()
  add_compile_options(-Wall -Wextra)
endif()

# 하위 디렉터리 등록
add_subdirectory(common)
add_subdirectory(problems/quant)
add_subdirectory(problems/conv2d)
add_subdirectory(problems/memcpy_opt)


핵심 포인트

add_subdirectory()로 하위 프로젝트를 하나의 빌드 그래프로 묶음

한 번의 구성으로 전체 타겟 일괄 빌드 가능

공통 옵션/문자셋/최적화 옵션을 루트에서 일괄 관리

4) common/ (공통 유틸 라이브러리)

역할
여러 문제에서 재사용하는 코드(utils.h, timer.h 등)를
정적 라이브러리(common)로 제공합니다.

# common/CMakeLists.txt
add_library(common STATIC
  src/utils.cpp
)
target_include_directories(common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 헤더 온리용 인터페이스 타깃(선택)
add_library(common_headers INTERFACE)
target_include_directories(common_headers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)


문제에서 사용

#include "common/utils.h"
#include "common/timer.h"

target_link_libraries(quant PRIVATE common)

5) 문제 폴더(서브프로젝트) CMake 패턴

루트 빌드 & 독립 빌드를 모두 지원하는 통합 패턴

cmake_minimum_required(VERSION 3.24)

# 1️⃣ 톱레벨(독립실행) 감지 시 프로젝트/옵션 자동 설정
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR OR PROJECT_IS_TOP_LEVEL)
  project(<name> LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  if (MSVC)
    add_compile_options(/W4 /utf-8 /MP)
  else()
    add_compile_options(-Wall -Wextra)
  endif()

  # 루트가 없을 경우 common 직접 등록
  get_filename_component(PROJ_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
  if (NOT TARGET common AND EXISTS "${PROJ_ROOT}/common/CMakeLists.txt")
    add_subdirectory("${PROJ_ROOT}/common" "${CMAKE_BINARY_DIR}/_common_build")
  endif()
endif()

# 2️⃣ 타깃 정의
add_executable(<name>
  src/main.cpp
  src/<name>.cpp
)
target_include_directories(<name> PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 3️⃣ common이 있으면 링크, 없으면 패스
if (TARGET common)
  target_link_libraries(<name> PRIVATE common)
elseif (TARGET common_headers)
  target_link_libraries(<name> PRIVATE common_headers)
endif()


예:
problems/quant/CMakeLists.txt에서 <name>을 quant로 변경.

이 구조 덕분에:

루트에서 전체 빌드 시 → 공통 common 사용

개별 문제 폴더에서 직접 빌드 시 → 스스로 common을 등록해 빌드 가능

6) 빌드 방법
(1) 루트 전체 빌드 (권장)
mkdir build && cd build
cmake -G "Ninja Multi-Config" -S .. -B .
cmake --build . --config Release

(2) 특정 타깃만 빌드
cmake --build . --config Release --target quant

(3) 개별 문제 독립 빌드
cd problems/quant
mkdir build && cd build
cmake -G "Ninja Multi-Config" -S .. -B .
cmake --build . --config Release

(4) Visual Studio 2022
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B build
cmake --build build --config Release

(5) VS Code

확장: C/C++, CMake Tools

좌하단 CMake: [Debug] → Release 변경 → Build/Run

7) 실행 파일 위치

멀티 구성 제너레이터 기준:

build/problems/<problem_name>/Release/<problem_name>.exe


예시:

build/problems/quant/Release/quant.exe
build/problems/conv2d/Release/conv2d.exe
build/problems/memcpy_opt/Release/memcpy_opt.exe

8) 새 문제 추가 체크리스트

1️⃣ 새 폴더 생성

problems/new_task/
  ├─ CMakeLists.txt
  ├─ include/new_task.h
  └─ src/{main.cpp, new_task.cpp}


2️⃣ CMakeLists 작성 (<name>만 변경)
3️⃣ 루트 CMake에 등록

add_subdirectory(problems/new_task)


4️⃣ 빌드

cmake --build . --config Release --target new_task

9) 공통 타이머 유틸 사용 예시
// common/include/common/timer.h
#pragma once
#include <chrono>
struct Timer {
  std::chrono::high_resolution_clock::time_point t0;
  void tic() { t0 = std::chrono::high_resolution_clock::now(); }
  double toc_ms() const {
    auto t1 = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(t1 - t0).count();
  }
};


문제 코드에서:

#include "common/timer.h"
Timer tm; tm.tic();
// ...
std::cout << "elapsed(ms): " << tm.toc_ms() << "\n";

10) 문자셋/인코딩(C4819) 가이드

원인: 소스가 CP949로 저장되어 있고, MSVC가 UTF-8로 인식하지 못함
해결:

if (MSVC)
  add_compile_options(/utf-8)
endif()


또는 파일을 UTF-8 (BOM 없이) 로 저장.
팀 레벨 설정(루트 .editorconfig):

root = true
[*.{h,hpp,c,cpp}]
charset = utf-8
end_of_line = crlf
insert_final_newline = true

11) 빌드 타입과 최적화
타입	설명
Debug	디버깅 심볼 포함, 최적화 비활성
Release	최적화 활성화(/O2, -O2), 성능 측정 시 사용

명령 예:

cmake --build . --config Debug
cmake --build . --config Release

12) 트러블슈팅
문제	원인/해결
Cannot find source file 'src/utils.cpp'	common/src/utils.cpp 존재 확인 (경로 오타 자주 발생)
MSBuild.exe 관련 에러	Visual Studio Build Tools 2022 설치, 제너레이터 "Visual Studio 17 2022" 또는 "Ninja Multi-Config" 지정
한글/공백 경로	터미널에서 경로를 따옴표로 감싸기: cd "C:\Users\owner\Desktop\C--\project_root\build"
13) 구조 설계 요약

루트 CMake는 전체 타깃을 통합 관리 → 한 번에 구성/빌드/테스트 가능

각 문제는 공통 코드(common) 에 선택적으로 의존

문제 폴더는 독립 실행도 가능, 전체 프로젝트 빌드에도 포함됨

확장성, 유지보수성, 재사용성을 모두 갖춘 표준형 CMake 구조