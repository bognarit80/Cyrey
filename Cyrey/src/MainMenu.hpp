#ifndef CYREY_MAINMENU_HEADER
#define CYREY_MAINMENU_HEADER

#include "CyreyApp.hpp"

namespace Cyrey
{
	class CyreyApp;

	class MainMenu
	{
	public:
		CyreyApp& mApp;
		bool mIsPlayBtnPressed { false };
		bool mIsUserPressed { false };

		static constexpr char cTitleName[] = "Cyrey";
		static constexpr char cPlayBtnText[] = "Play";
		static constexpr char cReplaysBtnText[] = "Replays";
		static constexpr char cSettingsBtnText[] = "Settings";
		static constexpr char cQuitBtnText[] = "Quit";

		static constexpr char cLocalVersion[] = "Local game config v%d";
		static constexpr char cFetchedVersion[] = "Fetched game config v%d";
		static constexpr char cFetching[] = "Fetching game config...";

		explicit MainMenu(CyreyApp& app);

		void Init();
		void Update();
		void Draw(); // not const because we want raygui functions to modify the members
		static void FetchGameConfig();
	};
} // namespace Cyrey

#endif // !_CYREY_MAINMENU_HEADER
