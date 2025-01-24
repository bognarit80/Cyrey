#ifndef CYREY_USER_HEADER
#define CYREY_USER_HEADER

#include <cstdint>
#include "nlohmann/json.hpp"

namespace Cyrey
{
	class User
	{
	public:
		std::string mName { cDefaultName };
		int64_t mXP;
		int64_t mPiecesCleared;
		int mGamesPlayed;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int64_t mBestMovePoints;
		int mBestMoveCascades;
		bool mFinishedTutorial;

		static constexpr int cMaxNameLength = 16;
		static constexpr char cDefaultName[] = "Player";

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(User, mName, mXP, mPiecesCleared, mGamesPlayed, mBombsDetonated,
		                                            mLightningsDetonated, mHypercubesDetonated, mBestMovePoints,
		                                            mBestMoveCascades, mFinishedTutorial)
	};
} // namespace Cyrey

#endif // !CYREY_USER_HEADER
