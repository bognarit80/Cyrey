#include "CyreyApp.hpp"

//Init the default values. Call this after constructing the object, before running the game.
void Cyrey::CyreyApp::Init()
{
	this->mRefreshRate = 60;
	this->mWidth = 800;
	this->mHeight = 650;
}

void Cyrey::CyreyApp::RunGame()
{
    raylib::Window window(
        this->mWidth, 
        this->mHeight, 
        "Cyrey", 
        ConfigFlags::FLAG_WINDOW_RESIZABLE |
        ConfigFlags::FLAG_WINDOW_ALWAYS_RUN |
        ConfigFlags::FLAG_WINDOW_HIGHDPI
    );

#if defined(PLATFORM_WEB)
    emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
    window.SetTargetFPS(this->mRefreshRate);
    this->mWindow = &window;

    this->mBoard = &Board(8, 8);
    this->mBoard->Init();

    while (!window.ShouldClose())
    {
        this->Update();
        this->Draw();
        window.DrawFPS();
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

    this->mBoard->Draw();

    EndDrawing();
}
