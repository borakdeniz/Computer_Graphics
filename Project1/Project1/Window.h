#pragma once
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/include/GLFW/glfw3.h>
class Window
{
public:
	Window();

	Window(GLint windowWidth, GLint windowHeight);

	int Initialise();

	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }

	bool* getKeys() { return keys; }
	GLfloat getXChange();
	GLfloat getYChange();

	bool getShouldClose() { return glfwWindowShouldClose(MainWindow); }

	void swapBuffers() { glfwSwapBuffers(MainWindow); }

	~Window();

private:
	GLFWwindow* MainWindow;

	GLint width, height;
	GLint bufferWidth, bufferHeight;
	bool keys[1024]; //for getting input 1024 ascii den dolay� klavyedeki b�t�n sembolleri kapsas�n diye

	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	bool mouseFirstMove;
	

	void CreateCallBacks();
	static void HandleKeys(GLFWwindow * window, int key, int code, int action, int mode);
	static void HandleMouse(GLFWwindow* window, double xPos, double yPos);
};

