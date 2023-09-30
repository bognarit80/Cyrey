#include "MainMenu.hpp"
#include "raygui.h"
#include "raylib.h"

void Cyrey::MainMenu::Init()
{
	this->mIsPlayBtnPressed = false;
}

void Cyrey::MainMenu::Update()
{
	
}

void Cyrey::MainMenu::Draw()
{
	float appWidth = static_cast<float>(this->mApp.mWidth);
	float appHeight = static_cast<float>(this->mApp.mHeight);
	//Controls below scale with appWidth if it's lower than appHeight, the case for mobile phones

	//draw title
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, GuiTextAlignment::TEXT_ALIGN_CENTER);
	::GuiSetStyle(::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.2f : appHeight * 0.2f)
	);
	int fontSize = ::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE);

	::GuiLabel(
		::Rectangle{ 0, this->mApp.mHeight * 0.125f,
			static_cast<float>(this->mApp.mWidth), static_cast<float>(fontSize) },
		MainMenu::cTitleName
	);

	//draw buttons
	float buttonWidth = appHeight > appWidth ? appWidth * 0.50f : appHeight * 0.50f;
	::GuiSetStyle(
		::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.125f : appHeight * 0.125f)
	);
	fontSize = ::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE);

	this->mIsPlayBtnPressed = ::GuiButton(
		::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.50f - fontSize / 2,
			buttonWidth,
			fontSize * 1.1f
		},
		MainMenu::cPlayBtnText
	);

	if (::GuiButton(
		::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.65f - fontSize / 2,
			buttonWidth,
			fontSize * 1.1f
		},
		MainMenu::cSettingsBtnText
	))
	{
		this->mApp.ChangeToState(CyreyAppState::SettingsMenu);
	}

#ifndef __EMSCRIPTEN__ //no quit button on web version
	if (::GuiButton(
		::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.80f - fontSize / 2,
			buttonWidth,
			fontSize * 1.1f
		},
		MainMenu::cQuitBtnText
	))
	{
		this->mApp.mWantExit = true;
	}
#endif // __EMSCRIPTEN__
}
