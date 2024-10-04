#ifndef CYREY_RESOURCE_MANAGER_HEADER
#define CYREY_RESOURCE_MANAGER_HEADER

#include <map>
#include <string>
#include "raylib.h"

namespace Cyrey
{
	class ResourceManager
	{
	public:
#ifdef PLATFORM_ANDROID // Android assets have their own folder, no need to layer the "resources" one on top of it.
		static constexpr char cRootResorcesPath[] = "";
#else
		static constexpr char cRootResorcesPath[] = "resources/";
#endif

		std::map<std::string, ::Music> mMusics
		{
			{ "mainMenuTheme.ogg", ::Music {} },
			{ "gameplayBlitz1min.ogg", ::Music {} },
			{ "resultsScreenBlitz1min.ogg", ::Music {} }
		};
		std::map<std::string, ::Sound> mSounds
		{
			{ "badMove.ogg", ::Sound {} },
			{ "boardAppear.ogg", ::Sound {} },
			{ "bombCreate.ogg", ::Sound {} },
			{ "bombExplode.ogg", ::Sound {} },
			{ "doubleset.ogg", ::Sound {} },
			{ "hypercubeCreate.ogg", ::Sound {} },
			{ "hypercubeExplode.ogg", ::Sound {} },
			{ "lightningCreate.ogg", ::Sound {} },
			{ "lightningExplode.ogg", ::Sound {} },
			{ "match.ogg", ::Sound {} },
			{ "pieceFall.ogg", ::Sound {} },
			{ "rowBlow.ogg", ::Sound {} }
		};

		[[nodiscard]] bool HasFinishedLoading() const;
		void LoadResources();
		void UnloadResources();
		void SetVolume(float soundVolume, float musicVolume);

	private:
		bool mHasFinishedLoading { false };

		static std::string GetFullPath(const std::string& fileName); // try with const char* instead to avoid .c_str()?

		void LoadMusic();
		void LoadSounds();

		void UnloadMusic();
		void UnloadSounds();
	};
} // namespace Cyrey

#endif // !CYREY_RESOURCE_MANAGER_HEADER
