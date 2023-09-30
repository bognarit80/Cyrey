#ifndef _CYREY_APP_HEADER
#define _CYREY_APP_HEADER

#include "raylib.h"
#include "Board.hpp"
#include "MainMenu.hpp"
#include "User.hpp"
#include "SettingsMenu.hpp"
#include "GameConfig.hpp"
#include <memory>

namespace Cyrey 
{
	class Board;
	class MainMenu;
	class User;
	class SettingsMenu;
	class GameConfig;

	enum class CyreyAppState
	{
		Loading,
		MainMenu,
		InGame,
		SettingsMenu
	};

	class CyreyApp
	{
	public:
		CyreyApp() = default;
		CyreyApp(const CyreyApp&) = delete;
		CyreyApp& operator=(const CyreyApp&) = delete;
		CyreyApp(CyreyApp&&) = delete;
		CyreyApp& operator=(const CyreyApp&&) = delete;

		int mWidth;
		int mHeight;
		int mRefreshRate;
		std::unique_ptr<Board> mBoard;
		bool mDarkMode;
		int mUpdateCnt;
		CyreyAppState mState;
		CyreyAppState mChangeToState; //to prevent drawing a state before updating it at least once
		CyreyAppState mPrevState;
		std::unique_ptr<MainMenu> mMainMenu;
		bool mWantExit;
		std::unique_ptr<User> mCurrentUser;
		std::unique_ptr<SettingsMenu> mSettings;
		::Vector2 mOldWindowSize;
		GameConfig mGameConfig;
		bool mHasWindow;
		bool mFinishedLoading;

		static constexpr char cTitle[] = "Cyrey";

		void Init();
		void InitWindow();
		void GameLoop();
		void Update();
		void Draw() const;
		float GetDeltaTime() const;
		void ChangeToState(CyreyAppState state); //Change to the state at the beginning of the next update
		void ToggleFullscreen();

		bool LoadingThread();
	};
} // namespace Cyrey 

#endif // !_CYREY_APP_HEADER
