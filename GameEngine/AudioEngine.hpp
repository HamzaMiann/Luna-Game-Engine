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


public:

	class Sound
	{
	private:
		friend class AudioEngine;
		FMOD::Sound* _sound;
		FMOD::Channel* _channel;
		float _volume = 1.f;
		float _pitch = 1.f;
		float _pan = 0.f;
		float _frequency = 0.f;
		bool _paused = true;
		bool _streamed = false;

		Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init);
	public:
		~Sound();

		float get_volume();
		bool set_volume(float volume);

		float get_pitch();
		bool set_pitch(float pitch);

		float get_pan();
		bool set_pan(float pan);

		float get_frequency();
		bool set_frequency(float frequency);

		unsigned int get_position();
		unsigned int get_size();
		bool reset_position();

		void toggle_pause();
		bool is_paused();

		bool is_streamed();

		std::string get_name();
		std::string get_format();
		std::string get_type();
	};


	AudioEngine() { system = 0; status = FMOD_OK; }
	~AudioEngine();

	void Init();

	std::size_t Num_Sounds() { return this->SoundNames.size(); }
	std::string Get_Name(int i) { return this->SoundNames[i]; }

	AUDIO_ID Create_Sound(std::string filename, std::string friendlyName, bool streamed = false);
	void PlaySound(Sound* sound);
	void PlaySound(AUDIO_ID sound_id);
	void PlaySound(std::string friendlyName);
	Sound* GetSound(AUDIO_ID sound_id);
	Sound* GetSound(std::string friendlyName);
	
};