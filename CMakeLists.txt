cmake_minimum_required(VERSION 3.20)
project(SKN-raylib LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
include(FetchContent)
FetchContent_Declare(raylib
    GIT_REPOSITORY https://github.com/raysan5/raylib.git
    GIT_TAG 6.0
    GIT_SHALLOW TRUE)
FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_executable(skn ${SOURCES} "src/SoundDisplayer.h" "src/SoundManager.h" "src/TurnManager.h" "src/Levels.h")
target_include_directories(skn PRIVATE src)
target_link_libraries(skn PRIVATE raylib)

if(MSVC)
    target_compile_options(skn PRIVATE /W3 /utf-8)
endif()

add_custom_target(copy_assets ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/assets $<TARGET_FILE_DIR:skn>/assets)
add_dependencies(skn copy_assets)