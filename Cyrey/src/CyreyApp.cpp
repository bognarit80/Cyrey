#include "CyreyApp.hpp"
#include "MainMenu.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wc++11-narrowing" // fix web build
#pragma GCC diagnostic ignored "-Wnarrowing" // fix clang build
#include "style_cyber.h"
#pragma GCC diagnostic pop
#include <thread>

#include "TutorialBoard.hpp"
#include "UserMenu.hpp"

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
	this->mOldWindowSize = ::Vector2 { static_cast<float>(this->mWidth), static_cast<float>(this->mHeight) };
	this->mHasWindow = false;
	this->mMTInstance = std::mt19937 {}; // init the object first, we will reseed for sure

	this->mResMgr = std::make_unique<ResourceManager>();
	this->InitWindow();
	this->mGameConfig = Cyrey::cDefaultGameConfig;
	this->mBoard = std::make_unique<Board>(this->mGameConfig.mBoardWidth, this->mGameConfig.mBoardHeight);
	this->mBoard->mApp = this;
	this->mMainMenu = std::make_unique<MainMenu>(*this);
	this->mCurrentUser = std::make_unique<User>(CyreyApp::ParseUserFile(CyreyApp::cUserFileName));
	this->mSettings = std::make_unique<SettingsMenu>(*this);
	this->mReplaysMenu = std::make_unique<ReplaysMenu>(*this);
	this->mUserMenu = std::make_unique<UserMenu>(*this);
}

void Cyrey::CyreyApp::InitWindow()
{
	if (this->mHasWindow)
	{
		::CloseWindow();
		this->mResMgr->UnloadResources();

		// We always get here through settings, so it's guaranteed that they are available.
		this->mSettings->mIsFullscreen ?
			::SetConfigFlags(::ConfigFlags::FLAG_FULLSCREEN_MODE) :
			::ClearWindowState(::ConfigFlags::FLAG_FULLSCREEN_MODE);

		this->mSettings->mIsVSync ?
			::SetConfigFlags(::ConfigFlags::FLAG_VSYNC_HINT) :
			::ClearWindowState(::ConfigFlags::FLAG_VSYNC_HINT);
	}
	::SetConfigFlags(ConfigFlags::FLAG_WINDOW_RESIZABLE | ConfigFlags::FLAG_WINDOW_ALWAYS_RUN);

	::InitWindow(this->mWidth, this->mHeight, CyreyApp::cTitle);
	this->mRefreshRate = ::GetMonitorRefreshRate(::GetCurrentMonitor());
	::SetTargetFPS(this->mRefreshRate);
	this->mHasWindow = true;

	// Load all textures and sounds
	::GuiLoadStyleCyber();
	::InitAudioDevice();
#if !defined(__EMSCRIPTEN__) || defined(__EMSCRIPTEN_PTHREADS__)
	std::thread([this] { this->mResMgr->LoadResources(); }).detach();
#else
    this->mResMgr->LoadResources();
#endif // !defined(__EMSCRIPTEN__) || defined(__EMSCRIPTEN_PTHREADS__)
}

void Cyrey::CyreyApp::GameLoop()
{
	this->Update();
	this->Draw();
	::DrawFPS(10, 10);
#ifndef NDEBUG
    ::DrawText(::TextFormat("%d", this->mUpdateCnt), 10, 100, 16,
        this->mDarkMode ? ::WHITE : ::BLACK);
#endif // NDEBUG
}

