add_library(common STATIC
  src/utils.cpp
)
target_include_directories(common PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 선택: 헤더 온리 인터페이스(타이머 같이 헤더만 쓰는 경우)
add_library(common_headers INTERFACE)
target_include_directories(common_headers INTERFACE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)
