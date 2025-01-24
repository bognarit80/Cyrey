#ifndef CYREY_GAMECONFIG_HEADER
#define CYREY_GAMECONFIG_HEADER
#include "nlohmann/json.hpp"

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

		static std::optional<GameConfig> ParseConfig(const std::string& cfg);

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(GameConfig, mVersion, mPieceColorAmount, mBoardWidth, mBoardHeight, mBaseScore,
		                               mStartingTime, mFallDelay, mMissDelayMultiplier, mLightningPiecesAmount);

		constexpr static char cLatestConfigUrl[] = CYREY_APIURL "GameConfig/latest";
	};

	constexpr GameConfig cDefaultGameConfig {
		.mVersion = 2,
		.mPieceColorAmount = 7,
		.mBoardWidth = 8,
		.mBoardHeight = 8,
		.mBaseScore = 50,
		.mStartingTime = 60.0f,
		.mFallDelay = 0.2f,
		.mMissDelayMultiplier = 3.0f,
		.mLightningPiecesAmount = 10
	};
	constexpr GameConfig cTutorialGameConfig {
		.mVersion = 1,
		.mPieceColorAmount = 7,
		.mBoardWidth = 8,
		.mBoardHeight = 8,
		.mBaseScore = 50,
		.mStartingTime = 60.0f,
		.mFallDelay = 0.75f,
		.mMissDelayMultiplier = 3.0f,
		.mLightningPiecesAmount = 10
	};
} // namespace Cyrey

#endif // !CYREY_GAMECONFIG_HEADER
