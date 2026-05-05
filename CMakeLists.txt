cmake_minimum_required(VERSION 3.15)

project(auto_broadcast VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Endstone SDK ──────────────────────────────────────────────────────────────
set(ENDSTONE_API_VERSION "0.11" CACHE STRING "Endstone API version")

include(FetchContent)
FetchContent_Declare(
    endstone
    GIT_REPOSITORY https://github.com/EndstoneMC/endstone.git
    GIT_TAG        v${ENDSTONE_API_VERSION}
)
FetchContent_MakeAvailable(endstone)

# ── yaml-cpp (config parsing) ─────────────────────────────────────────────────
FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG        0.8.0
)
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(yaml-cpp)

# ── Plugin target ─────────────────────────────────────────────────────────────
endstone_add_plugin(${PROJECT_NAME}
    src/plugin.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp::yaml-cpp)
