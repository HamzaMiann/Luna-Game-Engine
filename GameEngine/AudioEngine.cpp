#include "AudioEngine.hpp"
#include "fmod_helper.h"
#undef PlaySound

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

	status = system->set3DSettings(1.0f, 1.0f, 1.0f);
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


AUDIO_ID AudioEngine::Create_Sound(std::string filename, std::string friendlyName, bool streamed)
{
	std::string fullPath = "assets\\audio\\" + filename;
	FMOD::Sound* sound = 0;
	FMOD::Channel* channel = 0;
	status = system->createSound(fullPath.c_str(), streamed? FMOD_CREATESTREAM : FMOD_DEFAULT, 0, &sound);
	exit_on_failure(status);
	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, sound);
	status = system->playSound(newSound->_sound, FMOD_DEFAULT, true, &(newSound->_channel));
	newSound->_paused = true;
	newSound->_streamed = streamed;
	exit_on_failure(status);
	Sounds.push_back(newSound);
	SoundNames.push_back(friendlyName);
	return this->Sounds.size() - 1;
}


AUDIO_ID AudioEngine::Create_Sound_3d(std::string filename, std::string friendlyName, cGameObject* attach_to)
{
	std::string fullPath = "assets\\audio\\" + filename;
	FMOD::Sound* sound = 0;
	FMOD::Channel* channel = 0;
	status = system->createSound(fullPath.c_str(), FMOD_3D, 0, &sound);
	exit_on_failure(status);
	AudioEngine::Sound* newSound = new AudioEngine::Sound(channel, sound, attach_to);
	status = system->playSound(newSound->_sound, FMOD_DEFAULT, true, &(newSound->_channel));
	newSound->_paused = true;
	newSound->_streamed = false;
	exit_on_failure(status);
	Sounds.push_back(newSound);
	SoundNames.push_back(friendlyName);
	this->Sounds_3d.push_back(this->Sounds.size() - 1);
	return this->Sounds.size() - 1;
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


void AudioEngine::Update3d()
{
	status = system->set3DListenerAttributes(0, &_al->position, &_al->velocity, &_al->forward, &_al->up);
	exit_on_failure(status);
	for (unsigned int i = 0; i < Sounds_3d.size(); ++i)
	{
		this->Sounds[Sounds_3d[i]]->update_3d();
	}
}