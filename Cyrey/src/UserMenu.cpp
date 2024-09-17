#include "UserMenu.hpp"

#include <raygui.h>

void Cyrey::UserMenu::Update()
{
}

void Cyrey::UserMenu::Draw()
{
    if (!this->mIsOpen)
        return;

    auto appWidth = static_cast<float>(this->mApp.mWidth);
    auto appHeight = static_cast<float>(this->mApp.mHeight);

    float windowY = appHeight * 0.1f;
    float windowHeight = appHeight - (windowY * 2);
    float windowX = appWidth > appHeight ? (appWidth - windowHeight) / 2 : appWidth * 0.1f;
    float windowWidth = windowY < windowX ? windowHeight : appWidth - (windowX * 2); // square if window is wide
    Vector2 windowAnchor = { windowX, windowY };
    Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

    float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
    ::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
    ::GuiSetIconScale(std::max(fontSize / 16, 1.0f));

    float windowPaddingY = windowHeight * 0.1f;
    float controlOffsetY = fontSize * 1.3f;
    float controlPaddingX = windowWidth * 0.05f;

    Rectangle piecesClearedPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 3),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle piecesClearedValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 3),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle gamesLabelPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 4),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle gamesValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 4),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle bombsLabelPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 5),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle bombsValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 5),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle lightningsLabelPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 6),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle lightningsValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 6),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle hypercubesLabelPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 7),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle hypercubesValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 7),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle bestMoveLabelPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 8),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle bestMoveValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 8),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle bestCascadeLabelPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 9),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    Rectangle bestCascadeValuePos = { windowAnchor.x + (windowWidth / 2),
        windowAnchor.y + windowPaddingY + (controlOffsetY * 9),
        (windowWidth / 2) - controlPaddingX,
        fontSize
    };
    // buttons
    Rectangle mainMenuBtnPos = { windowAnchor.x + controlPaddingX,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 12),
        (windowWidth / 2) - controlPaddingX * 1.5f,
        fontSize
    };
    Rectangle changeUserBtnPos = { windowAnchor.x + (windowWidth / 2) + controlPaddingX * 0.5f,
        windowAnchor.y + windowPaddingY + (controlOffsetY * 12),
        (windowWidth / 2) - controlPaddingX * 1.5f,
        fontSize
    };

    if (::GuiWindowBox(windowRect, "User profile") ||
        ::GuiButton(mainMenuBtnPos, ::GuiIconText(::GuiIconName::ICON_EXIT, "Main Menu")))
    {
        this->mIsOpen = false;
    }
    ::GuiButton(changeUserBtnPos, ::GuiIconText(::GuiIconName::ICON_PLAYER, "Select User"));

    // left side
    ::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_RIGHT);
    ::GuiLabel(piecesClearedPos, "Pieces cleared: ");
    ::GuiLabel(gamesLabelPos, "Total games played: ");
    ::GuiLabel(bombsLabelPos, "Total Bombs: ");
    ::GuiLabel(lightningsLabelPos, "Total Lightnings: ");
    ::GuiLabel(hypercubesLabelPos, "Total Hypercubes: ");
    ::GuiLabel(bestMoveLabelPos, "Best move (points): ");
    ::GuiLabel(bestCascadeLabelPos, "Highest cascade: ");

    // right side
    User& user = *this->mApp.mCurrentUser; // just a shorthand since this ref will be used frequently
    ::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_LEFT);
    ::GuiLabel(piecesClearedValuePos, ::TextFormat(" %lld", user.mPiecesCleared));
    ::GuiLabel(gamesValuePos, ::TextFormat(" %d", user.mGamesPlayed));
    ::GuiLabel(bombsValuePos, ::TextFormat(" %d", user.mBombsDetonated));
    ::GuiLabel(lightningsValuePos, ::TextFormat(" %d", user.mLightningsDetonated));
    ::GuiLabel(hypercubesValuePos, ::TextFormat(" %d", user.mHypercubesDetonated));
    ::GuiLabel(bestMoveValuePos, ::TextFormat(" %lld", user.mBestMovePoints));
    ::GuiLabel(bestCascadeValuePos, ::TextFormat(" %d", user.mBestMoveCascades));
#ifdef PLATFORM_ANDROID
    if (const char* txt = ::GetInputDialogText(); txt != nullptr && txt[0])
    {
        user.mName = txt;
        this->mApp.SaveCurrentUserData();
    }
#else
    if (this->mWantNameDialog)
    {
        static std::string str = user.mName;
        str.resize(User::cMaxNameLength + 1);
        int i = ::GuiTextInputBox(windowRect,
                              "Change name",
                              "Enter new name:",
                              "OK;Cancel",
                              str.data(), User::cMaxNameLength + 1, nullptr);

        if (i == 1 && str[0])
        {
            user.mName = str;
            this->mApp.SaveCurrentUserData();
            this->mWantNameDialog = false;
        }
        else if (i == 2)
            this->mWantNameDialog = false;
    }
#endif

    // top text
    float fontSizeTitle = windowHeight > windowWidth ? windowWidth / 12 : windowHeight / 12;
    ::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, static_cast<int>(fontSizeTitle));
    ::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, ::GuiTextAlignment::TEXT_ALIGN_CENTER);

    Rectangle nameLabel = { windowAnchor.x,
        windowAnchor.y + windowPaddingY,
        windowWidth,
        fontSizeTitle
    };
    Rectangle xpLabel = { windowAnchor.x,
        windowAnchor.y + windowPaddingY + controlOffsetY,
        windowWidth,
        fontSizeTitle
    };
    if (::GuiLabelButton(nameLabel,
        ::TextFormat("%s", user.mName.c_str())))
    {
#ifdef PLATFORM_ANDROID
        ::ShowInputDialog();
#else
        this->mWantNameDialog = true;
#endif
    }
    ::GuiLabel(xpLabel,
        ::TextFormat("%lld lifetime XP", user.mXP));
}