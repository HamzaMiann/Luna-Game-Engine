#include <_GL/Window.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900

GLFWwindow* global::window = 0;

void SetErrorCallback(GLFWerrorfun callback)
{
	glfwSetErrorCallback(callback);
}

void SetKeyCallback(GLFWkeyfun callback)
{
	glfwSetKeyCallback(global::window, callback);
}

void SetScrollCallback(GLFWscrollfun callback)
{
	glfwSetScrollCallback(global::window, callback);
}

void SetDropCallback(GLFWdropfun callback)
{
	glfwSetDropCallback(global::window, callback);
}

void InitGL()
{
	if (!glfwInit())
	{
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	auto monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	// FOR FULL SCREEN WINDOWED
	/*glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
	global::window = glfwCreateWindow(mode->width, mode->height, "GLEngine", monitor, NULL);
	glfwSetWindowMonitor(global::window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);*/

	global::window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "GLEngine", NULL, NULL);
	

	if (!global::window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(global::window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwSwapInterval(1);
}