
#include "cAudioInputHandler.h"
#include "Scene.h"
#include "AudioEngine.hpp"
#include <sstream>

AUDIO_ID current_sound_id = 0;

static std::string get_sound_details(AudioEngine::Sound* sound, std::string friendlyName)
{
	std::ostringstream ss;

	ss << '[' << sound->get_name() << "] ";
	unsigned int seconds = sound->get_position() / 1000;
	unsigned int minutes = seconds / 60;
	unsigned int sec = seconds - (minutes * 60);
	unsigned int size = sound->get_size() / 1000;
	unsigned int size_minutes = size / 60;
	unsigned int size_sec = size - (size_minutes * 60);
	ss << "format(" << sound->get_format() << ") ";
	ss << "type(" << sound->get_type() << ") ";
	ss << "[" << minutes << ":" << sec << " / " << size_minutes << ":" << size_sec << "] ";
	ss << "Vol(" << sound->get_volume() << ") ";
	ss << "Fq(" << sound->get_frequency() << ") ";
	ss << "Pan(" << sound->get_pan() << ") ";

	return ss.str();
}

void cAudioInputHandler::HandleInput(GLFWwindow* window)
{

	if (_scene.pAudioEngine && _scene.pAudioEngine->Num_Sounds() > 0)
	{
		AudioEngine::Sound* current_sound = _scene.pAudioEngine->GetSound(current_sound_id);
		if (glfwGetKey(window, GLFW_KEY_1))
		{
			current_sound->set_volume(current_sound->get_volume() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_2))
		{
			current_sound->set_volume(current_sound->get_volume() + 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_3))
		{
			current_sound->set_pitch(current_sound->get_pitch() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_4))
		{
			current_sound->set_pitch(current_sound->get_pitch() + 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_5))
		{
			current_sound->set_pan(current_sound->get_pan() - 0.01f);
		}
		if (glfwGetKey(window, GLFW_KEY_6))
		{
			current_sound->set_pan(current_sound->get_pan() + 0.01f);
		}

		std::string friendlyName = _scene.pAudioEngine->Get_Name(current_sound_id);
		glfwSetWindowTitle(window, get_sound_details(_scene.pAudioEngine->GetSound(current_sound_id), friendlyName).c_str());

	}

}


void cAudioInputHandler::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

	if (_scene.pAudioEngine && _scene.pAudioEngine->Num_Sounds() > 0)
	{
		int max = _scene.pAudioEngine->Num_Sounds();

		if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
		{
			current_sound_id++;
			if (current_sound_id >= max) current_sound_id = 0;
		}

	}

}