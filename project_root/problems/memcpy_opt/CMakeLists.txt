add_executable(memcpy_opt
  src/main.cpp
  src/memcpy_opt.cpp
)

target_include_directories(memcpy_opt PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(memcpy_opt PRIVATE common)
