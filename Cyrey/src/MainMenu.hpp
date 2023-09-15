#ifndef _CYREY_MAINMENU_HEADER
#define _CYREY_MAINMENU_HEADER

#include "CyreyApp.hpp"

namespace Cyrey
{
	class CyreyApp;

	class MainMenu
	{
	public:
		CyreyApp &mApp;
		bool mIsPlayBtnPressed;

		MainMenu() = default;
		MainMenu(CyreyApp& app) : mApp(app) {};
		MainMenu(const MainMenu&) = default;
		MainMenu& operator=(const MainMenu&) = default;

		void Update();
		void Draw() const;
	};
}

#endif // !_CYREY_MAINMENU_HEADER
