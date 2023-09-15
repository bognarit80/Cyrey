#include "MainMenu.hpp"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raylib-cpp.hpp"

void Cyrey::MainMenu::Update()
{
	
	::GuiDrawText("Cyrey", 
		raylib::Rectangle{(float)::GetScreenWidth() / 2, (float)::GetScreenHeight() / 3, 100, 100}, 
		GuiTextAlignment::TEXT_ALIGN_CENTER, raylib::Color::SkyBlue());
	this->mIsPlayBtnPressed = ::GuiButton(raylib::Rectangle{ (float)::GetScreenWidth() / 2, (float)::GetScreenHeight() / 2, 100, 50 }, "Play");
}

void Cyrey::MainMenu::Draw() const
{
	return;
}
