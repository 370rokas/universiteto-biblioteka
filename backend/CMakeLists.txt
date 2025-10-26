add_executable(ub-backend
    main.cpp
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

### Biblioteku linkinimas
target_link_libraries(ub-frontend
    PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
    ZLIB::ZLIB
    Crow::Crow
    pqxx
)