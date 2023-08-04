#include "CyreyApp.hpp"

//Init the default values. Call this after constructing the object, before running the game.
void Cyrey::CyreyApp::Init()
{
	this->mRefreshRate = 60;
	this->mScreenWidth = 800;
	this->mScreenHeight = 650;
}

void Cyrey::CyreyApp::RunGame()
{
    raylib::Window window(this->mScreenWidth, 
        this->mScreenHeight, 
        "Cyrey", 
        ConfigFlags::FLAG_WINDOW_RESIZABLE |
        ConfigFlags::FLAG_WINDOW_ALWAYS_RUN |
        ConfigFlags::FLAG_WINDOW_HIGHDPI);

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    ::SetTargetFPS(this->mRefreshRate);

    while (!window.ShouldClose())
    {
        this->Update();
        this->Draw();
    }
#endif
}

void Cyrey::CyreyApp::Update()
{

}

void Cyrey::CyreyApp::Draw() 
{
    BeginDrawing();

    ClearBackground(RAYWHITE);

    DrawText("Hello World!", 160, 200, 20, LIGHTGRAY);

    EndDrawing();
}
