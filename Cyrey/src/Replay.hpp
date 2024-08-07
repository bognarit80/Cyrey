#ifndef CYREY_REPLAY_HEADER
#define CYREY_REPLAY_HEADER

#include "SwapDirection.hpp"
#include <deque>
#include <vector>
#include <cstdint>
#include <optional>

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

		static std::vector<uint8_t> Serialize(const ReplayCommand& cmd);
		static ReplayCommand Deserialize(const std::vector<uint8_t>& data);
	};

	struct Replay
	{
		int64_t mScore;
		unsigned int mSeed;
		int mConfigVersion;
		std::deque<ReplayCommand> mCommands;

        static constexpr char cReplaysFolderName[] = "replays";

		static std::vector<uint8_t> Serialize(const Replay& replayData);
		static std::optional<Replay> Deserialize(const std::vector<std::uint8_t>& data);
        static std::optional<Replay> OpenReplayFile(const char* fileName);
        static bool SaveReplayToFile(const Replay &replay, const char* fileName);
	};


} // namespace Cyrey

#endif // !CYREY_REPLAY_HEADER
