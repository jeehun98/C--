cmake_minimum_required(VERSION 3.24)

if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR OR PROJECT_IS_TOP_LEVEL)
  project(memcpy_opt LANGUAGES CXX)
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

add_executable(memcpy_opt
  src/main.cpp
  src/memcpy_opt.cpp
)

target_include_directories(memcpy_opt PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if (TARGET common)
  target_link_libraries(memcpy_opt PRIVATE common)
elseif (TARGET common_headers)
  target_link_libraries(memcpy_opt PRIVATE common_headers)
endif()

if (COMMAND tune_target)
  tune_target(memcpy_opt)
endif()
# set_target_properties(memcpy_opt PROPERTIES UNITY_BUILD ON)
