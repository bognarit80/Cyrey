#include "ReplaysMenu.hpp"
#include "raygui.h"

Cyrey::ReplaysMenu::ReplaysMenu(CyreyApp& app) : mApp(app)
{
    this->RefreshReplayList(); // inits the rest of the members
}

void Cyrey::ReplaysMenu::Update()
{

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
    Rectangle listRect = { windowX + windowPaddingX,
                           windowY + windowPaddingY,
                           windowWidth - (windowPaddingX * 2),
                           windowHeight - (windowPaddingY * 2.1f)};

    float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
    ::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
    ::GuiSetStyle(::GuiControl::LISTVIEW, ::GuiListViewProperty::LIST_ITEMS_HEIGHT, fontSize);
    ::GuiSetIconScale(fontSize / 16);

    Rectangle refreshBtnPos = { windowAnchor.x + windowPaddingX,
                                windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 1.9f),
                                (windowWidth - (windowPaddingX * 2)) * 0.475f,
                                fontSize
    };
    Rectangle doneBtnPos = { windowAnchor.x + (windowWidth / 2) + windowPaddingX,
                             windowAnchor.y + windowPaddingY + (windowHeight - windowPaddingY * 1.9f),
                             (windowWidth - (windowPaddingX * 2)) * 0.475f,
                             fontSize
    };
    Rectangle noReplaysPos = { windowAnchor.x + windowPaddingX,
                               windowAnchor.y + windowPaddingY,
                               windowWidth - (windowPaddingX * 2),
                               fontSize
    };

    if (::GuiWindowBox(windowRect, ReplaysMenu::cTitleText) ||
        ::GuiButton(doneBtnPos, ::GuiIconText(::GuiIconName::ICON_OK_TICK, ReplaysMenu::cBackBtnText)))
        this->mApp.ChangeToState(CyreyAppState::MainMenu);

    if (::GuiButton(refreshBtnPos, ::GuiIconText(::GuiIconName::ICON_RESTART, "Refresh")))
        this->RefreshReplayList();


    const int size = static_cast<int>(this->mReplays.size());
    this->mPaths = new const char*[size];
    for (int i = 0; i < size; ++i)
    {
        this->mPaths[i] = this->mReplays[i].c_str();
    }
    ::GuiListViewEx(listRect,
                    this->mPaths,
                    size,
                    &this->mScrollIndex,
                    &this->mActive,
                    &this->mFocus);

    if (size <= 0)
        ::GuiLabel(noReplaysPos, ReplaysMenu::cNoReplaysText);
    delete[] this->mPaths;
}

void Cyrey::ReplaysMenu::RefreshReplayList()
{
    // TODO: Make this async

#ifdef PLATFORM_ANDROID
    const char* directory = ::TextFormat("%s/%s", ::GetAndroidApp()->activity->internalDataPath, Replay::cReplaysFolderName);
#else
    const char *directory = Replay::cReplaysFolderName;
#endif
    if (!::DirectoryExists(directory))
        std::filesystem::create_directory(directory);

    this->mReplays.clear();
    for (const auto& entry : std::filesystem::directory_iterator(directory))
    {
        if (entry.path().extension() == Replay::cReplayFileExtension)
            this->mReplays.emplace_front(entry.path().filename().generic_string());
    }

    // reset the window's parameters, protect against out of bounds errors as we just changed the amount of list entries
    this->mScrollIndex = 0;
    this->mActive = -1;
    this->mFocus = -1;
}
