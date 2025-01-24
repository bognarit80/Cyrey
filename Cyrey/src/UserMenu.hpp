#ifndef CYREY_USERMENU_HEADER
#define CYREY_USERMENU_HEADER
#include "CyreyApp.hpp"

namespace Cyrey
{
	class UserMenu
	{
	public:
		CyreyApp& mApp;
		bool mIsOpen { false };
		bool mWantNameDialog { false };

		explicit UserMenu(CyreyApp& app) : mApp(app) {}

		void Update();
		void Draw();

		static constexpr char cResetTutorialTitle[] = "Reset Tutorial?";
		static constexpr char cResetTutorialTxt[] = "Are you sure you want to reset the tutorial?";
	};
} // namespace Cyrey

#endif // CYREY_USERMENU_HEADER
