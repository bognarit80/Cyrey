#ifndef _CYREY_APP_HEADER
#define _CYREY_APP_HEADER

#include "raylib-cpp.hpp"
#include "Board.hpp"
#include <memory>

namespace Cyrey 
{
	class Board;

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

		void Init();
		void GameLoop();
		void Update();
		void Draw() const;
	};
} // namespace Cyrey 

#endif // !_CYREY_APP_HEADER
