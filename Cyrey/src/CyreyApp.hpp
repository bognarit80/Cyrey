#ifndef CYREY_APP_HEADER
#define CYREY_APP_HEADER

#ifdef PLATFORM_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif
#include <memory>
#include <random>
#include "Board.hpp"
#include "GameConfig.hpp"
#include "MainMenu.hpp"
#include "ReplaysMenu.hpp"
#include "ResourceManager.hpp"
#include "SettingsMenu.hpp"
#include "User.hpp"
#include "UserMenu.hpp"

namespace Cyrey
{
	class Board;
	class MainMenu;
	class User;
	class SettingsMenu;
    class ReplaysMenu;
	struct GameConfig;
	class UserMenu;

	enum class CyreyAppState
	{
		Loading,
		MainMenu,
		InGame,
		SettingsMenu,
        ReplaysMenu
	};

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
		bool mDarkMode;
		int mUpdateCnt;
		CyreyAppState mState;
		CyreyAppState mChangeToState; // to prevent drawing a state before updating it at least once
		CyreyAppState mPrevState;
		std::unique_ptr<MainMenu> mMainMenu;
		bool mWantExit;
		std::unique_ptr<User> mCurrentUser;
		std::unique_ptr<SettingsMenu> mSettings;
		::Vector2 mOldWindowSize;
		GameConfig mGameConfig;
		bool mHasWindow;
		std::unique_ptr<ResourceManager> mResMgr;
        std::unique_ptr<ReplaysMenu> mReplaysMenu;
		std::unique_ptr<UserMenu> mUserMenu;

		static constexpr char cTitle[] = "Cyrey";
		static constexpr char cLoading[] = "Loading...";
		static constexpr char cUserFileName[] = "user.json";

		void Init();
		void InitWindow();
		void GameLoop();
		void Update();
		void Draw() const;
		[[nodiscard]] float GetDeltaTime() const;
		void ChangeToState(CyreyAppState state); /// Change to the state at the beginning of the next update
		void ToggleFullscreen();
		static User ParseUserFile();
		void SaveCurrentUserData() const;

		unsigned int SeedRNG(); /// Returns the seed.
		void SeedRNG(unsigned int seed); /// For replays and games with set seed.
		uint32_t GetRandomNumber(uint32_t min, uint32_t max);

	private:
		std::mt19937 mMTInstance;

	};
} // namespace Cyrey

#endif // !CYREY_APP_HEADER
