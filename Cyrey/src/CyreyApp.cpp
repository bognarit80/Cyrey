#include "CyreyApp.hpp"
#include "raygui.h"

//Init the default values. Call this after constructing the object, before running the game.
void Cyrey::CyreyApp::Init()
{
	this->mWidth = 1280;
	this->mHeight = 720;
    this->mDarkMode = true;

    this->mWindow = std::make_unique<raylib::Window>(
        this->mWidth,
        this->mHeight,
        "Cyrey",
        ConfigFlags::FLAG_WINDOW_RESIZABLE |
        ConfigFlags::FLAG_WINDOW_ALWAYS_RUN);
    int currentMonitor = ::GetCurrentMonitor();
    this->mRefreshRate = ::GetMonitorRefreshRate(currentMonitor);
    this->mWindow->SetTargetFPS(this->mRefreshRate);
    /*this->mWindow->SetPosition((::GetMonitorWidth(currentMonitor) / 2) - (this->mWindow->GetWidth() / 2),
        (::GetMonitorHeight(currentMonitor) / 2) - (this->mWindow->GetHeight() / 2));*/
    this->mBoard = std::make_unique<Board>(8, 8);
    this->mBoard->Init();
    this->mBoard->mApp = this;
}

void Cyrey::CyreyApp::GameLoop()
{
    this->Update();
    this->Draw();
    this->mWindow->DrawFPS();
}

void Cyrey::CyreyApp::Update()
{
    this->mBoard->Update();
}

void Cyrey::CyreyApp::Draw() const
{
    this->mWindow->BeginDrawing();

    this->mWindow->ClearBackground(this->mDarkMode ? raylib::Color::Black() : raylib::Color::RayWhite());

    this->mBoard->Draw();

    this->mWindow->EndDrawing();
}

float Cyrey::CyreyApp::GetDeltaTime() const
{
#ifdef _DEBUG
    return 1.0f / this->mRefreshRate; //fixed frametime for debugging
#else
    return this->mWindow->GetFrameTime();
#endif // _DEBUG
}
