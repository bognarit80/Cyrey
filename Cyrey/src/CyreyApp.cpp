#include "CyreyApp.hpp"
#include "MainMenu.hpp"

//Init the default values. Call this after constructing the object, before running the game.
void Cyrey::CyreyApp::Init()
{
    this->mWidth = 1280;
    this->mHeight = 720;
    this->mDarkMode = true;
    this->mUpdateCnt = 0;
    this->mState = CyreyAppState::Loading;
    this->mChangeToState = CyreyAppState::Loading;
    this->mPrevState = CyreyAppState::Loading;
    this->mWantExit = false;

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
    this->mMainMenu = std::make_unique<MainMenu>(*this);
    this->mMainMenu->Init();
    this->mCurrentUser = std::make_unique<User>();
    this->mSettings = std::make_unique<SettingsMenu>(*this);
}

void Cyrey::CyreyApp::GameLoop()
{
    this->Update();
    this->Draw();
    this->mWindow->DrawFPS();
#ifdef _DEBUG
    raylib::DrawText(std::to_string(this->mUpdateCnt), 10, 100, 16, 
        this->mDarkMode ? raylib::Color::White() : raylib::Color::Black());
#endif // _DEBUG

}

void Cyrey::CyreyApp::Update()
{
    this->mWidth = this->mWindow->GetWidth();
    this->mHeight = this->mWindow->GetHeight();

    this->mState = this->mChangeToState;

    switch (this->mState)
    {
    case CyreyAppState::Loading:
        if (this->LoadingThread())
            this->ChangeToState(CyreyAppState::MainMenu);
        break;

    case CyreyAppState::MainMenu:
        this->mMainMenu->Update();
        if (this->mMainMenu->mIsPlayBtnPressed)
        {
            this->ChangeToState(CyreyAppState::InGame);
            this->mBoard->Init();
        }
        break;

    case CyreyAppState::InGame:
        this->mBoard->Update();
        break;

    case CyreyAppState::SettingsMenu:
        this->mSettings->Update();
        break;

    default:
        break;
    }
    this->mUpdateCnt++;
}

void Cyrey::CyreyApp::Draw() const
{
    this->mWindow->BeginDrawing();
    {
        this->mWindow->ClearBackground(this->mDarkMode ? raylib::Color::Black() : raylib::Color::RayWhite());
        switch (this->mState)
        {
        case CyreyAppState::Loading:
            break;

        case CyreyAppState::MainMenu:
            this->mMainMenu->Draw();
            break;

        case CyreyAppState::InGame:
            this->mBoard->Draw();
            break;

        case CyreyAppState::SettingsMenu:
            this->mSettings->Draw();
            break;

        default:
            break;
        }
    }
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

void Cyrey::CyreyApp::ChangeToState(CyreyAppState state)
{
    this->mChangeToState = state;
    this->mPrevState = this->mState;
}

bool Cyrey::CyreyApp::LoadingThread()
{
    return true;
}
