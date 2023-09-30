#include "SettingsMenu.hpp"
#include "raygui.h"

void Cyrey::SettingsMenu::Update()
{
}

void Cyrey::SettingsMenu::Draw()
{
	float appWidth = static_cast<float>(this->mApp.mWidth);
	float appHeight = static_cast<float>(this->mApp.mHeight);

	float windowY = appHeight * 0.1f;
	float windowHeight = appHeight - (windowY * 2);
	float windowX = appWidth > appHeight ? (appWidth - windowHeight) / 2 : appWidth * 0.1f;
	float windowWidth = windowY < windowX ? windowHeight : appWidth - (windowX * 2); //square if window is wide
	Vector2 windowAnchor = Vector2{ windowX, windowY };
	Rectangle windowRect = Rectangle{ windowX, windowY, windowWidth, windowHeight };

	float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
	float windowPaddingX = windowWidth * 0.45f;
	float windowPaddingY = windowHeight * 0.1f;
	float linePadding = windowWidth * 0.05f;
	float controlOffset = fontSize * 1.3f;
	float sliderWidth = windowWidth - (windowPaddingX * 1.3f);

	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);

	Rectangle musicSliderPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY,
		sliderWidth, 
		fontSize 
	};
	Rectangle soundSliderPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + controlOffset,
		sliderWidth,
		fontSize 
	};
	Rectangle fullscreenCheckPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 2),
		fontSize,
		fontSize 
	};
	Rectangle vsyncCheckPos = Rectangle{ windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 2),
		fontSize,
		fontSize
	};
	Rectangle linePos = Rectangle{ windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 3),
		windowWidth - (linePadding * 2),
		fontSize
	};
	Rectangle swapDeadZoneSliderPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 4),
		sliderWidth,
		fontSize 
	};
	Rectangle queueSwapToleranceSliderPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 5),
		sliderWidth,
		fontSize
	};
	Rectangle swerveCheckPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 6),
		fontSize,
		fontSize
	};
	Rectangle defaultsBtnPos = Rectangle{ windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 7),
		sliderWidth,
		fontSize
	};
	Rectangle mainMenuBtnPos = Rectangle{ windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 7),
		sliderWidth * 0.9f,
		fontSize
	};

	if (::GuiWindowBox(windowRect, cWindowText))
		this->mApp.ChangeToState(this->mApp.mPrevState);

	::GuiSlider(musicSliderPos, cMusicSliderText, ::TextFormat("%d", static_cast<int>(this->mMusicVolume * 100)), &this->mMusicVolume, 0, 1);
	::GuiSlider(soundSliderPos, cSoundSliderText, ::TextFormat("%d", static_cast<int>(this->mSoundVolume * 100)), &this->mSoundVolume, 0, 1);

#ifndef __EMSCRIPTEN__ //TODO: Implement proper fullscreen on Web
	bool isFullscreen = ::IsWindowFullscreen();
	::GuiCheckBox(fullscreenCheckPos, SettingsMenu::cFullscreenCheckText, &isFullscreen);
	if (isFullscreen != this->mIsFullscreen)
	{
		this->mApp.ToggleFullscreen();
		this->mIsFullscreen ^= 1;
	}

	// enable once you figure this out
	bool isVsync = ::IsWindowState(::ConfigFlags::FLAG_VSYNC_HINT);
	::GuiCheckBox(vsyncCheckPos, SettingsMenu::cVsyncCheckText, &isVsync);
	if (isVsync != this->mIsVSync)
	{
		this->mIsVSync = isVsync;
		this->mApp.InitWindow(); // We need to reopen the window to apply VSync
	}
#endif // __EMSCRIPTEN__
	
	::GuiLine(linePos, NULL);

	::GuiSlider(swapDeadZoneSliderPos, 
		cSwapDeadZoneSliderText, 
		::TextFormat("%d%%", static_cast<int>(this->mSwapDeadZone * 100)), 
		&this->mSwapDeadZone, 0.1f, 1.0f);

	::GuiSlider(queueSwapToleranceSliderPos,
		cQueueSwapToleranceSliderText,
		this->mQueueSwapTolerance > 0.0f ?
		this->mQueueSwapTolerance < 1.0f ? ::TextFormat("%.2fs", this->mQueueSwapTolerance) : 
		SettingsMenu::cInf :
		SettingsMenu::cOff,
		&this->mQueueSwapTolerance, 0, 1);

	::GuiCheckBox(swerveCheckPos, cBoardSwerveCheckText, &this->mWantBoardSwerve);

	if (::GuiButton(defaultsBtnPos, cDefaultsButtonText))
	{
		this->mSwapDeadZone = SettingsMenu::cSwapDeadZone;
		this->mQueueSwapTolerance = SettingsMenu::cQueueSwapTolerance;
		this->mWantBoardSwerve = SettingsMenu::cQueueSwapTolerance;
	}

	if (this->mApp.mPrevState == CyreyAppState::InGame)
	{
		if (::GuiButton(mainMenuBtnPos, cMainMenuButtonText))
		{
			//TODO: Save the game, or add XP or something
			this->mApp.ChangeToState(CyreyAppState::MainMenu);
		}
	}
}
