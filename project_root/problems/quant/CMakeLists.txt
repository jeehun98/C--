cmake_minimum_required(VERSION 3.24)

# ── 1) 톱레벨(독립실행) 감지 시, 스스로 프로젝트/옵션 설정
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR OR PROJECT_IS_TOP_LEVEL)
  project(quant LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  if (MSVC)
    add_compile_options(/W4 /utf-8 /MP)
  else()
    add_compile_options(-Wall -Wextra)
  endif()

  # 루트가 없으므로 common을 직접 등록(있을 때만)
  get_filename_component(PROJ_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
  if (NOT TARGET common AND EXISTS "${PROJ_ROOT}/common/CMakeLists.txt")
    add_subdirectory("${PROJ_ROOT}/common" "${CMAKE_BINARY_DIR}/_common_build")
  endif()
endif()

# ── 2) 타깃 정의
add_executable(quant
  src/main.cpp
  src/quant.cpp
)

target_include_directories(quant PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# ── 3) 의존(있으면 링크) — 루트 일괄 빌드 / 독립 빌드 모두 안전
if (TARGET common)
  target_link_libraries(quant PRIVATE common)
elseif (TARGET common_headers)
  target_link_libraries(quant PRIVATE common_headers)
endif()

# ── 4) 선택: 루트에서 제공하는 튜닝 함수가 있으면 적용
if (COMMAND tune_target)
  tune_target(quant)
endif()

# 선택: 소스 많아지면 유니티 빌드 켜기
# set_target_properties(quant PROPERTIES UNITY_BUILD ON)
