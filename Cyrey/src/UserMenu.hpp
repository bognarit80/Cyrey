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
	};
} // namespace Cyrey

#endif // CYREY_USERMENU_HEADER
