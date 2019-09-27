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
	status = system->createSound(fullPath.c_str(), FMOD_DEFAULT /*FMOD_CREATESTREAM*/, 0, &sound);
	exit_on_failure(status);
	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, sound);
	status = system->playSound(newSound->_sound, FMOD_DEFAULT, true, &(newSound->_channel));
	newSound->_paused = true;
	exit_on_failure(status);
	Sounds.push_back(newSound);
	SoundNames.push_back(friendlyName);
	return this->Sounds.size() - 1;
}

AudioEngine::Sound::Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init)
{
	this->_channel = _channel_init;
	this->_sound = _sound_init;
	this->_paused = true;
	set_pitch(this->_pitch);
	set_volume(this->_volume);
	set_pan(this->_pan);
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
	if (sound->_channel)
	{
		sound->reset_position();
		status = sound->_channel->getPaused(&sound->_paused);
		exit_on_failure(status);
		if (sound->_paused)
		{
			status = sound->_channel->setPaused(!sound->_paused);
			exit_on_failure(status);
			sound->_paused = false;
		}
	}
}

void AudioEngine::PlaySound(int sound_id)
{
	AudioEngine::Sound* sound = Sounds[sound_id];
	if (sound->_channel)
	{
		sound->reset_position();
		status = sound->_channel->getPaused(&sound->_paused);
		exit_on_failure(status);
		if (sound->_paused)
		{
			status = sound->_channel->setPaused(!sound->_paused);
			exit_on_failure(status);
			sound->_paused = false;
		}
	}
}

void AudioEngine::PlaySound(std::string friendlyName)
{
	for (int i = 0; i < SoundNames.size(); ++i)
	{
		if (SoundNames[i] == friendlyName)
		{
			AudioEngine::Sound* sound = Sounds[i];
			if (sound->_channel)
			{
				sound->reset_position();
				status = sound->_channel->getPaused(&sound->_paused);
				exit_on_failure(status);
				if (sound->_paused)
				{
					status = sound->_channel->setPaused(!sound->_paused);
					exit_on_failure(status);
					sound->_paused = false;
				}
			}
		}
	}
}

AudioEngine::Sound* AudioEngine::GetSound(AUDIO_ID sound_id)
{
	if (sound_id < 0 && sound_id >= this->Sounds.size()) return nullptr;
	return this->Sounds[sound_id];
}

AudioEngine::Sound* AudioEngine::GetSound(std::string friendlyName)
{
	for (int i = 0; i < SoundNames.size(); ++i)
	{
		if (SoundNames[i] == friendlyName)
			return Sounds[i];
	}
	return nullptr;
}


float AudioEngine::Sound::get_volume()
{
	return _volume;
}
bool AudioEngine::Sound::set_volume(float volume)
{
	if (volume <= 1.0f && volume > 0.f && this->_channel)
	{
		FMOD_RESULT status = _channel->setVolume(volume);
		exit_on_failure(status);
		_volume = volume;
		return true;
	}
	return false;
}


float AudioEngine::Sound::get_pitch()
{
	return _pitch;
}
bool AudioEngine::Sound::set_pitch(float pitch)
{
	if (pitch <= 2.0f && pitch > 0.01f && this->_channel)
	{
		FMOD_RESULT status = _channel->setPitch(pitch);
		exit_on_failure(status);
		_pitch = pitch;
		return true;
	}
	return false;
}

float AudioEngine::Sound::get_pan()
{
	return _pan;
}
bool AudioEngine::Sound::set_pan(float pan)
{
	if (pan <= 1.0f && pan >= -1.f && this->_channel)
	{
		FMOD_RESULT status = _channel->setPan(pan);
		exit_on_failure(status);
		_pan = pan;
		return true;
	}
	return false;
}

float AudioEngine::Sound::get_frequency()
{
	if (this->_channel)
	{
		FMOD_RESULT status = _channel->getFrequency(&_frequency);
		exit_on_failure(status);
	}
	return _frequency;
}
bool AudioEngine::Sound::set_frequency(float frequency)
{
	if (frequency <= 1.0f && frequency >= -1.f && this->_channel)
	{
		FMOD_RESULT status = _channel->setPan(frequency);
		exit_on_failure(status);
		_frequency = frequency;
		return true;
	}
	return false;
}

unsigned int AudioEngine::Sound::get_position()
{
	unsigned int position = 0.f;
	if (this->_channel)
	{
		FMOD_RESULT status = _channel->getPosition(&position, FMOD_TIMEUNIT_MS);
		exit_on_failure(status);
	}
	return position;
}

bool AudioEngine::Sound::reset_position()
{
	if (this->_channel)
	{
		FMOD_RESULT status = _channel->setPosition(0, FMOD_TIMEUNIT_MS);
		exit_on_failure(status);
		return true;
	}
	return false;
}