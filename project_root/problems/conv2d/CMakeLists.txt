cmake_minimum_required(VERSION 3.24)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR OR PROJECT_IS_TOP_LEVEL)
  project(conv2d LANGUAGES CXX)
  set(CMAKE_CXX_STANDARD 17)
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  if (MSVC)
    add_compile_options(/W4 /utf-8 /MP)
  else()
    add_compile_options(-Wall -Wextra)
  endif()

  get_filename_component(PROJ_ROOT "${CMAKE_CURRENT_LIST_DIR}/../.." ABSOLUTE)
  if (NOT TARGET common AND EXISTS "${PROJ_ROOT}/common/CMakeLists.txt")
    add_subdirectory("${PROJ_ROOT}/common" "${CMAKE_BINARY_DIR}/_common_build")
  endif()
endif()

add_executable(conv2d
  src/main.cpp
  src/conv.cpp
)

target_include_directories(conv2d PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# conv2d는 common이 꼭 필요하지 않다면 링크 생략 가능.
# 필요하면 아래 두 줄 중 하나가 자동으로 적용됨.
if (TARGET common)
  target_link_libraries(conv2d PRIVATE common)
elseif (TARGET common_headers)
  target_link_libraries(conv2d PRIVATE common_headers)
endif()

if (COMMAND tune_target)
  tune_target(conv2d)
endif()
# set_target_properties(conv2d PROPERTIES UNITY_BUILD ON)
