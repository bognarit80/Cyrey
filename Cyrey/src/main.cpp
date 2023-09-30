#include "raylib.h"
#include "CyreyApp.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#endif

static Cyrey::CyreyApp gApp;

//----------------------------------------------------------------------------------
// Main Entry Point
//----------------------------------------------------------------------------------
void web_loop()
{
    gApp.GameLoop();
}

int main(void)
{
    gApp.Init();
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(web_loop, 0, 1);
#else
    while (!::WindowShouldClose() && !gApp.mWantExit)
    {
        gApp.GameLoop();
    }
#endif
    ::CloseWindow();

    return 0;
}