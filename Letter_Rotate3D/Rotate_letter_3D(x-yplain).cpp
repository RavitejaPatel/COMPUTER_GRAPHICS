// Rotate3DLetter.cpp: rotate letter in response to user

#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "GLXtras.h"

// Application Data

// letter 'B'
float points[10][3] = { {-.15f, .125f, 0}, {-.5f,  -.75f, 0}, {-.5f,  .75f, 0}, {.17f,  .75f, 0},  {.38f, .575f, 0},
                       { .38f,  .35f, 0}, { .23f, .125f, 0}, {.5f, -.125f, 0}, { .5f, -.5f,  0},  {.25f, -.75f, 0} };
float colors[10][3] = { { 1, 1, 1}, { 1, 0, 0}, {.5, 0, 0}, {1, 1, 0},  {.5, 1, 0},
                       { 0, 1, 0}, { 0, 1, 1}, { 0, 0, 1}, { 1, 0, 1}, {.5, 0, .5} };
int triangles[9][3] = { {0, 1, 2}, {0, 2, 3}, {0, 3, 4}, {0, 4, 5},
                        {0, 5, 6}, {0, 6, 7}, {0, 7, 8}, {0, 8, 9}, {0, 9, 1} };

GLuint vBuffer = 0;   // GPU vertex buffer ID
GLuint program = 0;   // GLSL program ID

// Shaders: vertex shader with view transform, trivial pixel shader

const char* vertexShader = R"(
    #version 130
    in vec3 point;
    in vec3 color;
    out vec4 vColor;
    uniform mat4 view;



    void main() {
        gl_Position = view*vec4(point, 1);
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

// Initialization

void InitVertexBuffer() {
    // create GPU vertex buffer
    glGenBuffers(1, &vBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}

// Interaction

vec2  mouseDown;            // reference for mouse drag
vec2  rotOld, rotNew;       // .x is rotation about Y-axis, .y about X-axis
float rotZ = 0;
vec2  tranOld, tranNew;
float rotSpeed = .3f, tranSpeed = .01f;

void MouseWheel(GLFWwindow* w, double ignore, double spin) {
    rotZ += (spin > 0 ? 1 : -1) * 2.5f;
}

void MouseButton(GLFWwindow* w, int butn, int action, int mods) {
    // called when mouse button pressed or released
    if (action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(w, &x, &y);
        mouseDown = vec2((float)x, (float)y); // save reference for MouseDrag
    }
    if (action == GLFW_RELEASE) {
        rotOld = rotNew;                        // save reference for MouseDrag
        tranOld = tranNew;
    }
}

void MouseMove(GLFWwindow* w, double x, double y) {
    if (glfwGetMouseButton(w, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) { // drag
        // find mouse drag difference
        vec2 mouse((float)x, (float)y), dif = mouse - mouseDown;
        bool shift = glfwGetKey(w, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(w, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        if (shift)
            tranNew = tranOld + tranSpeed * vec2(dif.x, -dif.y);    // SHIFT key: translate
        else
            rotNew = rotOld + rotSpeed * dif;                       // rotate
    }
}

// Application

void Display() {
    // clear screen to grey
    glClearColor(.5, .5, .5, 1);
    glClear(GL_COLOR_BUFFER_BIT);
    // enable z-buffer (needed for tetrahedron)
    glClear(GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    // init shader program, set vertex feed for points and colors
    glUseProgram(program);
    VertexAttribPointer(program, "point", 3, 0, (void*)0);
    VertexAttribPointer(program, "color", 3, 0, (void*)sizeof(points));
    // update view transformation
    mat4 view = Translate(tranNew.x, tranNew.y, 0) * RotateY(rotNew.x) * RotateX(rotNew.y) * RotateZ(rotZ);
    SetUniform(program, "view", view);
    glDrawElements(GL_TRIANGLES, sizeof(triangles) / sizeof(int), GL_UNSIGNED_INT, triangles);
    glFlush();
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

int main(int argc, char** argv) {
    // init app window
    if (!glfwInit())
        return 1;
    glfwSetErrorCallback(ErrorGFLW);
    GLFWwindow* w = glfwCreateWindow(400, 400, "Rotate 3D", NULL, NULL);
    if (!w) {
        glfwTerminate();
        return 1;
    }
    glfwSetWindowPos(w, 100, 100);
    glfwMakeContextCurrent(w);
    // init OpenGL
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    printf("GL version: %s\n", glGetString(GL_VERSION));
    PrintGLErrors();
    // init shader programs
    program = LinkProgramViaCode(&vertexShader, &pixelShader);
    InitVertexBuffer();
    // callbacks
    glfwSetCursorPosCallback(w, MouseMove);
    glfwSetMouseButtonCallback(w, MouseButton);
    glfwSetScrollCallback(w, MouseWheel);
    glfwSetKeyCallback(w, Keyboard);
    // event loop
    glfwSwapInterval(1);
    while (!glfwWindowShouldClose(w)) {
        Display();
        glfwPollEvents();
        glfwSwapBuffers(w);
    }
    Close();
    glfwDestroyWindow(w);
    glfwTerminate();
}
