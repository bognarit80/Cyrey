#include "raylib-cpp.hpp"
#include "CyreyApp.hpp"
//#define PLATFORM_WEB

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

//----------------------------------------------------------------------------------
// Main Entry Point
//----------------------------------------------------------------------------------
int main()
{
    Cyrey::CyreyApp theApp{};
    //Cyrey::gApp = &theApp;
    theApp.Init();
    theApp.RunGame();

    return 0;
}