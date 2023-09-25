#ifndef _CYREY_APP_HEADER
#define _CYREY_APP_HEADER

#include "raylib-cpp.hpp"
#include "Board.hpp"
#include "MainMenu.hpp"
#include "User.hpp"
#include "SettingsMenu.hpp"
#include <memory>

namespace Cyrey 
{
	class Board;
	class MainMenu;
	class User;
	class SettingsMenu;

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
		std::unique_ptr<raylib::Window> mWindow;
		bool mDarkMode;
		int mUpdateCnt;
		CyreyAppState mState;
		CyreyAppState mChangeToState; //to prevent drawing a state before updating it at least once
		CyreyAppState mPrevState;
		std::unique_ptr<MainMenu> mMainMenu;
		bool mWantExit;
		std::unique_ptr<User> mCurrentUser;
		std::unique_ptr<SettingsMenu> mSettings;

		void Init();
		void GameLoop();
		void Update();
		void Draw() const;
		float GetDeltaTime() const;
		void ChangeToState(CyreyAppState state); //Change to the state at the beginning of the next update

		bool LoadingThread();
	};
} // namespace Cyrey 

#endif // !_CYREY_APP_HEADER
