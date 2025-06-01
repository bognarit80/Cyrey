#include "Replay.hpp"
#include <concepts>
#include <cstring>
#include <filesystem>
#include <vector>
#ifdef PLATFORM_DESKTOP
#include "raylib_win32.h"
#endif
#include "cpr/cpr.h"
#ifdef PLATFORM_ANDROID
#include "raymob.h"
#else
#include "raylib.h"
#endif

// TODO: Break on invalid input, use std::optional

template <typename T>
#ifndef PLATFORM_ANDROID
	requires std::is_trivially_copyable_v<T>
#endif
static void InsertIntoVector(std::vector<uint8_t>& data, T val)
{
	data.insert(data.end(), sizeof(T), 0);
	std::memcpy(&data.back() - sizeof(T) + 1, &val, sizeof(T));
}

template <typename T>
	requires std::is_trivially_copyable_v<T>
void GetFromVector(const std::vector<uint8_t>& data, T& intoValue, size_t& index)
{
	std::memcpy(&intoValue, data.data() + index, sizeof(T));
	index += sizeof(T);
}

std::vector<uint8_t> Cyrey::Replay::Serialize(const Replay& replayData)
{
	std::vector<uint8_t> data {};

	data.insert(data.end(),
	            cReplayFileMagic, &cReplayFileMagic[std::size(cReplayFileMagic) - 1]); // don't push null terminator

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

std::optional<Cyrey::Replay> Cyrey::Replay::Deserialize(const std::vector<std::uint8_t>& data)
{
	size_t index = 0;

	// Verify the file's magic
	constexpr size_t arr = std::size(cReplayFileMagic);
	if (memcmp(data.data() + index, cReplayFileMagic, arr - 1) != 0)
	{
		::TraceLog(::TraceLogLevel::LOG_ERROR, "File is not a Cyrey Replay!");
		return std::nullopt;
	}
	index += arr - 1;

	Replay replay {};

	GetFromVector(data, replay.mConfigVersion, index);
	GetFromVector(data, replay.mSeed, index);
	GetFromVector(data, replay.mStats, index);

	int replayCmdSize = 8; // IMPORTANT: keep this up to date with changes to the structure!
	uint64_t cmdsAmount = (data.size() - index + 1) / replayCmdSize;
	// way too simple, no special indicator bytes nor anything, review later
	for (int i = 0; i < cmdsAmount; i++)
	{
		std::vector<uint8_t> cmdData(replayCmdSize);
		std::memcpy(cmdData.data(), data.data() + index + (i * replayCmdSize), replayCmdSize);
		replay.mCommands.push_back(ReplayCommand::Deserialize(cmdData));
	}
	unsigned char nextByte = data[index + (cmdsAmount * replayCmdSize)];
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

	std::vector<uint8_t> data(dataRead);
	std::memcpy(data.data(), fileData, dataRead);
	::UnloadFileData(fileData);
	return Replay::Deserialize(data);
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
	std::thread([=]
	{
		auto data = Replay::Serialize(replay);
		auto response = cpr::Post(cpr::Url { Replay::cReplaysUrl + userName },
		                          cpr::Timeout { 10000 },
		                          cpr::ConnectTimeout { 500 },
		                          cpr::Header { { "Content-Type", "application/octet-stream" } },
		                          cpr::Body { cpr::Buffer { data.begin(), data.end(), "" } });
	}).detach();
}

std::vector<uint8_t> Cyrey::ReplayCommand::Serialize(const ReplayCommand& cmd)
{
	std::vector<uint8_t> data {};

	data.insert(data.end(), cmd.mCommandNumber);
	data.insert(data.end(), cmd.mBoardCol);
	data.insert(data.end(), cmd.mBoardRow);
	data.insert(data.end(), static_cast<uint8_t>(cmd.mDirection));
	uint8_t floatData[sizeof(cmd.mSecondsSinceLastCmd)];
	std::memcpy(floatData, &cmd.mSecondsSinceLastCmd, sizeof(cmd.mSecondsSinceLastCmd));
	data.insert(data.end(), { floatData[0], floatData[1], floatData[2], floatData[3] });

	return data;
}

Cyrey::ReplayCommand Cyrey::ReplayCommand::Deserialize(const std::vector<uint8_t>& data)
{
	ReplayCommand cmd {};

	cmd.mCommandNumber = data[0];
	cmd.mBoardCol = data[1];
	cmd.mBoardRow = data[2];
	cmd.mDirection = static_cast<SwapDirection>(data[3]);
	std::memcpy(&cmd.mSecondsSinceLastCmd, data.data() + 4, sizeof(cmd.mSecondsSinceLastCmd));

	return cmd;
}
