
/*
* Written by Raviteja Poosala
* Computer Graphics
* MSCS @Seattle University
*/


#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include "GLXtras.h"
#include <vector>
#include <VecMat.h>
#include <Mesh.h>
#include <Camera.h>
#include <Widgets.h>
#include "Misc.h"


const char* objFilename = "C:/Users/PRATHYUSHA/Downloads/Head.obj";

std::vector<vec3> points;
//vector<vec3> normals;
std::vector<int3> triangles;

GLuint vBuffer = 0; // GPU vertex buffer ID, valid if > 0
GLuint program = 0; // GLSL program ID, valid if > 0

int winW = 500, winH = 500;
Camera camera((float)winW / winH, vec3(0, 0, 0), vec3(0, 0, -5));

//shaders
const char* vertexShader = R"(
	#version 130
	in vec3 point;
    uniform mat4 view;
void main() {
		gl_Position = view * vec4(point,1);
	}
)";



// pixel shader passes through input color
const char* pixelShader = R"(
#version 130
out vec4 pColor;
uniform vec4 color = vec4(.7, .7, .3, 1);
void main() {
pColor = color;
}
)";

void InitVertexBuffer() {
	// create GPU vertex buffer
	glGenBuffers(1, &vBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	int vsize = points.size() * sizeof(vec3);
	glBufferData(GL_ARRAY_BUFFER, vsize, NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, &points[0]);
	//glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}


int WindowHeight(GLFWwindow* w) {
	int width, height;
	glfwGetWindowSize(w, &width, &height);
	return height;
}


// Mouse Callbacks

bool Shift(GLFWwindow* w) {
	return glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
		glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
}



void MouseButton(GLFWwindow* w, int butn, int action, int mods) {
	double x, y;
	glfwGetCursorPos(w, &x, &y);
	y = winH - y;
	// called when mouse button pressed or released
	if (action == GLFW_PRESS)
		camera.MouseDown((int)x, (int)y); // save reference for MouseDrag
	if (action == GLFW_RELEASE)
		camera.MouseUp();                   // save reference for MouseDrag

}



void MouseMove(GLFWwindow* w, double x, double y) {
	if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
		y = winH - y;
		camera.MouseDrag((int)x, (int)y, Shift(w));
	}
}



void MouseWheel(GLFWwindow* w, double ignore, double spin) {
	camera.MouseWheel(spin > 0, Shift(w));
}


// display

void Display() {


	// clear screen to grey
	glClearColor(.5f, .5f, .5f, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(program);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
	glClear(GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	SetUniform(program, "view", camera.fullview);

	VertexAttribPointer(program, "point", 3, 0, (void*)0);


	for (int i = 0; i < (int)triangles.size(); i++)
	{
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_INT, &triangles[i]);
	}
	glFlush();

}//display

//Application

void Resize(GLFWwindow* window, int width, int height)
{
	camera.Resize(winW = width, winH = height);
	glViewport(0, 0, winW, winH);
}




void Close() {
	// unbind vertex buffer, free GPU memory
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &vBuffer);
}

void ErrorGFLW(int id, const char* reason) {
	printf("GFLW error %i: %s\n", id, reason);
}

void Keyboard(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}






int main()
{
	glfwSetErrorCallback(ErrorGFLW);
	if (!glfwInit())
		return 1;

	GLFWwindow* w = glfwCreateWindow(winW, winH, "Rotate 3D with Onject", NULL, NULL);
	if (!w) {
		glfwTerminate();
		return 1;
	}

	//glfwSetWindowPos(w, 100, 100);

	glfwMakeContextCurrent(w);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	printf("GL version: %s\n", glGetString(GL_VERSION));



	if (!ReadAsciiObj((char*)objFilename, points, triangles)) {
		printf("failed to read obj file (type any key to continue)\n");
		getchar();
	}
	printf("%i vertices, %i triangles\n", points.size(), triangles.size());
	Normalize(points, .8f);

	printf("GL version: %s\n", glGetString(GL_VERSION));
	PrintGLErrors();

	// init shader programs
	program = LinkProgramViaCode(&vertexShader, &pixelShader);
	InitVertexBuffer();
	// callbacks
	glfwSetWindowSizeCallback(w, Resize);
	glfwSetKeyCallback(w, Keyboard);
	glfwSetMouseButtonCallback(w, MouseButton);
	glfwSetCursorPosCallback(w, MouseMove);
	glfwSetScrollCallback(w, MouseWheel);

	printf("Line 203");

	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(w)) {
		Display();
		glfwSwapBuffers(w);
		glfwPollEvents();
		printf("Line 210");
	}
	Close();
	glfwDestroyWindow(w);
	glfwTerminate();

}