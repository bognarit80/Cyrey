#ifndef CYREY_RESOURCE_MANAGER_HEADER
#define CYREY_RESOURCE_MANAGER_HEADER

#include <map>
#include <string>
#include "raylib.h"
#include "Resources.hpp"

namespace Cyrey
{
	class ResourceManager
	{
	public:
		std::map<ResMusicID, ::Music> mMusics
		{
			{ ResMusicID::MainMenuTheme, ::Music {} },
			{ ResMusicID::Blitz1min, ::Music {} },
			{ ResMusicID::ResultsScreenBlitz1Min, ::Music {} }
		};
		std::map<ResSoundID, ::Sound> mSounds
		{
			{ ResSoundID::BadMove, ::Sound {} },
			{ ResSoundID::BoardAppear, ::Sound {} },
			{ ResSoundID::BombCreate, ::Sound {} },
			{ ResSoundID::BombExplode, ::Sound {} },
			{ ResSoundID::Cascade, ::Sound {} },
			{ ResSoundID::HypercubeCreate, ::Sound {} },
			{ ResSoundID::HypercubeExplode, ::Sound {} },
			{ ResSoundID::LightningCreate, ::Sound {} },
			{ ResSoundID::LightningExplode, ::Sound {} },
			{ ResSoundID::Match, ::Sound {} },
			{ ResSoundID::PieceFall, ::Sound {} },
			{ ResSoundID::RowBlow, ::Sound {} }
		};

		[[nodiscard]] bool HasFinishedLoading() const;
		void LoadResources();
		void UnloadResources();
		void SetVolume(float soundVolume, float musicVolume);

	private:
		bool mHasFinishedLoading { false };
		std::map<ResMusicID, std::string> mMusicNames
		{
			{ ResMusicID::MainMenuTheme, "mainMenuTheme.ogg" },
			{ ResMusicID::Blitz1min, "gameplayBlitz1min.ogg" },
			{ ResMusicID::ResultsScreenBlitz1Min, "resultsScreenBlitz1min.ogg" }
		};
		std::map<ResSoundID, std::string> mSoundNames
		{
			{ ResSoundID::BadMove, "badMove.ogg" },
			{ ResSoundID::BoardAppear, "boardAppear.ogg" },
			{ ResSoundID::BombCreate, "bombCreate.ogg" },
			{ ResSoundID::BombExplode, "bombExplode.ogg" },
			{ ResSoundID::Cascade, "doubleset.ogg" },
			{ ResSoundID::HypercubeCreate, "hypercubeCreate.ogg" },
			{ ResSoundID::HypercubeExplode, "hypercubeExplode.ogg" },
			{ ResSoundID::LightningCreate, "lightningCreate.ogg" },
			{ ResSoundID::LightningExplode, "lightningExplode.ogg" },
			{ ResSoundID::Match, "match.ogg" },
			{ ResSoundID::PieceFall, "pieceFall.ogg" },
			{ ResSoundID::RowBlow, "rowBlow.ogg" }
		};

#ifdef PLATFORM_ANDROID // Android assets have their own folder, no need to layer the "resources" one on top of it.
		static constexpr char cRootResorcesPath[] = "";
#else
		static constexpr char cRootResorcesPath[] = "resources/";
#endif

		static std::string GetFullPath(const std::string& fileName);

		void LoadMusic();
		void LoadSounds();

		void UnloadMusic();
		void UnloadSounds();
	};
} // namespace Cyrey

#endif // !CYREY_RESOURCE_MANAGER_HEADER
