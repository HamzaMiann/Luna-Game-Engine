#include "AudioEngine.hpp"
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#undef PlaySound

#define NUM_CHANNELS 32


void exit_on_failure(FMOD_RESULT status)
{
	if (status != FMOD_OK)
	{
		std::cout << "FMOD ERROR: " << status << std::endl;
		exit(1);
	}
}

void AudioEngine::Init()
{
	status = FMOD::System_Create(&system);
	exit_on_failure(status);

	status = system->init(NUM_CHANNELS, FMOD_INIT_NORMAL, 0);
	exit_on_failure(status);
}

AudioEngine::~AudioEngine()
{
	// clear all sounds (destructor will handle the memory cleanup)
	for (size_t i = 0; i < Sounds.size(); ++i)
	{
		delete Sounds[i];
	}
	Sounds.clear();
	SoundNames.clear();

	if (system)
	{
		// close the system
		status = system->close();
		exit_on_failure(status);

		// release the system
		status = system->release();
		exit_on_failure(status);
	}
}

AUDIO_ID AudioEngine::Create_Sound(std::string filename, std::string friendlyName)
{
	std::string fullPath = "assets\\audio\\" + filename;
	// create a sound using our system object
	FMOD::Sound* sound = 0;
	FMOD::Channel* channel = 0;
	status = system->createSound(fullPath.c_str(), FMOD_CREATESTREAM, 0, &sound);
	exit_on_failure(status);
	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, sound);
	Sounds.push_back(newSound);
	SoundNames.push_back(friendlyName);
	return this->Sounds.size() - 1;
}

AudioEngine::Sound::Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init)
{
	this->_channel = _channel_init;
	this->_sound = _sound_init;
	this->_isPlaying = false;
}

AudioEngine::Sound::~Sound()
{
	if (_sound)
	{
		FMOD_RESULT status = _sound->release();
		exit_on_failure(status);
	}
}

void AudioEngine::PlaySound(Sound* sound)
{
	sound->_isPlaying = true;
	FMOD_RESULT status = system->playSound(sound->_sound, FMOD_DEFAULT, false, &(sound->_channel));
	exit_on_failure(status);
}

void AudioEngine::PlaySound(int sound_id)
{
	AudioEngine::Sound* sound = Sounds[sound_id];
	sound->_isPlaying = true;
	FMOD_RESULT status = system->playSound(sound->_sound, FMOD_DEFAULT, false, &(sound->_channel));
	exit_on_failure(status);
}

void AudioEngine::PlaySound(std::string friendlyName)
{
	for (int i = 0; i < SoundNames.size(); ++i)
	{
		if (SoundNames[i] == friendlyName)
		{
			AudioEngine::Sound* sound = Sounds[i];
			sound->_isPlaying = true;
			FMOD_RESULT status = system->playSound(sound->_sound, FMOD_DEFAULT, false, &(sound->_channel));
			exit_on_failure(status);
		}
	}
}
