#ifndef _CYREY_SETTINGS_HEADER
#define _CYREY_SETTINGS_HEADER

#include "CyreyApp.hpp"

namespace Cyrey
{
	//User preferences. Changes in settings or through keyboard shortcuts, if I decide to keep them.
	class SettingsMenu
	{
	public:
		float mMusicVolume;
		float mSoundVolume;
		bool mIsFullscreen;
		bool mIsVSync;
		float mSwapDeadZone; //percentage of tile size before the drag turns into a swap
		bool mWantBoardSwerve;
		float mQueueSwapTolerance;

		SettingsMenu(CyreyApp& app) :
			mMusicVolume(cMusicVolume),
			mSoundVolume(cSoundVolume),
			mIsFullscreen(cWantFullscreen),
			mIsVSync(cWantVSync),
			mSwapDeadZone(cSwapDeadZone),
			mWantBoardSwerve(cWantBoardSwerve),
			mQueueSwapTolerance(cQueueSwapTolerance),
			mApp(app) {};

		void Update();
		void Draw(); //not const because raygui modifies variables

	private:
		CyreyApp& mApp;

		//default consts
		static constexpr float cMusicVolume = 0.80f;
		static constexpr float cSoundVolume = 0.90f;
		static constexpr bool cWantFullscreen = false;
		static constexpr bool cWantVSync = false;
		static constexpr float cSwapDeadZone = 0.33f;
		static constexpr bool cWantBoardSwerve = true;
		static constexpr float cQueueSwapTolerance = 0.15f;

		static constexpr char cWindowText[] = "Settings";
		static constexpr char cMusicSliderText[] = "Music volume";
		static constexpr char cSoundSliderText[] = "Sound volume";
		static constexpr char cSwapDeadZoneSliderText[] = "Swapping deadzone";
		static constexpr char cQueueSwapToleranceSliderText[] = "Queue swap interval";
		static constexpr char cBoardSwerveCheckText[] = "Enable Board swerve";
		static constexpr char cFullscreenCheckText[] = "Enable Fullscreen";
		static constexpr char cVsyncCheckText[] = "Enable V-Sync";
		static constexpr char cDefaultsButtonText[] = "Reset to default";
		static constexpr char cMainMenuButtonText[] = "Main Menu";
		static constexpr char cOff[] = "Off";
		static constexpr char cInf[] = "Inf";
	};
}

#endif // !_CYREY_SETTINGS_HEADER