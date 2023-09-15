#ifndef _CYREY_APP_HEADER
#define _CYREY_APP_HEADER

#include "raylib-cpp.hpp"
#include "Board.hpp"
#include "MainMenu.hpp"
#include <memory>

namespace Cyrey 
{
	class Board;
	class MainMenu;

	enum class CyreyAppState
	{
		Loading,
		MainMenu,
		InGame
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
		std::unique_ptr<MainMenu> mMainMenu;

		void Init();
		void GameLoop();
		void Update();
		void Draw() const;
		float GetDeltaTime() const;

		bool LoadingThread();
	};
} // namespace Cyrey 

#endif // !_CYREY_APP_HEADER
