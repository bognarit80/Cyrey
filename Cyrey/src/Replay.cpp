#include "Replay.hpp"
#include <vector>
#include <cstring>
#include <filesystem>
#include "raylib.h"

// TODO: Break on invalid input, use std::optional

std::vector<uint8_t> Cyrey::Replay::Serialize(const Replay& replayData)
{
    std::vector<uint8_t> data{};

    data.insert(data.end(), {'C', 'Y', 'R', 'E', 'P'});

    data.insert(data.end(), replayData.mConfigVersion); // are we going to have more than 255 versions?
    data.insert(data.end(), { static_cast<uint8_t>((replayData.mScore & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 8) & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 16) & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 24) & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 32) & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 40) & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 48) & 0xFF)),
        static_cast<uint8_t>(((replayData.mScore >> 56) & 0xFF)),
    });

    data.insert(data.end(), { static_cast<uint8_t>((replayData.mSeed & 0xFF)),
        static_cast<uint8_t>(((replayData.mSeed >> 8) & 0xFF)),
        static_cast<uint8_t>(((replayData.mSeed >> 16) & 0xFF)),
        static_cast<uint8_t>(((replayData.mSeed >> 24) & 0xFF))
    });

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
    // Verify the file's magic
    char magic[6];
    magic[5] = 0;
    std::memcpy(magic, data.data(), 5);
    if (strcmp(magic, "CYREP") != 0)
    {
        ::TraceLog(::TraceLogLevel::LOG_ERROR, "File is not a Cyrey Replay!");
        return std::nullopt;
    }

    Replay replay{};

    replay.mConfigVersion = data[5];

    std::memcpy(&replay.mScore, data.data() + 6, sizeof(replay.mScore));
    std::memcpy(&replay.mSeed, data.data() + 14, sizeof(replay.mSeed));

    int replayCmdSize = 8; // IMPORTANT: keep this up to date with changes to the structure!
    uint64_t cmdsAmount = (data.size() - 19) / replayCmdSize; // way too simple, no special indicator bytes nor anything, review later
    for (int i = 0; i < cmdsAmount; i++)
    {
        std::vector<uint8_t> cmdData(replayCmdSize);
        std::memcpy(cmdData.data(), data.data() + 18 + (i * replayCmdSize), replayCmdSize);
        replay.mCommands.push_back(ReplayCommand::Deserialize(cmdData));
    }
    unsigned char nextByte = data[18 + (cmdsAmount * replayCmdSize)];
    if (nextByte != 0)
    {
        ::TraceLog(::TraceLogLevel::LOG_ERROR, ::TextFormat("Last byte of file is 0x%hhX instead of 0x00!", nextByte));
        return std::nullopt;
    }

    return replay;
}

std::optional<Cyrey::Replay> Cyrey::Replay::OpenReplayFile(const char* fileName) {
    int dataRead;
    unsigned char* fileData = ::LoadFileData(fileName, &dataRead);
    if (!fileData)
        return std::nullopt;

    std::vector<uint8_t> data(dataRead);
    std::memcpy(data.data(), fileData, dataRead);
    ::UnloadFileData(fileData);
    return Replay::Deserialize(data);
}

bool Cyrey::Replay::SaveReplayToFile(const Replay& replay, const char* fileName) {
    if (!::DirectoryExists(Replay::cReplaysFolderName))
        std::filesystem::create_directory(Replay::cReplaysFolderName);

    std::vector<uint8_t> data = Replay::Serialize(replay);
    return ::SaveFileData(fileName, data.data(), static_cast<int>(data.size()));
}

std::vector<uint8_t> Cyrey::ReplayCommand::Serialize(const ReplayCommand& cmd)
{
    std::vector<uint8_t> data{};

    data.insert(data.end(), cmd.mCommandNumber);
    data.insert(data.end(), cmd.mBoardCol);
    data.insert(data.end(), cmd.mBoardRow);
    data.insert(data.end(), static_cast<uint8_t>(cmd.mDirection));
    uint8_t floatData[sizeof(cmd.mSecondsSinceLastCmd)];
    std::memcpy(floatData, &cmd.mSecondsSinceLastCmd, sizeof(cmd.mSecondsSinceLastCmd));
    data.insert(data.end(), {floatData[0], floatData[1], floatData[2], floatData[3]});

    return data;
}

Cyrey::ReplayCommand Cyrey::ReplayCommand::Deserialize(const std::vector<uint8_t>& data)
{
    ReplayCommand cmd{};

    cmd.mCommandNumber = data[0];
    cmd.mBoardCol = data[1];
    cmd.mBoardRow = data[2];
    cmd.mDirection = static_cast<SwapDirection>(data[3]);
    std::memcpy(&cmd.mSecondsSinceLastCmd, data.data() + 4, sizeof(cmd.mSecondsSinceLastCmd));

    return cmd;
}
