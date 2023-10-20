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

    for (auto &cmd : replayData.mCommands)
    {
        std::vector<uint8_t> cmdData = ReplayCommand::Serialize(cmd);
        data.insert(data.end(), cmdData.begin(), cmdData.end());
    }

    data.push_back(0); // mark the end

    return data;
}

Cyrey::Replay Cyrey::Replay::Deserialize(const std::vector<std::uint8_t> &data)
{
    Replay replay{};

    char magic[6];
    magic[5] = 0;
    std::memcpy(magic, data.data(), 5);
    if (strcmp(magic, "CYREP") != 0)
    {
        ::TraceLog(::TraceLogLevel::LOG_WARNING, "File is not a Cyrey Replay!");
    }

    replay.mConfigVersion = data[5];

    int64_t scoreData;
    std::memcpy(&scoreData, data.data() + 6, 8);
    replay.mScore = scoreData;

    unsigned int seedData;
    std::memcpy(&seedData, data.data() + 14, 4);
    replay.mSeed = seedData;

    uint64_t cmdsNum = (data.size() - 19) / 8;
    for (int i = 0; i < cmdsNum; i++)
    {
        std::vector<uint8_t> cmdData(8);
        std::memcpy(cmdData.data(), data.data() + 18 + (i * 8), 8);
        replay.mCommands.push_back(ReplayCommand::Deserialize(cmdData));
    }

    return replay;
}

Cyrey::Replay Cyrey::Replay::OpenReplayFile(const char *fileName) {
    int dataRead;
    unsigned char* fileData = ::LoadFileData(fileName, &dataRead);
    std::vector<uint8_t> data(dataRead);
    std::memcpy(data.data(), fileData, dataRead);
    ::UnloadFileData(fileData);
    return Replay::Deserialize(data);
}

bool Cyrey::Replay::SaveReplayToFile(const Cyrey::Replay &replay, const char *fileName) {
    if (!::DirectoryExists(Replay::cReplaysFolderName))
        std::filesystem::create_directory(Replay::cReplaysFolderName);

    std::vector<uint8_t> data = Replay::Serialize(replay);
    return ::SaveFileData(fileName, data.data(), data.size());
}

std::vector<uint8_t> Cyrey::ReplayCommand::Serialize(const Cyrey::ReplayCommand& cmd)
{
    std::vector<uint8_t> data{};

    data.insert(data.end(), cmd.mCommandNumber);
    data.insert(data.end(), cmd.mBoardCol);
    data.insert(data.end(), cmd.mBoardRow);
    data.insert(data.end(), static_cast<uint8_t>(cmd.mDirection));
    uint8_t floatData[4];
    std::memcpy(floatData, &cmd.mSecondsSinceLastCmd, 4);
    data.insert(data.end(), {floatData[0], floatData[1], floatData[2], floatData[3]});

    return data;
}

Cyrey::ReplayCommand Cyrey::ReplayCommand::Deserialize(const std::vector<uint8_t> &data)
{
    ReplayCommand cmd{};

    cmd.mCommandNumber = data[0];
    cmd.mBoardCol = data[1];
    cmd.mBoardRow = data[2];
    cmd.mDirection = static_cast<SwapDirection>(data[3]);
    float floatData;
    std::memcpy(&floatData, data.data() + 4, 4);
    cmd.mSecondsSinceLastCmd = floatData;

    return cmd;
}
