add_executable(ub-backend
    main.cpp

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

## Logginimas
find_package(spdlog REQUIRED)

## JSON
find_package(nlohmann_json REQUIRED)

### Biblioteku linkinimas
target_link_libraries(ub-backend
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Crow::Crow
    pqxx
    spdlog::spdlog
    nlohmann_json::nlohmann_json
)