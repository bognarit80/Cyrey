#include "Replay.hpp"
#include <concepts>
#include <cstring>
#include <filesystem>
#include <vector>
#include "Networking.hpp"
#ifdef PLATFORM_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

// TODO: Break on invalid input, use std::optional

namespace
{
	template <typename T>
#ifndef PLATFORM_ANDROID
		requires std::is_trivially_copyable_v<T>
#endif
	void InsertIntoVector(std::vector<uint8_t>& data, T val)
	{
		data.insert(data.end(), sizeof(T), 0);
		std::memcpy(&data.back() - sizeof(T) + 1, &val, sizeof(T));
	}

	template <typename T>
		requires std::is_trivially_copyable_v<T>
	void GetFromVector(const std::span<uint8_t>& data, T& intoValue, size_t& index)
	{
		std::memcpy(&intoValue, data.data() + index, sizeof(T));
		index += sizeof(T);
	}

	std::future<Cyrey::Response> publish;
} // namespace

std::vector<uint8_t> Cyrey::Replay::Serialize(const Replay& replayData)
{
	std::vector<uint8_t> data {};

	InsertIntoVector(data, Replay::cReplayFileMagic);
	InsertIntoVector(data, replayData.mConfigVersion);
	InsertIntoVector(data, replayData.mSeed);
	InsertIntoVector(data, replayData.mStats);

	for (auto& cmd : replayData.mCommands)
	{
		std::vector<uint8_t> cmdData = ReplayCommand::Serialize(cmd);
		data.insert(data.end(), cmdData.begin(), cmdData.end());
	}

	data.push_back(0); // mark the end

	return data;
}

std::optional<Cyrey::Replay> Cyrey::Replay::Deserialize(const std::span<std::uint8_t>& data)
{
	size_t index = 0;

	// Verify the file's magic
	int magic = 0;
	if (GetFromVector(data, magic, index); magic != Replay::cReplayFileMagic)
	{
		::TraceLog(::TraceLogLevel::LOG_ERROR, "File is not a Cyrey Replay!");
		return std::nullopt;
	}

	Replay replay {};

	GetFromVector(data, replay.mConfigVersion, index);
	GetFromVector(data, replay.mSeed, index);
	GetFromVector(data, replay.mStats, index);

	int replayCmdSize = 8; // IMPORTANT: keep this up to date with changes to the structure!
	size_t cmdsAmount = (data.size() - index + 1) / replayCmdSize;
	if (cmdsAmount &&
		(cmdsAmount < replay.mStats.mMovesMade ||
		cmdsAmount - 1 < replay.mStats.mBestMovePointsIdx ||
		cmdsAmount - 1 < replay.mStats.mBestMoveCascadesIdx))
	{
		// TODO: communicate errors to the user, use exceptions?
		::TraceLog(::TraceLogLevel::LOG_ERROR, "Invalid replay stats!");
		return std::nullopt;
	}

	// way too simple, no special indicator bytes nor anything, review later
	for (size_t i = 0; i < cmdsAmount; i++)
	{
		replay.mCommands.push_back(
			ReplayCommand::Deserialize(data.subspan(index + (i * replayCmdSize), replayCmdSize)));
	}
	uint8_t nextByte = data[index + (cmdsAmount * replayCmdSize)];
	if (nextByte != 0)
	{
		::TraceLog(::TraceLogLevel::LOG_ERROR, ::TextFormat("Last byte of file is 0x%hhX instead of 0x00!", nextByte));
		return std::nullopt;
	}

	return replay;
}

std::optional<Cyrey::Replay> Cyrey::Replay::OpenReplayFile(const char* fileName)
{
	int dataRead;
	unsigned char* fileData = ::LoadFileData(fileName, &dataRead);
	if (!fileData)
		return std::nullopt;

	std::span data(fileData, dataRead);
	auto replay = Replay::Deserialize(data);
	::UnloadFileData(fileData);
	return replay;
}

bool Cyrey::Replay::SaveReplayToFile(const Replay& replay, const char* fileName)
{
#ifdef PLATFORM_ANDROID
    const char* directory = ::TextFormat("%s/%s", ::GetAndroidApp()->activity->internalDataPath, Replay::cReplaysFolderName);
#else
	const char* directory = Replay::cReplaysFolderName;
#endif
	if (!::DirectoryExists(directory))
		std::filesystem::create_directory(directory);

	std::vector<uint8_t> data = Replay::Serialize(replay);
	return ::SaveFileData(fileName, data.data(), static_cast<int>(data.size()));
}

void Cyrey::Replay::PublishReplay(const Replay& replay, const std::string& userName)
{
	publish = Networking::PostBuffer(std::format("{}/{}", Replay::cReplaysUrl, userName), Replay::Serialize(replay));
}

std::vector<uint8_t> Cyrey::ReplayCommand::Serialize(const ReplayCommand& cmd)
{
	std::vector<uint8_t> data {};

	data.insert(data.end(), cmd.mCommandNumber);
	data.insert(data.end(), cmd.mBoardCol);
	data.insert(data.end(), cmd.mBoardRow);
	data.insert(data.end(), static_cast<uint8_t>(cmd.mDirection));
	InsertIntoVector(data, cmd.mSecondsRemaining);

	return data;
}

Cyrey::ReplayCommand Cyrey::ReplayCommand::Deserialize(const std::span<uint8_t>& data)
{
	ReplayCommand cmd {};

	cmd.mCommandNumber = data[0];
	cmd.mBoardCol = data[1];
	cmd.mBoardRow = data[2];
	cmd.mDirection = SwapDirection { data[3] };
	std::memcpy(&cmd.mSecondsRemaining, data.data() + 4, sizeof(cmd.mSecondsRemaining));

	return cmd;
}
