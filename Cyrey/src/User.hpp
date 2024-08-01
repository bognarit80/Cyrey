#ifndef CYREY_USER_HEADER
#define CYREY_USER_HEADER

#include <cstdint>

namespace Cyrey
{
	class User
	{
	public:
		int64_t mXP;
		int64_t mPiecesMatched;
		int mGamesPlayed;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int mBestMovePoints;
		int mBestMoveCascades;
	};
} // namespace Cyrey

#endif // !CYREY_USER_HEADER
