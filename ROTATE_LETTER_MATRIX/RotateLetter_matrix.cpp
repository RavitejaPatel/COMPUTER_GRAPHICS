﻿/*
* Author:Raviteja Poosala
* Seattle University
* Date:10/3/2021
* Computer Graphics
*/


// 4-ColorfulLetter.cpp: draw multiple triangles to form a colorful letter

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "GLXtras.h"
#include <vector>
#include <VecMat.h>

// shader program

GLuint vBuffer = 0; // GPU vertex buffer ID, valid if > 0
GLuint program = 0; // GLSL program ID, valid if > 0

time_t startTime = clock();
static float degPerSec = 30;

const char* vertexShader = R"(
	#version 130
	in vec2 point;
    in vec3 color;
    out vec4 vColor;
uniform mat4 m;
uniform float radAng = 0;
	
	//in vec3 color;
	//out vec4 vColor;
    uniform mat4 view;



	void main() {
		gl_Position = view*vec4(point, 0, 1);
		vColor = vec4(color, 1);
	}
)";

const char* pixelShader = R"(
	#version 130
	in vec4 vColor;
	out vec4 pColor;
	void main() {
		pColor = vColor;
	}
)";

// the letter T: 8 vertices, 8 colors, triangulation
float points[][2] = { {-.15f, .125f},{-.15f, -.75f}, {-.5f, -.75f}, {-.5f,   .75f}, {.17f, .75f}, {.38f, .575f},
						 {.38f,   .35f}, {.23f, .125f}, {.5f,  -.125f}, {.5f,  -.5f}, {.25f, -.75f} };
float colors[][3] = { {1, 1, 1}, {1, 0, 0}, {.5f, 0, 0}, {1, 1, 0}, {.5f, 1, 0},
						 {0, 1, 0}, {0, 1, 1}, {0,   0, 1}, {1, 0, 1}, {.5f, 0, .5f} };

//float colors[][3] = { {0,0,0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0}, {0, 0, 0},
	//					 {0, 0, 0}, {0, 0, 0}, {0,0, 0}, {0, 0, 0}, {0, 0,0} };


int triangles[][3] = { {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5},
						  {0, 5, 6}, {0, 6, 7}, {0, 7, 8}, {0, 8, 9}, {0, 9, 10} };


/* the letter B: 10 vertices, 10 colors, 9 triangles

float points[][2] = {{-.15f, .125f}, {-.5f, -.75f}, {-.5f,   .75f}, {.17f, .75f}, {.38f, .575f},
					 {.38f,   .35f}, {.23f, .125f}, {.5f,  -.125f}, {.5f,  -.5f}, {.25f, -.75f}};

float colors[][3] = {{1, 1, 1}, {1, 0, 0}, {.5f, 0, 0}, {1, 1, 0}, {.5f, 1, 0},
					 {0, 1, 0}, {0, 1, 1}, {0,   0, 1}, {1, 0, 1}, {.5f, 0, .5f}};

int triangles[][3] = {{0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5},
					  {0, 5, 6}, {0, 6, 7}, {0, 7, 8}, {0, 8, 9}, {0, 9, 1}}; */

void InitVertexBuffer() {
	// create GPU buffer, make it active, allocate memory and copy vertices
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	// allocate and fill vertex buffer
	int vsize = sizeof(points), csize = sizeof(colors);
	glBufferData(GL_ARRAY_BUFFER, vsize + csize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, points);
	glBufferSubData(GL_ARRAY_BUFFER, vsize, csize, colors);
}

// display

void Display() {
	
	float dt = (float)(clock() - startTime) / CLOCKS_PER_SEC;
	SetUniform(program, "radAng", (3.1415f / 180.f) * dt * degPerSec);

	mat4 m = RotateZ(30*dt);
	SetUniform(program,"view",m);
	
	// clear screen to grey
	glClearColor(.5f, .5f, .5f, 1);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(program);
	// establish vertex fetch for point and for color, then draw triangles
	int vsize = sizeof(points), ntris = sizeof(triangles) / (3 * sizeof(int));
	VertexAttribPointer(program, "point", 2, 0, (void*)0);
	VertexAttribPointer(program, "color", 3, 0, (void*)vsize);
	glDrawElements(GL_TRIANGLES, 3 * ntris, GL_UNSIGNED_INT, &triangles[0]);
	glFlush();
}

// application

void Close() {
	// unbind vertex buffer and free GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (vBuffer >= 0)
		glDeleteBuffers(1, &vBuffer);
}

void ErrorGFLW(int id, const char* reason) {
	printf("GFLW error %i: %s\n", id, reason);
}

void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
	glfwSetErrorCallback(ErrorGFLW);
	if (!glfwInit())
		return 1;
	GLFWwindow* window = glfwCreateWindow(600, 600, "Colorful Letter", NULL, NULL);
	if (!window) {
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	printf("GL version: %s\n", glGetString(GL_VERSION));
	PrintGLErrors();
	if (!(program = LinkProgramViaCode(&vertexShader, &pixelShader)))
		return 0;
	InitVertexBuffer();
	glfwSetKeyCallback(window, Keyboard);
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		Display();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	Close();
	glfwDestroyWindow(window);
	glfwTerminate();
}
