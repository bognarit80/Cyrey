#ifndef CYREY_GAMESTATS_HEADER
#define CYREY_GAMESTATS_HEADER

namespace Cyrey
{
	struct GameStats
	{
		int64_t mScore;
		int mPiecesCleared;
		int mMovesMade;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int mBestMoveCascades;
		int64_t mBestMovePoints;
		int mBestMoveCascadesIdx;
		int mBestMovePointsIdx;
	};
}

#endif //CYREY_GAMESTATS_HEADER
