#include "MainMenu.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"

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
	float titleWidth = appHeight > appWidth ? appWidth * 0.75f : appHeight * 0.75f;
	::GuiSetStyle(
		::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.2f : appHeight * 0.2f)
	);
	int fontSize = ::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE);

	::GuiDrawText(MainMenu::cTitleName,
		raylib::Rectangle{ (appWidth / 2) - titleWidth / 2,
			appHeight * 0.125f,
			titleWidth,
			static_cast<float>(fontSize)
		},
		::GuiTextAlignment::TEXT_ALIGN_CENTER,
		raylib::Color::SkyBlue()
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
		raylib::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.50f - fontSize / 2,
			buttonWidth,
			fontSize * 1.1f
		},
		MainMenu::cPlayBtnText
	);

	::GuiButton(
		raylib::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.65f - fontSize / 2,
			buttonWidth,
			fontSize * 1.1f
		},
		MainMenu::cSettingsBtnText
	);

#ifndef __EMSCRIPTEN__ //no quit button on web version
	if (::GuiButton(
		raylib::Rectangle{ appWidth / 2 - buttonWidth / 2,
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
