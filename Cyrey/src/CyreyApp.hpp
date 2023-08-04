#ifndef _CYREY_APP_HEADER
#define _CYREY_APP_HEADER

#include "raylib-cpp.hpp"

namespace Cyrey 
{
	class CyreyApp
	{

	public:

		CyreyApp() = default;
		CyreyApp(CyreyApp& other) = delete;
		CyreyApp(const CyreyApp& other) = delete;
		CyreyApp& operator=(const CyreyApp& other) = delete;

		int mScreenWidth;
		int mScreenHeight;
		int mRefreshRate;

		void Init();
		void RunGame();
		void Update();
		void Draw();
	};
}

#endif // !_CYREY_APP_HEADER
