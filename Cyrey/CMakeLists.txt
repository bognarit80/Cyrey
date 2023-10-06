cmake_minimum_required(VERSION 3.14)
project("CyreyApp")

# raylib
find_package(raylib QUIET)
if (NOT raylib_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG f7c3035b8c5f7b86165ba24dfa868a4527e48a2a
    )
    FetchContent_MakeAvailable(raylib)
endif()

# raygui
find_package(raygui QUIET)
if (NOT raygui_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raygui
        GIT_REPOSITORY https://github.com/raysan5/raygui.git
        GIT_TAG 4.0
    )
    FetchContent_MakeAvailable(raygui)
endif()

# nlohmann_json
# find_package(json QUIET)
# if (NOT json_FOUND)
#     include(FetchContent)
#     FetchContent_Declare(
#         json
#         GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
#         GIT_TAG v3.11.2
#     )
#     FetchContent_MakeAvailable(json)
# endif()

file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${CPP_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include" "${raygui_SOURCE_DIR}/src" "${raygui_SOURCE_DIR}/styles/cyber")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

# Web Configurations
if (EMSCRIPTEN)
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    # Required linker flags for using Raylib with Emscripten
    target_link_options(${PROJECT_NAME} PRIVATE -sUSE_GLFW=3 -sWASM=1 -sFULL_ES3=1 -sMIN_WEBGL_VERSION=2 -sTOTAL_MEMORY=128mb --shell-file minshell.html --preload-file "../../../resources")
endif()
