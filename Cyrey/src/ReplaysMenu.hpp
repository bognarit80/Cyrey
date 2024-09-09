#ifndef CYREY_REPLAYSMENU_HPP
#define CYREY_REPLAYSMENU_HPP

#include <deque>
#include "CyreyApp.hpp"

namespace Cyrey
{

    class ReplaysMenu
    {
    public:
        explicit ReplaysMenu(CyreyApp& app);

        std::deque<std::string> mReplays;

        // ListView variables
        int mActive;
        int mScrollIndex;
        int mFocus;

        void Update();
        void Draw(); // not const because we want raygui functions to modify members
        void RefreshReplayList();

    private:
        CyreyApp& mApp;
        const char** mPaths; // for the ListView

        static constexpr char cTitleText[] = "Replays";
        static constexpr char cBackBtnText[] = "Back";
        static constexpr char cNoReplaysText[] = "No replays found.";
    };

} // namespace Cyrey

#endif //CYREY_REPLAYSMENU_HPP
