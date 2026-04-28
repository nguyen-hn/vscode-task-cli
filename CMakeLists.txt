cmake_minimum_required(VERSION 3.20)
project(vscode-task-runner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Thread REQUIRED)
include (FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz)
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
)
FetchContent_Declare(
  spdlog
  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.x  # Or a specific version like v1.15.0
)

FetchContent_MakeAvailable(json argparse spdlog)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(vscode-task-runner ${SOURCES})

target_link_libraries(vscode-task-runner PRIVATE spdlog::spdlog argparse nlohmann_json::nlohmann_json)
target_include_directories(vscode-task-runner PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_options(vscode-task-runner PRIVATE -g -Wall -Wextra)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

enable_testing()

add_test(NAME RunTests COMMAND vscode-task-runner --task Test --workspace ../tests --file task.json)
