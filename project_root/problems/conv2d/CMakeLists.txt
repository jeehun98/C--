add_executable(conv2d
  src/main.cpp
  src/conv.cpp
)

target_include_directories(conv2d PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(conv2d PRIVATE common)
