cmake_minimum_required (VERSION 3.25)

project(universiteto-biblioteka
  LANGUAGES CXX
  VERSION 1.0.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Output all build files (e.g. executables, libraries) into a single directory
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(ub-backend
    main.cpp

    api/api.cpp

    database/db.cpp
    database/queries.cpp

    utils/config.cpp
)

target_include_directories(ub-backend
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

### Bibliotekos
include(FetchContent)
find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)

## HTTP serveris
set(CROW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(CROW_BUILD_TESTS OFF CACHE BOOL "" FORCE)

FetchContent_Declare(Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.3.0
)
FetchContent_MakeAvailable(Crow)

## PostgreSQL klientas
find_package(libpqxx REQUIRED)
find_package(PostgreSQL REQUIRED)

## Logginimas
find_package(spdlog REQUIRED)
## JSON
find_package(nlohmann_json REQUIRED)

## bCrypt password hashinimas
FetchContent_Declare(bcrypt
    GIT_REPOSITORY https://github.com/hilch/Bcrypt.cpp.git
    GIT_TAG master
)
FetchContent_MakeAvailable(bcrypt)

## JWT sesiju tokenam
include(FetchContent)
fetchcontent_declare(jwt-cpp 
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG v0.7.1
)
set(JWT_BUILD_EXAMPLES OFF CACHE BOOL "disable building examples" FORCE)
fetchcontent_makeavailable(jwt-cpp)

### Biblioteku linkinimas
target_link_libraries(ub-backend
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Crow::Crow
    pqxx pq
    spdlog::spdlog
    jwt-cpp::jwt-cpp
    nlohmann_json::nlohmann_json
    bcrypt
)