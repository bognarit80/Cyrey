#ifndef CYREY_GAMECONFIG_HEADER
#define CYREY_GAMECONFIG_HEADER

namespace Cyrey
{
	struct GameConfig
	{
		int mVersion;
		int mPieceColorAmount;
		int mBoardWidth;
		int mBoardHeight;
		int mBaseScore;
		float mStartingTime;
		float mFallDelay;
		float mMissDelayMultiplier;
		int mLightningPiecesAmount;

		static GameConfig GetLatestConfig();
	};

	constexpr GameConfig cDefaultGameConfig {
		.mVersion = 1,
		.mPieceColorAmount = 7,
		.mBoardWidth = 8,
		.mBoardHeight = 8,
		.mBaseScore = 50,
		.mStartingTime = 60.0f,
		.mFallDelay = 0.2f,
		.mMissDelayMultiplier = 3.0f,
		.mLightningPiecesAmount = 10
	};
} // namespace Cyrey

#endif // !CYREY_GAMECONFIG_HEADER
