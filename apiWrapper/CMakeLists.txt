add_library(
    ub-library
    STATIC

    api.cpp
)

## HTTP requestai
find_package(cpr REQUIRED)

target_include_directories(
    ub-library
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(
    ub-library
    PRIVATE
    cpr::cpr
)
