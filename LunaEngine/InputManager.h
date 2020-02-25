#pragma once
#include <_GL/GLCommon.h>
#include <vector>

class Input
{
	struct Scroll
	{
		double xoffset = 0.f;
		double yoffset = 0.f;
	};

	static Scroll scroll;

	static std::vector<int> keys_released;
	static std::vector<int> keys_pressed;
	friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

public:
	~Input() {}

	static bool KeyDown(int key);
	static bool KeyUp(int key);
	static bool GetKey(int key);

	static void LockCursor();
	static void UnlockCursor();
	static void CursorPosition(double& x, double& y);
	static bool GetMouseButton(int button);
	static float GetScrollX();
	static float GetScrollY();

	static void ClearBuffer();
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void error_callback(int error, const char* description);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void HandleInput(GLFWwindow* window);