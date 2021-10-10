// 2-Chessboard.cpp - pixel shader exercise

#include <glad.h>
#include <glfw3.h>
#include "GLXtras.h"
#include <stdio.h>

GLuint vBuffer = 0; // GPU vertex buffer ID, valid if > 0
GLuint program = 0; // GLSL program ID, valid if > 0

// vertex shader: operations before the rasterizer
const char *vertexShader = R"(
	#version 130
	in vec2 point;
	void main() {
		gl_Position = vec4(point, 0, 1);
	}
)";

// pixel shader: operations after the rasterizer
const char *pixelShader = R"(
	#version 130
	out vec4 pColor;
	bool Odd(float n) {
		return int(n)%2 == 1; // or mod(int(n), 2) == 1
	}
	void main() {
		// divide 400x400 window into 8 columns and 8 rows:
		bool oddCol = Odd(gl_FragCoord.x/50);
		bool oddRow = Odd(gl_FragCoord.y/50);
		// black if both odd or both even, else white:
		pColor = oddCol == oddRow? vec4(0, 0, 0, 1) : vec4(1, 1, 1, 1);
	}
)";


void InitVertexBuffer() {
	// REQUIREMENT 3A) create GPU buffer, copy 4 vertices
#ifdef GL_QUADS
	float pts[][2] = {{-1,-1},{-1,1},{1,1},{1,-1}};               // “1 quad object”
#else
	float pts[][2] = {{-1,-1},{-1,1},{1,1},{-1,-1},{1,1},{1,-1}}; // “2 triangle object”
#endif
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);
}

void Display() {
	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	// REQ’T 3B) set vertex feeder
	VertexAttribPointer(program, "point", 2, 0, (void *) 0);
#ifdef GL_QUADS
	glDrawArrays(GL_QUADS, 0, 4);                   // display 4 vertices = 1 quad
#else
	glDrawArrays(GL_TRIANGLES, 0, 6);				// display 6 vertices = 2 triangles
#endif
	glFlush();                                      // flush GL ops complete
}

// application

void GlfwError(int id, const char *reason) {
	printf("GFLW error %i: %s\n", id, reason);
}

int main() {
	int winWidth = 400, winHeight = 400;
	glfwSetErrorCallback(GlfwError);
	if (!glfwInit())
		return 1;
	GLFWwindow *window = glfwCreateWindow(winWidth, winHeight, "Chessboard", NULL, NULL);
	if (!window) {
		printf("failed to open GLFW window\n");
		glfwTerminate();
		return 1;
	}
	glfwMakeContextCurrent(window);
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
	printf("GL version %s\n", glGetString(GL_VERSION));
	  // or: printf("GL version %i.%i\n", GLVersion.major, GLVersion.minor);
	if (!(program = LinkProgramViaCode(&vertexShader, &pixelShader))) {
		printf("failed to build shader program\n");
		getchar();
		return 1;
	}
	InitVertexBuffer();
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		Display();
		PrintGLErrors();
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}
