add_executable(ub-frontend
    main.cpp
)

### Bibliotekos
include(FetchContent)

## UI
FetchContent_Declare(ftxui
    GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui.git
    GIT_TAG v6.1.9 
)
FetchContent_MakeAvailable(ftxui)

## HTTP requestai
find_package(cpr REQUIRED)

### Biblioteku linkinimas
target_link_libraries(ub-frontend
    PRIVATE
    ftxui::screen
    ftxui::dom
    ftxui::component
    cpr::cpr
)