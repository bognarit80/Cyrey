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

        void Update();
        void Draw(); // not const because we want raygui functions to modify members
        void RefreshReplayList();

    private:
        ::FilePathList mReplayPaths;
        CyreyApp& mApp;

        // ListView variables
        int mActive;
        int mScrollIndex;
        int mFocus;


        static constexpr char cReplaysFolderPath[] = "replays";
        static constexpr char cTitleText[] = "Replays";
        static constexpr char cBackBtnText[] = "Done";

    };

} // Cyrey

#endif //CYREY_REPLAYSMENU_HPP
