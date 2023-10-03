#ifndef _CYREY_REPLAY_HEADER
#define _CYREY_REPLAY_HEADER

#include "Board.hpp"
#include <deque>

namespace Cyrey
{
	enum class SwapDirection;

	struct ReplayCommand
	{
		int mBoardCol;
		int mBoardRow;
		SwapDirection mDirection;
		float mSecondsSinceLastCmd;
	};

	struct Replay
	{
		int64_t mScore;
		unsigned int mSeed;
		int mConfigVersion;
		std::deque<ReplayCommand> mCommands;
	};


} //namespace Cyrey

#endif // !_CYREY_REPLAY_HEADER
