add_executable(quant
  src/main.cpp
  src/quant.cpp
)

target_include_directories(quant PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(quant PRIVATE common)
