#ifndef CYREY_RESOURCE_MANAGER_HEADER
#define CYREY_RESOURCE_MANAGER_HEADER

#include "raylib.h"
#include <map>
#include <string>

namespace Cyrey
{
	class ResourceManager
	{
	public:
		static constexpr char cRootResorcesPath[] = "resources/";

		std::map<std::string, ::Music> mMusics
		{
			{"mainMenuTheme.ogg", ::Music{}},
			{"gameplayBlitz1min.ogg", ::Music{}},
			{"resultsScreenBlitz1min.ogg", ::Music{}}
		};
		std::map<std::string, ::Sound> mSounds
		{
			{"badMove.ogg", ::Sound{}},
			{"boardAppear.ogg", ::Sound{}},
			{"bombCreate.ogg", ::Sound{}},
			{"bombExplode.ogg", ::Sound{}},
			{"doubleset.ogg", ::Sound{}},
			{"hypercubeCreate.ogg", ::Sound{}},
			{"hypercubeExplode.ogg", ::Sound{}},
			{"lightningCreate.ogg", ::Sound{}},
			{"lightningExplode.ogg", ::Sound{}},
			{"match.ogg", ::Sound{}},
			{"pieceFall.ogg", ::Sound{}},
			{"rowBlow.ogg", ::Sound{}}
		};

		[[nodiscard]] bool HasFinishedLoading() const;
		void LoadResources();
		void UnloadResources();
		void SetVolume(float soundVolume, float musicVolume);

	private:
		bool mHasFinishedLoading;

		static constexpr std::string GetFullPath(const std::string& fileName); // try with const char* instead to avoid .c_str()?

		void LoadMusic();
		void LoadSounds();

		void UnloadMusic();
		void UnloadSounds();
	};
} // namespace Cyrey

#endif // !CYREY_RESOURCE_MANAGER_HEADER
