// 2-ClearScreen.cpp - use OpenGL shader architecture

#include <glad.h>											// GL header file
#include <glfw3.h>											// GL toolkit
#include <stdio.h>											// printf, etc.
#include "GLXtras.h"										// convenience routines


void GlfwError(int id, const char* reason) {
	printf("GFLW error %i: %s\n", id, reason);
	getchar();
}


void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}




int main() {
	// application entry
	glfwSetErrorCallback(GlfwError);						// init GL framework

	if (!glfwInit())
		return 1;

	// create named window of given size
	GLFWwindow* w = glfwCreateWindow(800, 600, "Clear to Green", NULL, NULL);
	if (!w)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(w);

	//GLAD manages function pointers for OpenGL so we want to initialize GLAD before we call any OpenGL function:
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);	// set OpenGL extensions
	

	while (!glfwWindowShouldClose(w)) {						// event loop
		processInput(w);
		glfwSwapBuffers(w);									// double-buffer is default
		glfwPollEvents();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}


	glfwDestroyWindow(w);
	glfwTerminate();
}
