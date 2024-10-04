#ifndef CYREY_REPLAY_HEADER
#define CYREY_REPLAY_HEADER

#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <vector>
#include "SwapDirection.hpp"

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

		ReplayCommand() = default;

		ReplayCommand(int mCommandNumber, int mBoardCol, int mBoardRow, SwapDirection mDirection,
		              float mSecondsSinceLastCmd) : mCommandNumber(mCommandNumber),
		                                            mBoardCol(mBoardCol), mBoardRow(mBoardRow),
		                                            mDirection(mDirection),
		                                            mSecondsSinceLastCmd(mSecondsSinceLastCmd) {}

		static std::vector<uint8_t> Serialize(const ReplayCommand& cmd);
		static ReplayCommand Deserialize(const std::vector<uint8_t>& data);
	};

	struct Replay
	{
		int64_t mScore;
		unsigned int mSeed;
		int mConfigVersion;
		int mMovesMade;
		int mBombsDetonated;
		int mLightningsDetonated;
		int mHypercubesDetonated;
		int64_t mBestMovePoints;
		int mBestMoveCascades;
		int mPiecesCleared;
		std::deque<ReplayCommand> mCommands;

		static constexpr char cReplaysFolderName[] = "replays/";
		static constexpr char cReplayFileExtension[] = ".cyrep";
		static constexpr char cReplayFileMagic[] = "CYREP";
		static constexpr char cReplaysUrl[] = CYREY_APIURL "Replays/";

		static std::vector<uint8_t> Serialize(const Replay& replayData);
		static std::optional<Replay> Deserialize(const std::vector<std::uint8_t>& data);
		static std::optional<Replay> OpenReplayFile(const char* fileName);
		static bool SaveReplayToFile(const Replay& replay, const char* fileName);
		/// Temporarily in this class, move later
		static void PublishReplay(const Replay& replay, const std::string& userName);
	};
} // namespace Cyrey

#endif // !CYREY_REPLAY_HEADER
