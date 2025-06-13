#ifndef CYREY_SETTINGS_HEADER
#define CYREY_SETTINGS_HEADER

#include "CyreyApp.hpp"

namespace Cyrey
{
	/// User preferences. Changes in settings or through keyboard shortcuts, if I decide to keep them.
	class SettingsMenu
	{
	public:
		float mMusicVolume { cMusicVolume };
		float mSoundVolume { cSoundVolume };
		bool mIsFullscreen { cWantFullscreen };
		bool mIsVSync { cWantVSync };
		float mSwapDeadZone { cSwapDeadZone }; /// Percentage of tile size before the drag turns into a swap
		bool mWantBoardSwerve { cWantBoardSwerve };
		float mQueueSwapTolerance { cQueueSwapTolerance };
		bool mWantReplayAutoSave { cWantReplayAutoSave };

		explicit SettingsMenu(CyreyApp& app) : mApp(app) { this->OpenSettingsFile(SettingsMenu::cSettingsFileName); }

		void Update();
		void Draw(); // not const because raygui modifies variables
		void OpenSettingsFile(const std::string& path);
		bool SaveSettingsFile(const std::string& path);

	private:
		CyreyApp& mApp;

		// default consts
		static constexpr float cMusicVolume = 0.80f;
		static constexpr float cSoundVolume = 0.90f;
		static constexpr bool cWantFullscreen = false;
		static constexpr bool cWantVSync = false;
		static constexpr float cSwapDeadZone = 0.33f;
		static constexpr bool cWantBoardSwerve = true;
		static constexpr float cQueueSwapTolerance = 0.15f;
		static constexpr bool cWantReplayAutoSave = true;

#ifdef EMSCRIPTEN
		static constexpr char cSettingsFileName[] = "/data/settings.json";
#else
		static constexpr char cSettingsFileName[] = "settings.json";
#endif

		static constexpr char cWindowText[] = "Settings";
		static constexpr char cMusicSliderText[] = "Music volume";
		static constexpr char cSoundSliderText[] = "Sound volume";
		static constexpr char cSwapDeadZoneSliderText[] = "Swapping deadzone";
		static constexpr char cQueueSwapToleranceSliderText[] = "Queue swap interval";
		static constexpr char cBoardSwerveCheckText[] = "Enable Board swerve";
		static constexpr char cFullscreenCheckText[] = "Enable Fullscreen";
		static constexpr char cVsyncCheckText[] = "Enable V-Sync";
		static constexpr char cDefaultsButtonText[] = "Reset to default";
		static constexpr char cReplayAutoSaveText[] = "Auto-save Replays";
		static constexpr char cMainMenuButtonText[] = "Main Menu";
		static constexpr char cDoneButtonText[] = "Done";
		static constexpr char cOff[] = "Off";
		static constexpr char cInf[] = "Inf";
	};
} // namespace Cyrey

#endif // !CYREY_SETTINGS_HEADER
