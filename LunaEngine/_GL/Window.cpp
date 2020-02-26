#include <_GL/Window.h>
#include <stdlib.h>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 720

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