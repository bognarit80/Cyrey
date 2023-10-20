#ifndef _CYREY_REPLAY_HEADER
#define _CYREY_REPLAY_HEADER

#include "SwapDirection.hpp"
#include <deque>
#include <vector>
#include <cstdint>

namespace Cyrey
{
	enum class SwapDirection;

	struct ReplayCommand
	{
		int mCommandNumber;
		int mBoardCol;
		int mBoardRow;
		SwapDirection mDirection;
		float mSecondsSinceLastCmd;

		static std::vector<uint8_t> Serialize(const ReplayCommand &cmd);
		static ReplayCommand Deserialize(const std::vector<uint8_t> &data);
	};

	struct Replay
	{
		int64_t mScore;
		unsigned int mSeed;
		int mConfigVersion;
		std::deque<ReplayCommand> mCommands;

        static constexpr const char *cReplaysFolderName = "replays";

		static std::vector<uint8_t> Serialize(const Replay &replayData);
		static Replay Deserialize(const std::vector<std::uint8_t> &data);
        static Replay OpenReplayFile(const char* fileName);
        static bool SaveReplayToFile(const Replay &replay, const char *fileName);
	};


} //namespace Cyrey

#endif // !_CYREY_REPLAY_HEADER
