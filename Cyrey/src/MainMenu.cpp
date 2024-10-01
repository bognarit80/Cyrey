#include "MainMenu.hpp"
#include "raygui.h"
#include "raylib.h"
#ifdef PLATFORM_DESKTOP
#include "raylib_win32.h"
#endif
#include "cpr/cpr.h"

static std::future<cpr::Response> futureConfig;

static void FetchGameConfig()
{
	if (futureConfig.valid())
		return; // we are already trying to fetch the config

	futureConfig = std::async(std::launch::async, []
	{
		return cpr::Get(cpr::Url{Cyrey::GameConfig::cLatestConfigUrl},
			cpr::Timeout{10000},
			cpr::ConnectTimeout{500});
	});
}

Cyrey::MainMenu::MainMenu(CyreyApp& app) : mApp(app)
{
	FetchGameConfig();
}

void Cyrey::MainMenu::Update()
{
	if (!futureConfig.valid())
		return;
	if (futureConfig.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready)
		return;
	auto resp = futureConfig.get();
	if (resp.status_code != 200)
		return;

	if (auto cfg = GameConfig::ParseConfig(resp.text); cfg.has_value())
		this->mApp.mGameConfig = *cfg;
}

void Cyrey::MainMenu::Draw()
{
	auto appWidth = static_cast<float>(this->mApp.mWidth);
	auto appHeight = static_cast<float>(this->mApp.mHeight);
	// Controls below scale with appWidth if it's lower than appHeight, the case for mobile phones

	// draw title
	::GuiSetStyle(::GuiControl::LABEL, ::GuiControlProperty::TEXT_ALIGNMENT, GuiTextAlignment::TEXT_ALIGN_CENTER);
	::GuiSetStyle(::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.2f : appHeight * 0.2f)
	);
	auto fontSize = static_cast<float>(::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE));

	::GuiLabel(
		::Rectangle{ 0, static_cast<float>(this->mApp.mHeight) * 0.05f,
			static_cast<float>(this->mApp.mWidth), fontSize },
		MainMenu::cTitleName
	);

	// draw current user text
	::GuiSetStyle(::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.05f : appHeight * 0.05f)
	);
	fontSize = static_cast<float>(::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE));
	if (::GuiLabelButton(
		::Rectangle{ 0, static_cast<float>(this->mApp.mHeight) * 0.30f,
			static_cast<float>(this->mApp.mWidth), fontSize },
		::TextFormat("Welcome back, %s.", this->mApp.mCurrentUser->mName.c_str())))
	{
		this->mIsUserPressed = true;
	}

	//draw buttons
	float buttonWidth = appHeight > appWidth ? appWidth * 0.50f : appHeight * 0.50f;
	::GuiSetStyle(
		::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.125f : appHeight * 0.125f)
	);
	fontSize = static_cast<float>(::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE));

	if (futureConfig.valid())
		::GuiDisable();
	this->mIsPlayBtnPressed = ::GuiButton(
		::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.45f - fontSize / 2,
			buttonWidth,
			fontSize * 1.05f
		},
		MainMenu::cPlayBtnText
	);

    if (::GuiButton(
            ::Rectangle{ appWidth / 2 - buttonWidth / 2,
                         appHeight * 0.59f - fontSize / 2,
                         buttonWidth,
                         fontSize * 1.05f
            },
            MainMenu::cReplaysBtnText
    ))
    {
        this->mApp.ChangeToState(CyreyAppState::ReplaysMenu);
    }
	::GuiEnable();

	if (::GuiButton(
		::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.73f - fontSize / 2,
			buttonWidth,
			fontSize * 1.05f
		},
		MainMenu::cSettingsBtnText
	))
	{
		this->mApp.ChangeToState(CyreyAppState::SettingsMenu);
	}

#ifndef __EMSCRIPTEN__ //no quit button on web version
	if (::GuiButton(
		::Rectangle{ appWidth / 2 - buttonWidth / 2,
			appHeight * 0.87f - fontSize / 2,
			buttonWidth,
			fontSize * 1.05f
		},
		MainMenu::cQuitBtnText
	))
	{
		this->mApp.mWantExit = true;
	}
#endif // __EMSCRIPTEN__

	::GuiSetStyle(::GuiControl::DEFAULT,
		::GuiDefaultProperty::TEXT_SIZE,
		static_cast<int>(appHeight > appWidth ? appWidth * 0.035f : appHeight * 0.035f)
	);
	fontSize = static_cast<float>(::GuiGetStyle(::GuiControl::DEFAULT, GuiDefaultProperty::TEXT_SIZE));
	const char* txt;
	if (futureConfig.valid())
		txt = MainMenu::cFetching;
	else if (this->mApp.mGameConfig.mVersion == Cyrey::cDefaultGameConfig.mVersion)
		txt = ::TextFormat(MainMenu::cLocalVersion, this->mApp.mGameConfig.mVersion);
	else
		txt = ::TextFormat(MainMenu::cFetchedVersion, this->mApp.mGameConfig.mVersion);
	if (::GuiLabelButton(
		::Rectangle{ 0,
			static_cast<float>(this->mApp.mHeight) * 0.94f,
			static_cast<float>(this->mApp.mWidth),
			fontSize },
		txt
	))
	{
		FetchGameConfig();
	}
}
