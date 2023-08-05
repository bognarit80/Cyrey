#ifndef _CYREY_APP_HEADER
#define _CYREY_APP_HEADER

#include "raylib-cpp.hpp"
#include "Board.hpp"

namespace Cyrey 
{
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
		Board *mBoard;
		raylib::Window *mWindow;

		void Init();
		void RunGame();
		void Update();
		void Draw();
	};
}

#endif // !_CYREY_APP_HEADER
