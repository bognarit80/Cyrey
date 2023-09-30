#include "CyreyApp.hpp"
#include "MainMenu.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-narrowing" // fix web build
#include "style_cyber.h"
#pragma GCC diagnostic pop

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
    this->mOldWindowSize = ::Vector2{ static_cast<float>(this->mWidth), static_cast<float>(this->mHeight) };
    this->mHasWindow = false;
    this->mFinishedLoading = false;
    this->InitWindow();
    this->mGameConfig = GameConfig::GetLatestConfig();
    this->mBoard = std::make_unique<Board>(this->mGameConfig.mBoardWidth, this->mGameConfig.mBoardHeight);
    this->mBoard->mApp = this;
    this->mBoard->Init();
    this->mMainMenu = std::make_unique<MainMenu>(*this);
    this->mMainMenu->Init();
    this->mCurrentUser = std::make_unique<User>();
    this->mSettings = std::make_unique<SettingsMenu>(*this);
}

void Cyrey::CyreyApp::InitWindow()
{
    if (this->mHasWindow)
    {
        ::CloseWindow();

        // We always get here through settings, so it's guaranteed that they are available.
        this->mSettings->mIsFullscreen ?
            ::SetConfigFlags(::ConfigFlags::FLAG_FULLSCREEN_MODE) :
            ::ClearWindowState(::ConfigFlags::FLAG_FULLSCREEN_MODE);

        this->mSettings->mIsVSync ?
            ::SetConfigFlags(::ConfigFlags::FLAG_VSYNC_HINT) :
            ::ClearWindowState(::ConfigFlags::FLAG_VSYNC_HINT);
    }
    // TODO: Add local user configs here, for fullscreen, vsync and everything else that's added later.
    ::SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE | ConfigFlags::FLAG_WINDOW_ALWAYS_RUN);
        
    ::InitWindow(this->mWidth, this->mHeight, CyreyApp::cTitle);
    this->mRefreshRate = ::GetMonitorRefreshRate(::GetCurrentMonitor());
    ::SetTargetFPS(this->mRefreshRate);
    this->mHasWindow = true;

    // Load all textures and sounds
    ::GuiLoadStyleCyber();
}

void Cyrey::CyreyApp::GameLoop()
{
    this->Update();
    this->Draw();
    ::DrawFPS(10, 10);
#ifdef _DEBUG
    ::DrawText(::TextFormat("%d", this->mUpdateCnt), 10, 100, 16,
        this->mDarkMode ? ::WHITE : ::BLACK);
#endif // _DEBUG

}

void Cyrey::CyreyApp::Update()
{
    this->mWidth = ::GetScreenWidth();
    this->mHeight = ::GetScreenHeight();

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
    ::BeginDrawing();
    {
        ::ClearBackground(this->mDarkMode ? ::BLACK : ::RAYWHITE);
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
    ::EndDrawing();
}

float Cyrey::CyreyApp::GetDeltaTime() const
{
#ifdef _DEBUG
    return 1.0f / this->mRefreshRate; //fixed frametime for debugging
#else
    return ::GetFrameTime();
#endif // _DEBUG
}

void Cyrey::CyreyApp::ChangeToState(CyreyAppState state)
{
    this->mChangeToState = state;
    this->mPrevState = this->mState;
}

void Cyrey::CyreyApp::ToggleFullscreen()
{
    if (::IsWindowFullscreen())
    {
        ::ToggleFullscreen();
        ::SetWindowSize(this->mOldWindowSize.x, this->mOldWindowSize.y);
    }
    else
    {
        this->mOldWindowSize = ::Vector2{ static_cast<float>(this->mWidth), static_cast<float>(this->mHeight) };
        int currentMonitor = ::GetCurrentMonitor();
        ::SetWindowSize(::GetMonitorWidth(currentMonitor), ::GetMonitorHeight(currentMonitor));
        ::ToggleFullscreen();
    }
}

bool Cyrey::CyreyApp::LoadingThread()
{
    this->mFinishedLoading = true;
    return true;
}
