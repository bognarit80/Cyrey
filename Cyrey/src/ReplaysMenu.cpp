#include "ReplaysMenu.hpp"
#include "raygui.h"
#include "nlohmann/json.hpp"
#ifdef PLATFORM_DESKTOP
#include "raylib_win32.h"
#endif
#include "cpr/cpr.h"

static bool loading = false;

void Cyrey::ReplaysMenu::Update()
{
	if (this->mActive >= 0)
	{
		// TODO: Make this async later, for now it's already ~1 frame of waiting in both network and local cases
		std::string fileName;
		if (this->mWantLeaderboard)
			fileName = this->mLeaderboardData[this->mActive]["fileName"].get<std::string>();
		else if (this->mWantServerReplays)
			fileName = this->mReplays[this->mActive];

		if (fileName.empty())
			this->mSelectedReplay = Replay::OpenReplayFile(
				(Replay::cReplaysFolderName + this->mReplays[this->mActive]).c_str());
		else
		{
			cpr::Response r = cpr::Get(cpr::Url { Replay::cReplaysUrl },
			                           cpr::Parameters { { "fileName", fileName } },
			                           cpr::Timeout { 10000 },
			                           cpr::ConnectTimeout { 500 });
			if (r.status_code == 200)
			{
				std::vector<std::uint8_t> data(r.text.size());
				std::memcpy(data.data(), r.text.c_str(), r.text.size());
				this->mSelectedReplay = Replay::Deserialize(data);
			}
		}
		this->mActive = -1;
	}
}

