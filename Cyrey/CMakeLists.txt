cmake_minimum_required(VERSION 3.14)
project("CyreyApp")

# raylib
set(RAYLIB_VERSION 5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# raygui
set(RAYGUI_VERSION 4.0)
find_package(raygui ${RAYGUI_VERSION} QUIET)
if (NOT raygui_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            raygui
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raygui/archive/refs/tags/${RAYGUI_VERSION}.tar.gz
    )
    if (NOT raygui_POPULATED)
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raygui)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    endif()
endif()

# nlohmann_json
set(JSON_VERSION 3.11.3)
find_package(json ${JSON_VERSION} QUIET)
if (NOT json_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            json
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/nlohmann/json/releases/download/v${JSON_VERSION}/json.tar.xz
    )
    FetchContent_MakeAvailable(json)
endif()

file(GLOB_RECURSE CPP_SOURCES "src/*.cpp")
add_executable(${PROJECT_NAME} ${CPP_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib nlohmann_json)
target_include_directories(${PROJECT_NAME} PUBLIC "${CMAKE_CURRENT_LIST_DIR}/include" "${raygui_SOURCE_DIR}/src" "${raygui_SOURCE_DIR}/styles/cyber")

# Copy /resources
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${CMAKE_SOURCE_DIR}/resources $<TARGET_FILE_DIR:${PROJECT_NAME}>/resources)

# Web Configurations
if (EMSCRIPTEN)
    # Tell Emscripten to build an example.html file.
    set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".html")

    # Required linker flags for using Raylib with Emscripten
    target_link_options(${PROJECT_NAME} PRIVATE -sUSE_GLFW=3 -sWASM=1 -sFULL_ES3=1 -sMIN_WEBGL_VERSION=2
            -sTOTAL_MEMORY=128mb --shell-file ${CMAKE_SOURCE_DIR}/Cyrey/minshell.html --preload-file "../../../resources")
    # Resources path has to be a relative path so raylib can read the files
endif()
