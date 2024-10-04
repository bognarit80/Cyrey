#ifndef CYREY_USER_HEADER
#define CYREY_USER_HEADER

#include <cstdint>
#include "nlohmann/json.hpp"

namespace Cyrey
{
	class User
	{
	public:
		std::string mName { "Player" };
		int64_t mXP;
		int64_t mPiecesCleared;
		int mGamesPlayed;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int64_t mBestMovePoints;
		int mBestMoveCascades;

		constexpr static int cMaxNameLength = 16;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(User, mName, mXP, mPiecesCleared, mGamesPlayed, mBombsDetonated,
		                                            mLightningsDetonated, mHypercubesDetonated, mBestMovePoints,
		                                            mBestMoveCascades)
	};
} // namespace Cyrey

#endif // !CYREY_USER_HEADER
