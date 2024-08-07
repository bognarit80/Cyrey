#include "SettingsMenu.hpp"
#include "raygui.h"

void Cyrey::SettingsMenu::Update()
{
}

void Cyrey::SettingsMenu::Draw()
{
	auto appWidth = static_cast<float>(this->mApp.mWidth);
	auto appHeight = static_cast<float>(this->mApp.mHeight);

	float windowY = appHeight * 0.1f;
	float windowHeight = appHeight - (windowY * 2);
	float windowX = appWidth > appHeight ? (appWidth - windowHeight) / 2 : appWidth * 0.1f;
	float windowWidth = windowY < windowX ? windowHeight : appWidth - (windowX * 2); //square if window is wide
	Vector2 windowAnchor = { windowX, windowY };
	Rectangle windowRect = { windowX, windowY, windowWidth, windowHeight };

	float fontSize = windowHeight > windowWidth ? windowWidth / 20 : windowHeight / 20;
	float windowPaddingX = windowWidth * 0.45f;
	float windowPaddingY = windowHeight * 0.1f;
	float linePadding = windowWidth * 0.05f;
	float controlOffset = fontSize * 1.3f;
	float sliderWidth = windowWidth - (windowPaddingX * 1.3f);

	::GuiSetStyle(::GuiControl::DEFAULT, ::GuiDefaultProperty::TEXT_SIZE, fontSize);
	::GuiSetIconScale(fontSize / 16);

	Rectangle musicSliderPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY,
		sliderWidth, 
		fontSize 
	};
	Rectangle soundSliderPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + controlOffset,
		sliderWidth,
		fontSize 
	};
	Rectangle fullscreenCheckPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 2),
		fontSize,
		fontSize 
	};
	Rectangle vsyncCheckPos = { windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 2),
		fontSize,
		fontSize
	};
	Rectangle firstLinePos = { windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 3),
		windowWidth - (linePadding * 2),
		fontSize
	};
	Rectangle swapDeadZoneSliderPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 4),
		sliderWidth,
		fontSize 
	};
	Rectangle queueSwapToleranceSliderPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 5),
		sliderWidth,
		fontSize
	};
	Rectangle swerveCheckPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 6),
		fontSize,
		fontSize
	};
    Rectangle autoSaveCheckPos = { windowAnchor.x + windowPaddingX,
    	windowAnchor.y + windowPaddingY + (controlOffset * 7),
    	fontSize,
    	fontSize
    };
	Rectangle defaultsBtnPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 8),
		sliderWidth,
		fontSize
	};
	Rectangle secondLinePos = { windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 9),
		windowWidth - (linePadding * 2),
		fontSize
	};
	Rectangle mainMenuBtnPos = { windowAnchor.x + linePadding,
		windowAnchor.y + windowPaddingY + (controlOffset * 10),
		sliderWidth * 0.9f,
		fontSize
	};
	Rectangle doneBtnPos = { windowAnchor.x + windowPaddingX,
		windowAnchor.y + windowPaddingY + (controlOffset * 10),
		sliderWidth * 0.9f,
		fontSize
	};

	if (::GuiWindowBox(windowRect, SettingsMenu::cWindowText) || 
		::GuiButton(doneBtnPos, ::GuiIconText(::GuiIconName::ICON_OK_TICK, SettingsMenu::cDoneButtonText)))
	{
		this->SaveSettingsFile(SettingsMenu::cSettingsFileName);
		this->mApp.ChangeToState(this->mApp.mPrevState);
	}

	::GuiSlider(musicSliderPos, 
		::GuiIconText(::GuiIconName::ICON_AUDIO, SettingsMenu::cMusicSliderText), 
		::TextFormat("%d", static_cast<int>(this->mMusicVolume * 100)), 
		&this->mMusicVolume, 0, 1);

	::GuiSlider(soundSliderPos, 
		::GuiIconText(::GuiIconName::ICON_FX, SettingsMenu::cSoundSliderText), 
		::TextFormat("%d", static_cast<int>(this->mSoundVolume * 100)), 
		&this->mSoundVolume, 0, 1);

