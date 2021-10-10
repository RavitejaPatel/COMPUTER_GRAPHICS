// 2-ClearScreen.cpp - use OpenGL shader architecture

#include <glad.h>											// GL header file
#include <glfw3.h>											// GL toolkit
#include <stdio.h>											// printf, etc.
#include "GLXtras.h"										// convenience routines

GLuint vBuffer = 0;											// GPU vert buf ID, valid if > 0
GLuint program = 0;											// shader prog ID, valid if > 0

// vertex shader: operations before the rasterizer
const char* vertexShader = R"(
	#version 130
	in vec2 point;											// 2D point from GPU memory
	void main() {
		// REQUIREMENT 1A) transform vertex:
		gl_Position = vec4(point, 0, 1);					// 'built-in' variable
	}
)";

// pixel shader: operations after the rasterizer
const char* pixelShader = R"(
	#version 130
	out vec4 pColor;
	void main() {
		// REQUIREMENT 1B) shade pixel:
		pColor = vec4(0, 1, 0, 1);							// r, g, b, alpha
	}
)";

void InitVertexBuffer() {
    // REQUIREMENT 3A) create GPU buffer, copy 4 vertices
#ifdef GL_QUADS
    float pts[][2] = { {-1,-1},{-1,1},{1,1},{1,-1} };			// “object”
#else
    float pts[][2] = { {-1,-1},{-1,1},{1,1},{-1,-1},{1,1},{1,-1} };
#endif
    glGenBuffers(1, &vBuffer);								// ID for GPU buffer
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);					// make it active
    glBufferData(GL_ARRAY_BUFFER, sizeof(pts), pts, GL_STATIC_DRAW);
}

void Display() {
    glUseProgram(program);									// ensure correct program
    glBindBuffer(GL_ARRAY_BUFFER, vBuffer);					// activate vertex buffer
    // REQUIREMENT 3B) set vertex feeder
    GLint id = glGetAttribLocation(program, "point");
    glEnableVertexAttribArray(id);
    glVertexAttribPointer(id, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
    // in subsequent code the above three lines will be replaced with
    // VertexAttribPointer(program, "point", 2, 0, (void *) 0);
#ifdef GL_QUADS
    glDrawArrays(GL_QUADS, 0, 4);							// display entire window
#else
    glDrawArrays(GL_TRIANGLES, 0, 6);
#endif
    glFlush();												// flush GL ops
}

void GlfwError(int id, const char* reason) {
    printf("GFLW error %i: %s\n", id, reason);
    getchar();
}

void APIENTRY GlslError(GLenum source, GLenum type, GLuint id, GLenum severity,
    GLsizei len, const GLchar* msg, const void* data) {
    printf("GLSL Error: %s\n", msg);
    getchar();
}

int AppError(const char* msg) {
    glfwTerminate();
    printf("Error: %s\n", msg);
    getchar();
    return 1;
}

int main() {												// application entry
    glfwSetErrorCallback(GlfwError);						// init GL framework
    if (!glfwInit())
        return 1;
    // create named window of given size
    GLFWwindow* w = glfwCreateWindow(400, 400, "Clear to Green", NULL, NULL);
    if (!w)
        return AppError("can't open window");
    glfwMakeContextCurrent(w);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);	// set OpenGL extensions
    // following line will not compile if glad.h < OpenGLv4.3
    glDebugMessageCallback(GlslError, NULL);
    // REQUIREMENT 2) build shader program
    if (!(program = LinkProgramViaCode(&vertexShader, &pixelShader)))
        return AppError("can't link shader program");
    InitVertexBuffer();										// set GPU vertex memory
    while (!glfwWindowShouldClose(w)) {						// event loop
        Display();
        if (PrintGLErrors())								// test for runtime GL error
            getchar();										// if so, pause
        glfwSwapBuffers(w);									// double-buffer is default
        glfwPollEvents();
    }
    glfwDestroyWindow(w);
    glfwTerminate();
}






















/**/


// 2-ClearScreen.cpp - use OpenGL shader architecture

#include <glad.h>											// GL header file
#include <glfw3.h>											// GL toolkit
#include <stdio.h>											// printf, etc.
#include "GLXtras.h"										// convenience routines
#include <gl/GL.h>
#include <gl/GL.h>
#include <gl/GL.h>



int width;
int height;

void GlfwError(int id, const char* reason) {
    printf("GFLW error %i: %s\n", id, reason);
    getchar();
}


void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}


int render_text(const char* t, int x, int y, FT_Face f)
{
    int ret = 0;

    GLuint* textures;

    size_t i;
    size_t length = strlen(t);

    FT_GlyphSlot g = f->glyph;

    // Allocate a texture for each character.

    textures = (GLuint*)malloc(sizeof(GLuint) * length);

    if (textures == NULL)
    {
        ret = -ENOMEM;
        goto done;
    }

    glGenTextures(length, textures);

    // FreeType glyphs are 1-byte greyscale, so we can't use alignment.

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Render each character...

    for (i = 0; i < length; ++i)
    {
        // Render this particular character using FreeType.

        if (FT_Load_Char(f, t[i], FT_LOAD_RENDER))
            continue;

        // Make the current glyph's texture active.

        glActiveTexture(textures[i]);
        glEnable(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, textures[i]);

        /*
         * To prevent artifacts when a character is not rendered exactly on
         * pixel boundaries, clamp the texture to edges, and enable linear
         * interpolation.
         */

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

        // Load the texture.

        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width,
            g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
            g->bitmap.buffer);

        // Render the texture on the screen.

        glBegin(GL_TRIANGLE_STRIP);

        glTexCoord2i(0, 0);
        glVertex2i(x + 0, y + 0);

        glTexCoord2i(g->bitmap.width, 0);
        glVertex2i(x + g->bitmap.width, y + 0);

        glTexCoord2i(0, g->bitmap.rows);
        glVertex2i(x + 0, y + g->bitmap.rows);



        glTexCoord2i(g->bitmap.width, 0);
        glVertex2i(x + g->bitmap.width, y + 0);

        glTexCoord2i(g->bitmap.width, g->bitmap.rows);
        glVertex2i(x + g->bitmap.width, y + g->bitmap.rows);

        glTexCoord2i(0, g->bitmap.rows);
        glVertex2i(x + 0, y + g->bitmap.rows);

        glEnd();
    }

    // Free our textures and we're done.

    glDeleteTextures(length, textures);
    free(textures);
done:
    return ret;
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

        glfwGetFramebufferSize(w, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);


        processInput(w);
        glfwSwapBuffers(w);									// double-buffer is default
        glfwPollEvents();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(-0.5f, (width - 1) + 0.5f,
            (height - 1) + 0.5f, -0.5f, 0.0f, 1.0f);

        // Draw some text.

        FT_Face font = get_font();


        //rendering text code
        render_text("f", 5, 5, font);
        glfwSwapBuffers(w);
        glfwPollEvents();

    }


    glfwDestroyWindow(w);
    glfwTerminate();
}
