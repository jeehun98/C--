C/C++ 코딩 테스트 실험환경 문서
1) 프로젝트 개요

목적: 여러 문제를 독립 실행 타겟으로 관리하면서, 공통 유틸을 공유 라이브러리로 재사용

빌드 시스템: CMake (MSVC, Ninja, VS Code/Visual Studio 호환)

장점:

한 번의 CMake 구성 후 모든 문제를 일괄 빌드

공통 코드(common/)를 한 곳에서 유지, 중복 제거

새 문제 추가가 폴더 추가 + 한 줄 등록으로 끝

2) 디렉터리 레이아웃
project_root/
├─ CMakeLists.txt                # 루트(총괄) CMake
├─ common/                       # 공통 유틸 라이브러리
│  ├─ CMakeLists.txt
│  ├─ include/common/utils.h
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


공통 타이머(common/include/common/timer.h)를 추가했다면 같은 규칙으로 공용 헤더에 둡니다.

3) 루트 CMakeLists.txt의 역할

전체 빌드의 총괄 관리자: 공통 컴파일 옵션/표준, 하위 디렉터리 등록, 타겟 간 의존 순서 관리

cmake_minimum_required(VERSION 3.24)
project(CPP_MultiProblems LANGUAGES CXX)

# C++ 표준 및 경고
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
  add_compile_options(/W4 /utf-8)   # UTF-8 권장
else()
  add_compile_options(-Wall -Wextra)
endif()

add_subdirectory(common)            # 공통 라이브러리
add_subdirectory(problems/quant)    # 실행 타겟 1
add_subdirectory(problems/conv2d)   # 실행 타겟 2
add_subdirectory(problems/memcpy_opt)


핵심 포인트

add_subdirectory(...) 로 하위 프로젝트를 하나의 빌드 그래프로 묶음

한 번의 구성으로 전체 타겟 빌드 가능

공통 옵션/문자셋/최적화 등의 일괄 관리가 쉬움

4) common/ (공통 유틸 라이브러리)

역할: 여러 문제에서 재사용하는 코드(예: utils.h, timer.h 등)를 정적 라이브러리로 제공

CMake

# common/CMakeLists.txt
add_library(common STATIC
  src/utils.cpp
  # src/timer.cpp  (필요 시)
)
target_include_directories(common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)


사용 방법(문제 타겟에서):

#include "common/utils.h"

target_link_libraries(타겟 PRIVATE common)

5) 문제 폴더(서브프로젝트) CMake 패턴

각 문제는 독립 실행 파일을 만든다.

# problems/<name>/CMakeLists.txt
add_executable(<name>
  src/main.cpp
  src/<name>.cpp
)
target_include_directories(<name> PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(<name> PRIVATE common)  # 공통 유틸 사용


예: problems/quant/CMakeLists.txt에서 타겟 이름은 quant.
빌드 후 실행 파일 위치(멀티 구성 제너레이터 기준):
build/problems/quant/Release/quant.exe

6) 빌드 방법
(권장) Ninja Multi-Config + MSVC
# 최초 1회: 빌드 디렉터리 생성 및 구성
mkdir build; cd build
cmake -G "Ninja Multi-Config" -S .. -B .

# 빌드 (Release 권장)
cmake --build . --config Release

# 특정 타겟만 빌드
cmake --build . --config Release --target quant

Visual Studio 2022 제너레이터
mkdir build; cd build
cmake -G "Visual Studio 17 2022" -A x64 -S .. -B .
cmake --build . --config Release

VS Code

확장: C/C++, CMake Tools

좌하단 CMake: [Debug] → Release로 변경 → Build/Run

개별 타겟 선택해 실행 가능

7) 실행 위치

Ninja/VS 공통(멀티 구성):

build/problems/<problem_name>/Release/<problem_name>.exe


예:

build/problems/quant/Release/quant.exe
build/problems/conv2d/Release/conv2d.exe
build/problems/memcpy_opt/Release/memcpy_opt.exe

8) 새 문제 추가 체크리스트

디렉터리 생성

problems/new_task/
  ├─ CMakeLists.txt
  ├─ include/new_task.h
  └─ src/{main.cpp, new_task.cpp}


문제용 CMakeLists.txt 작성 (위 패턴 복붙 후 타겟명/파일명만 변경)

루트 CMakeLists.txt 에 한 줄 추가

add_subdirectory(problems/new_task)


빌드

cmake --build . --config Release --target new_task

9) 공통 타이머 유틸 사용(예)
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
// ... 작업 ...
std::cout << "elapsed(ms): " << tm.toc_ms() << "\n";

10) 문자셋/인코딩(경고 C4819) 가이드

원인: 소스가 CP949로 저장, MSVC가 UTF-8로 해석 못함

해결(루트 CMake에 추가):

if (MSVC)
  add_compile_options(/utf-8)
endif()


또는 파일을 UTF-8 (BOM 없이) 로 저장

팀/프로젝트 레벨 권장: 루트에 .editorconfig

root = true
[*.{h,hpp,c,cpp}]
charset = utf-8
end_of_line = crlf
insert_final_newline = true

11) 빌드 타입과 최적화

Debug: 디버깅 심볼, 최적화 꺼짐

Release: 최적화 켜짐(-O2//O2), 실측 성능은 이 모드에서 측정

명령 예:

cmake --build . --config Debug
cmake --build . --config Release

12) 트러블슈팅

❌ Cannot find source file 'src/utils.cpp'
→ common/src/utils.cpp 파일 존재 여부/경로 확인. 폴더를 잘못 만들거나 타이핑 오타가 흔함.

❌ MSBuild.exe 관련 에러, VCTargetsPath 없음
→ Visual Studio Build Tools 2022 설치 + 제너레이터를 "Visual Studio 17 2022" 또는 "Ninja Multi-Config"로 지정.

❌ 한글/특수문자 경로 문제
→ 공백/한글 경로도 동작하지만, 터미널에서는 경로를 따옴표로 감싸는 습관 권장:

cd "C:\Users\owner\Desktop\C--\project_root\build"

13) 이 구조를 쓰는 이유(요약)

루트 CMake가 전체 타겟을 통합 관리 → 한 번에 구성/빌드/테스트

common 정적 라이브러리로 공통 코드 재사용 → 중복 제거, 유지보수 용이

문제 폴더는 독립 실행이면서 의존/포함 경로가 자동 관리

새로운 문제/모듈을 쉽게 확장할 수 있는 표준적인 CMake 패턴