#ifndef __EMSCRIPTEN__ // TODO: Implement proper fullscreen on Web
	bool isFullscreen = ::IsWindowFullscreen();
	::GuiCheckBox(fullscreenCheckPos, SettingsMenu::cFullscreenCheckText, &isFullscreen);
	if (isFullscreen != this->mIsFullscreen)
	{
		this->mApp.ToggleFullscreen();
		this->mIsFullscreen ^= 1;
	}

	bool isVsync = ::IsWindowState(::ConfigFlags::FLAG_VSYNC_HINT);
	::GuiCheckBox(vsyncCheckPos, SettingsMenu::cVsyncCheckText, &isVsync);
	if (isVsync != this->mIsVSync)
	{
		this->mIsVSync = isVsync;
		this->mApp.ChangeToState(CyreyAppState::Loading);
		this->mApp.InitWindow(); // We need to reopen the window to apply VSync
	}
#endif // __EMSCRIPTEN__
	
	::GuiLine(firstLinePos, nullptr);

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
	::GuiCheckBox(autoSaveCheckPos, cReplayAutoSaveText, &this->mWantReplayAutoSave);

	if (::GuiButton(defaultsBtnPos, cDefaultsButtonText))
	{
		this->mSwapDeadZone = SettingsMenu::cSwapDeadZone;
		this->mQueueSwapTolerance = SettingsMenu::cQueueSwapTolerance;
		this->mWantBoardSwerve = SettingsMenu::cWantBoardSwerve;
		this->mWantReplayAutoSave = SettingsMenu::cWantReplayAutoSave;
	}

	::GuiLine(secondLinePos, nullptr);

	if (this->mApp.mPrevState == CyreyAppState::InGame)
		if (::GuiButton(mainMenuBtnPos, ::GuiIconText(::GuiIconName::ICON_EXIT, SettingsMenu::cMainMenuButtonText)))
		{
			// TODO: Save the game, or add XP or something
			this->SaveSettingsFile(SettingsMenu::cSettingsFileName);
			this->mApp.ChangeToState(CyreyAppState::MainMenu);
		}
}

void Cyrey::SettingsMenu::OpenSettingsFile(const std::string& path)
{
	auto data = ::LoadFileText(path.c_str());
	using json = nlohmann::ordered_json;

	if (!data || !json::accept(data))
		return;

	json j = json::parse(data);

	this->mMusicVolume = j.value("musicVolume", SettingsMenu::cMusicVolume);
	this->mSoundVolume = j.value("soundVolume", SettingsMenu::cSoundVolume);
	this->mIsFullscreen = j.value("wantFullscreen", SettingsMenu::cWantFullscreen);
	this->mIsVSync = j.value("wantVSync", SettingsMenu::cWantVSync);
	this->mSwapDeadZone = j.value("swapDeadZone", SettingsMenu::cSwapDeadZone);
	this->mWantBoardSwerve = j.value("wantBoardSwerve", SettingsMenu::cWantBoardSwerve);
	this->mQueueSwapTolerance = j.value("queueSwapTolerance", SettingsMenu::cQueueSwapTolerance);
	this->mWantReplayAutoSave = j.value("wantReplayAutoSave", SettingsMenu::cWantReplayAutoSave);
}

bool Cyrey::SettingsMenu::SaveSettingsFile(const std::string& path)
{
	using json = nlohmann::ordered_json;

	json j;
	j["musicVolume"] = this->mMusicVolume;
	j["soundVolume"] = this->mSoundVolume;
	j["wantFullscreen"] = this->mIsFullscreen;
	j["wantVSync"] = this->mIsVSync;
	j["swapDeadZone"] = this->mSwapDeadZone;
	j["wantBoardSwerve"] = this->mWantBoardSwerve;
	j["queueSwapTolerance"] = this->mQueueSwapTolerance;
	j["wantReplayAutoSave"] = this->mWantReplayAutoSave;

	return ::SaveFileText(path.c_str(), j.dump().data());
}
