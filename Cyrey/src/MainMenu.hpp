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
		static constexpr char cTitleName[] = "Cyrey";
		static constexpr char cPlayBtnText[] = "Play";
		static constexpr char cSettingsBtnText[] = "Settings";
		static constexpr char cQuitBtnText[] = "Quit";

		MainMenu(CyreyApp& app) : mApp(app) {};

		void Init();
		void Update();
		void Draw(); //not const because we want raygui functions to modify the members
	};
}

#endif // !_CYREY_MAINMENU_HEADER
