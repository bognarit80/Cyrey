#ifndef _CYREY_USER_HEADER
#define _CYREY_USER_HEADER

#include <cstdint>

namespace Cyrey
{
	class User
	{
	public:
		int64_t mXP;
		int mGamesPlayed;
		int mGemsMatched;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int mBestMovePoints;
		int mBestMoveCascades;
	};
}

#endif // !_CYREY_USER_HEADER