void Cyrey::CyreyApp::Update()
{
	this->mWidth = ::GetScreenWidth();
	this->mHeight = ::GetScreenHeight();

	this->mState = this->mChangeToState;

	this->mResMgr->SetVolume(this->mSettings->mSoundVolume, this->mSettings->mMusicVolume);
	// func performs checks if sounds are loaded

	switch (this->mState)
	{
	case CyreyAppState::Loading:
		if (this->mResMgr->HasFinishedLoading())
		{
			if (this->mPrevState == CyreyAppState::Loading)
				this->ChangeToState(CyreyAppState::MainMenu);
			else
				// we can really only get to loading again by reopening window, i.e. from settings
				this->ChangeToState(CyreyAppState::SettingsMenu);
		}
		break;

	case CyreyAppState::MainMenu:
		this->mMainMenu->Update();
		if (this->mMainMenu->mIsPlayBtnPressed)
		{
			if (!this->mCurrentUser->mFinishedTutorial)
				this->mBoard = std::make_unique<TutorialBoard>(this->mGameConfig.mBoardWidth, this->mGameConfig.mBoardHeight);
			else
			{
				this->mGameConfig = Cyrey::cDefaultGameConfig; // TODO: Attempt to fetch game config here?
				this->mBoard = std::make_unique<Board>(this->mGameConfig.mBoardWidth, this->mGameConfig.mBoardHeight);
			}
			this->mBoard->mApp = this;

			this->ChangeToState(CyreyAppState::InGame);
			this->mBoard->NewGame();
		}
		if (this->mMainMenu->mIsUserPressed)
		{
			this->mUserMenu->mIsOpen = true;
			this->mMainMenu->mIsUserPressed = false;
		}
		break;

	case CyreyAppState::InGame:
		this->mBoard->Update();
		break;

	case CyreyAppState::SettingsMenu:
		this->mSettings->Update();
		break;

	case CyreyAppState::ReplaysMenu:
		{
			this->mReplaysMenu->Update();
			if (this->mReplaysMenu->mPlayReplay && this->mReplaysMenu->mSelectedReplay->mConfigVersion == this->
				mGameConfig.mVersion)
			{
				this->ChangeToState(CyreyAppState::InGame);
				this->mBoard->PlayReplay(*this->mReplaysMenu->mSelectedReplay); // we will always have a value here
				this->mBoard->mHasSavedReplay = true; // prevent saving the replay again
				this->mReplaysMenu->mActive = -1; // otherwise the same replay will play every update
				this->mReplaysMenu->mPlayReplay = false;
				this->mReplaysMenu->mSelectedReplay = std::nullopt;
			}
			break;
		}

	default:
		break;
	}
	if (!this->mBoard->mIsPaused)
		::UpdateMusicStream(this->mResMgr->mMusics[this->mCurrentMusic]);
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
			::GuiLabel(Rectangle {
				           static_cast<float>(this->mWidth) / 2,
				           static_cast<float>(this->mHeight) / 2,
				           300, 300
			           }, CyreyApp::cLoading);
			break;

		case CyreyAppState::MainMenu:
			if (this->mUserMenu->mIsOpen)
				this->mUserMenu->Draw();
			else
				this->mMainMenu->Draw();
			break;

		case CyreyAppState::InGame:
			this->mBoard->Draw();
			break;

		case CyreyAppState::SettingsMenu:
			this->mSettings->Draw();
			break;

		case CyreyAppState::ReplaysMenu:
			this->mReplaysMenu->Draw();
			break;

		default:
			break;
		}
	}
	::EndDrawing();
}

int Cyrey::CyreyApp::DrawDialog(const char* title, const char* message, const char* buttons) const
{
	int fontSize = this->mHeight / 18;
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
	auto [x, y] = ::MeasureTextEx(
		::GuiGetFont(),
		message,
		static_cast<float>(fontSize),
		static_cast<float>(::GuiGetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SPACING)));
	float dialogWidth = x * 1.1f;
	float dialogHeight = y + (static_cast<float>(fontSize) * 2.5f);
	auto appWidth = static_cast<float>(this->mWidth);
	auto appHeight = static_cast<float>(this->mHeight);

	Rectangle dialogPos = {
		(appWidth / 2) - (dialogWidth / 2),
		(appHeight / 2) - (dialogHeight / 2),
		dialogWidth,
		dialogHeight
	};
	return ::GuiMessageBox(dialogPos, title, message, buttons);
}

float Cyrey::CyreyApp::GetDeltaTime() const
{
#ifndef NDEBUG
    return ::GetFrameTime();
#else
	return 1.0f / static_cast<float>(this->mRefreshRate); //fixed frametime for debugging
#endif
}

