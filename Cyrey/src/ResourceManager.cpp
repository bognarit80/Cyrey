#include "ResourceManager.hpp"

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

	for (auto &kvp : this->mSounds)
	{
		::SetSoundVolume(kvp.second, soundVolume);
	}
	for (auto &kvp : this->mMusics)
	{
		::SetMusicVolume(kvp.second, musicVolume);
	}
}

constexpr std::string Cyrey::ResourceManager::GetFullPath(std::string fileName)
{
	return ResourceManager::cRootResorcesPath + fileName;
}

void Cyrey::ResourceManager::LoadMusic()
{
	for (auto &kvp : this->mMusics)
	{
		kvp.second = ::LoadMusicStream(ResourceManager::GetFullPath(kvp.first).c_str());
	}
}

void Cyrey::ResourceManager::LoadSounds()
{
	for (auto &kvp : this->mSounds)
	{
		kvp.second = ::LoadSound(ResourceManager::GetFullPath(kvp.first).c_str());
	}
}

void Cyrey::ResourceManager::UnloadMusic()
{
	for (auto &kvp : this->mMusics)
	{
		::UnloadMusicStream(kvp.second);
	}
}

void Cyrey::ResourceManager::UnloadSounds()
{
	for (auto &kvp : this->mSounds)
	{
		::UnloadSound(kvp.second);
	}
}
