cmake_minimum_required(VERSION 3.14)
project("CyreyApp")

# raylib
set(RAYLIB_VERSION 5.5)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(raylib)
endif ()

# raygui
set(RAYGUI_VERSION 4.0)
find_package(raygui ${RAYGUI_VERSION} QUIET)
if (NOT raygui_FOUND)
    include(FetchContent)
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
            raygui
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raygui/archive/refs/tags/${RAYGUI_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(raygui)
endif ()

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
endif ()

# libcpr
set(CPR_VERSION 1.10.5)
find_package(cpr ${CPR_VERSION} QUIET)
if (NOT cpr_FOUND)
    include(FetchContent)
    FetchContent_Declare(
            cpr
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/libcpr/cpr/archive/refs/tags/${CPR_VERSION}.tar.gz
    )
    FetchContent_MakeAvailable(cpr)
endif ()

add_compile_definitions(CYREY_APIURL="https://localhost:44392/api/")

set(CPP_SOURCES
        src/Board.cpp
        src/Board.hpp
        src/CyreyApp.cpp
        src/CyreyApp.hpp
        src/GameConfig.cpp
        src/GameConfig.hpp
        src/GameStats.hpp
        src/main.cpp
        src/MainMenu.cpp
        src/MainMenu.hpp
        src/MatchSet.cpp
        src/MatchSet.hpp
        src/Piece.cpp
        src/Piece.hpp
        src/PieceDropAnim.cpp
        src/PieceDropAnim.hpp
        src/PieceMatchAnim.cpp
        src/PieceMatchAnim.hpp
        src/raylib_win32.h
        src/Replay.cpp
        src/Replay.hpp
        src/ReplaysMenu.cpp
        src/ReplaysMenu.hpp
        src/ResourceManager.cpp
        src/ResourceManager.hpp
        src/Resources.hpp
        src/SettingsMenu.cpp
        src/SettingsMenu.hpp
        src/SwapAnim.cpp
        src/SwapAnim.hpp
        src/SwapDirection.hpp
        src/TutorialBoard.cpp
        src/TutorialBoard.hpp
        src/User.cpp
        src/User.hpp
        src/UserMenu.cpp
        src/UserMenu.hpp)

if (BUILD_LIBRARY)
    set(CPP_SOURCES
            ${CPP_SOURCES}
            src/VerificationBoard.cpp
            src/VerificationBoard.hpp
            src/MinApp.cpp
            src/MinApp.hpp)
    add_library(${PROJECT_NAME} SHARED ${CPP_SOURCES})
else ()
    add_executable(${PROJECT_NAME} ${CPP_SOURCES})
endif ()
set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20 WIN32_EXECUTABLE
        $<AND:$<PLATFORM_ID:Windows>,$<NOT:$<CONFIG:Debug,RelWithDebInfo>>>)
target_link_libraries(${PROJECT_NAME} PUBLIC raylib nlohmann_json cpr::cpr -static gcc stdc++ winpthread -dynamic)
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
endif ()
