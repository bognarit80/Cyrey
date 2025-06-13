#ifndef CYREY_REPLAYSMENU_HEADER
#define CYREY_REPLAYSMENU_HEADER

#include <deque>
#include <optional>
#include "CyreyApp.hpp"
#include "Replay.hpp"

namespace Cyrey
{
	class ReplaysMenu
	{
	public:
		explicit ReplaysMenu(CyreyApp& app) : mApp(app) { this->RefreshReplayList(); }

		std::deque<std::string> mReplays;

		// ListView variables
		int mActive { -1 };
		int mScrollIndex { 0 };
		int mFocus { -1 };
		std::optional<Replay> mSelectedReplay;
		bool mPlayReplay { false };

		void Update();
		void Draw(); // not const because we want raygui functions to modify members
		void DrawReplayPopup();
		void RefreshReplayList();
		void ResetReplayList();

	private:
		CyreyApp& mApp;
		bool mWantServerReplays { false };
		bool mWantLeaderboard { false };
		int mCurrentPage { 1 };
		int mTotalPages { 0 };
		nlohmann::json mLeaderboardData;

		static constexpr int cPageSize = 15;

		static constexpr char cTitleText[] = "Replays";
		static constexpr char cTitleReplayInfo[] = "Replay info";
		static constexpr char cRefreshText[] = "Refresh";
		static constexpr char cBackBtnText[] = "Back";
		static constexpr char cLeaderboardTxt[] = "Show Leaderboard";
		static constexpr char cLocalTxt[] = "Show Local";
		static constexpr char cServerTxt[] = "Show Server";
		static constexpr char cNoReplaysText[] = "No replays found.";
		static constexpr char cFetchingTxt[] = "Fetching...";
		static constexpr char cPreviousTxt[] = "Previous";
		static constexpr char cNextTxt[] = "Next";

		static constexpr char cLeaderboardUrl[] = "Leaderboard";
	};
} // namespace Cyrey

#endif //CYREY_REPLAYSMENU_HEADER