void Cyrey::CyreyApp::ChangeToState(CyreyAppState state)
{
	this->mChangeToState = state;
	if (this->mState != CyreyAppState::Loading && state != CyreyAppState::Loading)
		// prevent switching back to loading state
		this->mPrevState = this->mState;

	switch (state)
	{
	case Cyrey::CyreyAppState::Loading:
		break;
	case Cyrey::CyreyAppState::MainMenu:
		if (this->mPrevState != CyreyAppState::SettingsMenu && this->mPrevState != CyreyAppState::ReplaysMenu)
			this->PlayMusic(ResMusicID::MainMenuTheme);
		break;
	case Cyrey::CyreyAppState::InGame:
		this->PlayMusic(ResMusicID::Blitz1min, false);
		break;
	case Cyrey::CyreyAppState::SettingsMenu:
		if (this->mPrevState != CyreyAppState::MainMenu)
			this->PlayMusic(ResMusicID::MainMenuTheme);
		break;
	case CyreyAppState::ReplaysMenu:
		this->mReplaysMenu->RefreshReplayList();
		break;
	default:
		break;
	}
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
		this->mOldWindowSize = ::Vector2 { static_cast<float>(this->mWidth), static_cast<float>(this->mHeight) };
		int currentMonitor = ::GetCurrentMonitor();
		::SetWindowSize(::GetMonitorWidth(currentMonitor), ::GetMonitorHeight(currentMonitor));
		::ToggleFullscreen();
	}
}

Cyrey::User Cyrey::CyreyApp::ParseUserFile(const char* path)
{
	char* txt = ::LoadFileText(path);
	if (!txt)
		return User {};

	return nlohmann::json::parse(txt);
}

void Cyrey::CyreyApp::SaveCurrentUserData() const
{
	this->mCurrentUser->mName.erase(this->mCurrentUser->mName.find_last_not_of('\0') + 1);
	nlohmann::json json = *this->mCurrentUser;
	std::string str = json.dump();
	::SaveFileData(CyreyApp::cUserFileName, str.data(), static_cast<int>(str.length()));
}

void Cyrey::CyreyApp::PlayMusic(ResMusicID id, bool reset)
{
	if (reset)
		::StopMusicStream(this->mResMgr->mMusics[id]);
	::PlayMusicStream(this->mResMgr->mMusics[id]);
	this->mCurrentMusic = id;
}
void Cyrey::CyreyApp::PlaySound(ResSoundID id)
{
	::PlaySound(this->mResMgr->mSounds[id]);
}

void Cyrey::CyreyApp::SeekMusic(ResMusicID id, float toSeconds)
{
	::SeekMusicStream(this->mResMgr->mMusics[id], toSeconds);
}

void Cyrey::CyreyApp::SetSoundPitch(ResSoundID id, float pitch)
{
	::SetSoundPitch(this->mResMgr->mSounds[id], pitch);
}

unsigned int Cyrey::CyreyApp::SeedRNG()
{
	unsigned int seed = std::random_device()();
	this->SeedRNG(seed);
	return seed;
}

void Cyrey::CyreyApp::SeedRNG(unsigned int seed)
{
	this->mMTInstance.seed(seed);
}

uint32_t Cyrey::CyreyApp::GetRandomNumber(const uint32_t min, const uint32_t max)
{
	// return std::uniform_int_distribution{ min, max }(this->mMTInstance);
	// just simplify and reimplement the gcc's implementation because it's not portable :)

	const uint32_t range = (max - min) + 1;
	uint64_t product = static_cast<uint64_t>(this->mMTInstance()) * static_cast<uint64_t>(range);
	if (auto low = static_cast<uint32_t>(product); low < range)
	{
		const uint32_t threshold = -range % range;
		while (low < threshold)
		{
			product = static_cast<uint64_t>(this->mMTInstance()) * static_cast<uint64_t>(range);
			low = static_cast<uint32_t>(product);
		}
	}
	return (product >> 32) + min;
}