void Cyrey::ReplaysMenu::Draw()
{
	auto appWidth = static_cast<float>(this->mApp.mWidth);
	auto appHeight = static_cast<float>(this->mApp.mHeight);

	float windowY = appHeight * 0.1f;
	float windowHeight = appHeight - (windowY * 2);
	float windowX = appWidth > appHeight ? (appWidth - windowHeight) / 2 : appWidth * 0.1f;
	float windowWidth = windowY < windowX ? windowHeight : appWidth - (windowX * 2); //square if window is wide
	Vector2 windowAnchor = { windowX, windowY };
	Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };
	float windowPaddingX = windowWidth * 0.025f;
	float windowPaddingY = windowHeight * 0.065f;
	Rectangle listRect = {
		windowX + windowPaddingX,
		windowY + windowPaddingY,
		windowWidth - (windowPaddingX * 2),
		windowHeight - (windowPaddingY * 3.2f)
	};

	float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
	::GuiSetStyle(::GuiControl::LISTVIEW, ::GuiListViewProperty::LIST_ITEMS_HEIGHT, fontSize);
	::GuiSetIconScale(fontSize / 16);

	Rectangle leaderboardBtnPos = {
		windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 2.9f),
		(windowWidth - (windowPaddingX * 2)) * 0.475f,
		fontSize
	};
	Rectangle serverBtnPos = {
		windowAnchor.x + (windowWidth / 2) + windowPaddingX,
		windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 2.9f),
		(windowWidth - (windowPaddingX * 2)) * 0.475f,
		fontSize
	};
	Rectangle refreshBtnPos = {
		windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 1.9f),
		(windowWidth - (windowPaddingX * 2)) * 0.475f,
		fontSize
	};
	Rectangle doneBtnPos = {
		windowAnchor.x + (windowWidth / 2) + windowPaddingX,
		windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 1.9f),
		(windowWidth - (windowPaddingX * 2)) * 0.475f,
		fontSize
	};
	Rectangle noReplaysPos = {
		windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY,
		windowWidth - (windowPaddingX * 2),
		fontSize
	};


	if (this->mSelectedReplay.has_value())
	{
		if (::GuiWindowBox(windowRect, ReplaysMenu::cTitleReplayInfo) ||
			::GuiButton(doneBtnPos, ::GuiIconText(::GuiIconName::ICON_EXIT, ReplaysMenu::cBackBtnText)))
			this->mSelectedReplay = std::nullopt;

		if (::GuiButton(refreshBtnPos, ::GuiIconText(::GuiIconName::ICON_PLAYER_PLAY, "Play")))
			this->mPlayReplay = true;

		float controlOffsetY = fontSize * 1.3f;
		float controlPaddingX = windowWidth * 0.05f;

		const std::map<const char*, std::string> labelValueMap {
			{ "Moves: ", ::TextFormat("%d", this->mSelectedReplay->mMovesMade) },
			{
				"Moves per second: ", ::TextFormat("%.2f",
				                                   static_cast<float>(this->mSelectedReplay->mMovesMade) / this->mApp.
				                                   mGameConfig.mStartingTime)
			},
			{ "Bombs: ", ::TextFormat("%d", this->mSelectedReplay->mBombsDetonated) },
			{ "Lightnings: ", ::TextFormat("%d", this->mSelectedReplay->mLightningsDetonated) },
			{ "Hypercubes: ", ::TextFormat("%d", this->mSelectedReplay->mHypercubesDetonated) },
			{ "Best move (points): ", ::TextFormat("%d", this->mSelectedReplay->mBestMovePoints) },
			{ "Highest cascade: ", ::TextFormat("%d", this->mSelectedReplay->mBestMoveCascades) },
			{ "Pieces cleared: ", ::TextFormat("%d", this->mSelectedReplay->mPiecesCleared) }
		};

		int i = 3;
		for (auto& [label, value] : labelValueMap)
		{
			::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT,
			              ::GuiTextAlignment::TEXT_ALIGN_RIGHT);
			Rectangle labelPos = {
				windowAnchor.x + controlPaddingX,
				windowAnchor.y + windowPaddingY + (controlOffsetY * static_cast<float>(i)),
				(windowWidth / 2) - controlPaddingX,
				fontSize
			};
			::GuiLabel(labelPos, label);

			::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT,
			              ::GuiTextAlignment::TEXT_ALIGN_LEFT);
			Rectangle valuePos = {
				windowAnchor.x + (windowWidth / 2),
				windowAnchor.y + windowPaddingY + (controlOffsetY * static_cast<float>(i)),
				(windowWidth / 2) - controlPaddingX,
				fontSize
			};
			::GuiLabel(valuePos, value.c_str());
			i++;
		}
		float fontSizeTitle = windowHeight > windowWidth ? windowWidth / 12 : windowHeight / 12;
		::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, static_cast<int>(fontSizeTitle));
		::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_CENTER);
		Rectangle finalScoreLabel = {
			windowAnchor.x,
			windowAnchor.y + windowPaddingY + controlOffsetY,
			windowWidth,
			fontSizeTitle
		};
		::GuiLabel(finalScoreLabel,
		           ::TextFormat("Blitz %ds: %lld pts", static_cast<int>(this->mApp.mGameConfig.mStartingTime),
		                        this->mSelectedReplay->mScore));

		return;
	}

	if (::GuiWindowBox(windowRect, ReplaysMenu::cTitleText) ||
		::GuiButton(doneBtnPos, ::GuiIconText(::GuiIconName::ICON_EXIT, ReplaysMenu::cBackBtnText)))
	{
		if (this->mWantLeaderboard)
		{
			this->mWantLeaderboard = false;
			this->RefreshReplayList();
		}
		else
			this->mApp.ChangeToState(CyreyAppState::MainMenu);
	}

	if (::GuiButton(refreshBtnPos, ::GuiIconText(::GuiIconName::ICON_RESTART, ReplaysMenu::cRefreshText)))
		this->RefreshReplayList();

	if (this->mWantLeaderboard)
	{
		if (this->mCurrentPage <= 1)
			::GuiDisable();
		if (::GuiButton(leaderboardBtnPos, ReplaysMenu::cPreviousTxt))
		{
			this->mCurrentPage--;
			this->RefreshReplayList();
		}
		::GuiEnable();

		if (this->mCurrentPage >= this->mTotalPages || !this->mTotalPages)
			::GuiDisable();
		if (::GuiButton(serverBtnPos, ReplaysMenu::cNextTxt))
		{
			this->mCurrentPage++;
			this->RefreshReplayList();
		}
		::GuiEnable();
	}
	else
	{
		if (::GuiButton(leaderboardBtnPos, ReplaysMenu::cLeaderboardTxt))
		{
			this->mWantLeaderboard = true;
			this->mCurrentPage = 1;
			this->RefreshReplayList();
		}
		if (::GuiButton(serverBtnPos, this->mWantServerReplays ? ReplaysMenu::cLocalTxt : ReplaysMenu::cServerTxt))
		{
			this->mWantServerReplays ^= 1;
			this->RefreshReplayList();
		}
	}

	if (loading)
	{
		::GuiLabel(noReplaysPos, ReplaysMenu::cFetchingTxt);
		return;
	}

	const int size = static_cast<int>(this->mReplays.size());
	auto paths = new const char*[size];
	for (int i = 0; i < size; ++i)
	{
		paths[i] = this->mReplays[i].c_str();
	}
	::GuiListViewEx(listRect,
	                paths,
	                size,
	                &this->mScrollIndex,
	                &this->mActive,
	                &this->mFocus);

	if (size <= 0)
		::GuiLabel(noReplaysPos, ReplaysMenu::cNoReplaysText);
	delete[] paths;
}

