#include "CyreyApp.hpp"

//Init the default values. Call this after constructing the object, before running the game.
void Cyrey::CyreyApp::Init()
{
	this->mRefreshRate = 60;
	this->mWidth = 800;
	this->mHeight = 650;
    this->mDarkMode = true;

    this->mWindow = std::make_unique<raylib::Window>(
        this->mWidth,
        this->mHeight,
        "Cyrey",
        ConfigFlags::FLAG_WINDOW_RESIZABLE |
        ConfigFlags::FLAG_WINDOW_ALWAYS_RUN |
        ConfigFlags::FLAG_WINDOW_HIGHDPI);
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

void Cyrey::CyreyApp::Draw() 
{
    this->mWindow->BeginDrawing();

    this->mWindow->ClearBackground(this->mDarkMode ? raylib::Color::Black() : raylib::Color::RayWhite());

    this->mBoard->Draw();

    this->mWindow->EndDrawing();
}
