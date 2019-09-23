#pragma once

#include <fmod/fmod.hpp>
#include <vector>
#include <string>

#define AUDIO_ID int

class AudioEngine
{
	class Sound;

private:

	FMOD::System* system;
	FMOD_RESULT status;
	std::vector<Sound*> Sounds;
	std::vector<std::string> SoundNames;

	AudioEngine() {}

public:

	static AudioEngine& Instance()
	{
		static AudioEngine engine;
		return engine;
	}

	class Sound
	{
	private:
		friend class AudioEngine;
		FMOD::Sound* _sound;
		FMOD::Channel* _channel;
		bool _isPlaying;

		Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init);
	public:
		~Sound();
	};


	void Init();
	~AudioEngine();

	AUDIO_ID Create_Sound(std::string filename, std::string friendlyName);
	void PlaySound(Sound* sound);
	void PlaySound(int sound_id);
	void PlaySound(std::string friendlyName);
	
};