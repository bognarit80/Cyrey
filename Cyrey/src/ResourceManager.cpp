#include "ResourceManager.hpp"
#include <ranges>

bool Cyrey::ResourceManager::HasFinishedLoading() const
{
	return this->mHasFinishedLoading;
}

void Cyrey::ResourceManager::LoadResources()
{
	this->LoadMusic();
	this->LoadSounds();
	this->mHasFinishedLoading = true;
}

void Cyrey::ResourceManager::UnloadResources()
{
	this->UnloadMusic();
	this->UnloadSounds();
	::CloseAudioDevice();
	this->mHasFinishedLoading = false;
}

void Cyrey::ResourceManager::SetVolume(float soundVolume, float musicVolume)
{
	if (!this->mHasFinishedLoading)
		return;

	for (auto& sound : this->mSounds | std::views::values)
	{
		::SetSoundVolume(sound, soundVolume);
	}
	for (auto& music : this->mMusics | std::views::values)
	{
		::SetMusicVolume(music, musicVolume);
	}
}

constexpr std::string Cyrey::ResourceManager::GetFullPath(const std::string& fileName)
{
	return ResourceManager::cRootResorcesPath + fileName;
}

void Cyrey::ResourceManager::LoadMusic()
{
	for (auto& [name, music] : this->mMusics)
	{
		music = ::LoadMusicStream(ResourceManager::GetFullPath(name).c_str());
	}
}

void Cyrey::ResourceManager::LoadSounds()
{
	for (auto& [name, sound] : this->mSounds)
	{
		sound = ::LoadSound(ResourceManager::GetFullPath(name).c_str());
	}
}

void Cyrey::ResourceManager::UnloadMusic()
{
	for (auto& val : this->mMusics | std::views::values)
	{
		::UnloadMusicStream(val);
	}
}

void Cyrey::ResourceManager::UnloadSounds()
{
	for (auto& val : this->mSounds | std::views::values)
	{
		::UnloadSound(val);
	}
}
