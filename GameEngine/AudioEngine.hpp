#pragma once

#include <fmod/fmod.hpp>
#include <vector>
#include <string>

#define AUDIO_ID int


class cGameObject;

static class AudioEngine
{
private:
	class Sound;

	FMOD::System* system;
	FMOD_RESULT status;
	std::vector<Sound*> Sounds;
	std::vector<std::string> SoundNames;
	std::vector<AUDIO_ID> Sounds_3d;

	struct AudioListener
	{
		FMOD_VECTOR location;
		FMOD_VECTOR look_at;
	} listener;

	AudioEngine()
	{
		system = 0; status = FMOD_OK;
	}

public:

	class Sound
	{
	private:
		friend class AudioEngine;

		struct Audio3d
		{
			cGameObject* _attached;
			FMOD_VECTOR _pos;
			FMOD_VECTOR _velocity;

		} _details_3d;

		FMOD::Sound* _sound;
		FMOD::Channel* _channel;
		FMOD::ChannelGroup* _channelGroup;
		float _volume = 1.f;
		float _pitch = 1.f;
		float _pan = 0.f;
		float _frequency = 0.f;
		bool _paused = true;
		bool _streamed = false;

		Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init);
		Sound(FMOD::Channel* _channel_init, FMOD::Sound* _sound_init, cGameObject* attach_to);
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

		bool update_3d();
	};

	static AudioEngine* Instance()
	{
		static AudioEngine instance;
		return &instance;
	}

	~AudioEngine();

	void Init();

	std::size_t Num_Sounds() { return this->SoundNames.size(); }
	std::string Get_Name(int i) { return this->SoundNames[i]; }

	AUDIO_ID Create_Sound(std::string filename, std::string friendlyName, bool streamed = false);
	AUDIO_ID Create_Sound_3d(std::string filename, std::string friendlyName, cGameObject* attach_to);
	Sound* Create_Sound(std::string filename, bool streamed = false);
	void PlaySound(Sound* sound);
	void PlaySound(AUDIO_ID sound_id);
	void PlaySound(std::string friendlyName);
	Sound* GetSound(AUDIO_ID sound_id);
	Sound* GetSound(std::string friendlyName);

	void Update3d();

};
