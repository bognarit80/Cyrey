#include "GameConfig.hpp"

Cyrey::GameConfig Cyrey::GameConfig::GetLatestConfig()
{
	// TODO: Fetch the latest config, once a server is built or something.

	// http::Request request{"/api/GameConfigs/latest"};
	// auto response = request.send("GET");
	// 
	// nlohmann::json json = nlohmann::json::parse(response.body);
	// this->mPieceColorAmount = json["PieceColorAmount"].get<int>();
	// this->mBoardWidth = json["BoardWidth"].get<int>();
	// this->mBoardHeight = json["BoardHeight"].get<int>();
	// this->mBaseScore = json["BaseScore"].get<int>();
	// this->mStartingTime = json["StartingTime"].get<float>();
	// this->mFallDelay = json["FallDelay"].get<float>();
	// this->mMissDelayMultiplier = json["MissDelayMultiplier"].get<float>();
	// this->mLightningPiecesAmount = json["LightningPiecesAmount"].get<float>();

	return Cyrey::cDefaultGameConfig;
}
