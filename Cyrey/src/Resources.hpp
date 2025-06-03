#ifndef CYREY_RESOURCES_HEADER
#define CYREY_RESOURCES_HEADER

#include <cstdint>

namespace Cyrey
{
	enum class ResMusicID : uint8_t
	{
		None,
		MainMenuTheme,
		Blitz1min,
		ResultsScreenBlitz1Min
	};

	enum class ResSoundID : uint8_t
	{
		BadMove,
		BoardAppear,
		BombCreate,
		BombExplode,
		Cascade,
		HypercubeCreate,
		HypercubeExplode,
		LightningCreate,
		LightningExplode,
		Match,
		PieceFall,
		RowBlow
	};
} // namespace Cyrey

#endif // CYREY_RESOURCES_HEADER
