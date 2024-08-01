#ifndef CYREY_REPLAYSMENU_HPP
#define CYREY_REPLAYSMENU_HPP

#include "raylib.h"
#include "CyreyApp.hpp"

namespace Cyrey
{

    class ReplaysMenu
    {
    public:
        explicit ReplaysMenu(CyreyApp& app);
        ~ReplaysMenu();

        ::FilePathList mReplayPaths;

        // ListView variables
        int mActive;
        int mScrollIndex;
        int mFocus;

        void Update();
        void Draw(); // not const because we want raygui functions to modify members
        void RefreshReplayList();

    private:
        CyreyApp& mApp;

        static constexpr char cReplaysFolderPath[] = "replays";
        static constexpr char cTitleText[] = "Replays";
        static constexpr char cBackBtnText[] = "Back";
        static constexpr char cNoReplaysText[] = "No replays found.";
    };

} // namespace Cyrey

#endif //CYREY_REPLAYSMENU_HPP
