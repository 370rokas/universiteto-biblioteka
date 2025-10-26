cmake_minimum_required (VERSION 3.30)

project(universiteto-biblioteka
  LANGUAGES CXX
  VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(frontend)
add_subdirectory(backend)