// A pretty hacky way to fetch these lists asynchronously. Probably wanna fix later but right now it's not that big of a deal.
void Cyrey::ReplaysMenu::RefreshReplayList()
{
	if (loading)
		return; // we don't want more than one thread like this running at the same time

	loading = true;
	std::thread([this]
	{
		this->mReplays.clear();
		if (this->mWantLeaderboard)
		{
			cpr::Response r = cpr::Get(cpr::Url { ReplaysMenu::cLeaderboardUrl },
			                           cpr::Parameters {
				                           { "page", std::to_string(this->mCurrentPage) },
				                           { "size", std::to_string(ReplaysMenu::cPageSize) }
			                           },
			                           cpr::Timeout { 10000 },
			                           cpr::ConnectTimeout { 500 });
			if (r.status_code == 200)
			{
				this->mTotalPages = std::stoi(r.header["TotalPages"]);
				int idx = 1;
				this->mLeaderboardData = nlohmann::json::parse(r.text);
				for (auto rep : this->mLeaderboardData)
				{
					auto str = ::TextFormat("%d. %s %lld pts %s",
					                        idx + (this->mCurrentPage - 1) * ReplaysMenu::cPageSize,
					                        rep["playerName"].get<std::string>().c_str(),
					                        rep["mScore"].get<int64_t>(),
					                        rep["achievedOn"].get<std::string>().substr(0, 19).c_str());
					this->mReplays.emplace_back(str);
					idx++;
				}
			}
		}
		else if (this->mWantServerReplays)
		{
			cpr::Response r = cpr::Get(cpr::Url { Replay::cReplaysUrl + this->mApp.mCurrentUser->mName },
			                           cpr::Timeout { 10000 },
			                           cpr::ConnectTimeout { 500 });
			if (r.status_code == 200)
			{
				for (auto rep : nlohmann::json::parse(r.text))
				{
					this->mReplays.emplace_front(rep["fileName"]);
				}
			}
		}
		else
		{
#ifdef PLATFORM_ANDROID
            const char* directory = ::TextFormat("%s/%s", ::GetAndroidApp()->activity->internalDataPath, Replay::cReplaysFolderName);
#else
			const char* directory = Replay::cReplaysFolderName;
#endif
			if (!::DirectoryExists(directory))
				std::filesystem::create_directory(directory);

			for (const auto& entry : std::filesystem::directory_iterator(directory))
			{
				if (entry.path().extension() == Replay::cReplayFileExtension)
					this->mReplays.emplace_front(entry.path().filename().generic_string());
			}
		}

		// reset the window's parameters, protect against out of bounds errors as we just changed the amount of list entries
		this->mScrollIndex = 0;
		this->mActive = -1;
		this->mFocus = -1;
		loading = false;
	}).detach();
}
