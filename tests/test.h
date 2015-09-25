/*======================================================================
 Maratis Tiny C Library
 version 1.0
------------------------------------------------------------------------
 Copyright (c) 2015 Anael Seghezzi <www.maratis3d.com>

 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.

 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:

 1. The origin of this software must not be misrepresented; you must not
    claim that you wrote the original software. If you use this software
    in a product, an acknowledgment in the product documentation would
    be appreciated but is not required.

 2. Altered source versions must be plainly marked as such, and must not
    be misrepresented as being the original software.

 3. This notice may not be removed or altered from any source
    distribution.

========================================================================*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <GLFW/glfw3.h>
#include <../deps/GL/glext.h>
#include <m_image.h>
#include <m_color.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#else
#include <tinycthread.h>
#endif

#ifdef WIN32
#include <WinBase.h>
#else
#include <unistd.h>
#endif

GLFWwindow *test_glfw_window = NULL;
char test_title[256];
int test_state = 1;
int test_width = 0;
int test_height = 0;
int test_tex_width = 0;
int test_tex_height = 0;
float test_mouse_x = 0;
float test_mouse_y = 0;
unsigned long test_t = 0;

GLuint test_texture;

#define TEST_BUTTON_COUNT 8
char test_button[TEST_BUTTON_COUNT][2]; // (release, press), pressed

static void close_callback(GLFWwindow * window)
{
	test_state = 0;
}

static void size_callback(GLFWwindow * window, int width, int height)
{
	test_width = width;
	test_height = height;
}

static void cursorpos_callback(GLFWwindow * window, double x, double y)
{
	test_mouse_x = (float)x / (float)test_width;
	test_mouse_y = (float)y / (float)test_height;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	switch (key) {
		case GLFW_KEY_RIGHT:
		test_button[0][0] = action + 1;
		test_button[0][1] = action;
		break;
		case GLFW_KEY_LEFT:
		test_button[1][0] = action + 1;
		test_button[1][1] = action;
		break;
		case GLFW_KEY_DOWN:
		test_button[2][0] = action + 1;
		test_button[2][1] = action;
		break;
		case GLFW_KEY_UP:
		test_button[3][0] = action + 1;
		test_button[3][1] = action;
		break;
		case GLFW_KEY_SPACE:
		test_button[4][0] = action + 1;
		test_button[4][1] = action;
		break;
		case GLFW_KEY_A:
		test_button[5][0] = action + 1;
		test_button[5][1] = action;
		break;
		case GLFW_KEY_Z:
		test_button[6][0] = action + 1;
		test_button[6][1] = action;
		break;
		case GLFW_KEY_ENTER:
		test_button[7][0] = action + 1;
		test_button[7][1] = action;
		break;
	}
}

static int test_key_press(int key)
{
	if (key > TEST_BUTTON_COUNT) return 0;
	return (test_button[key][0] == 2);
}

static int test_key_release(int key)
{
	if (key > TEST_BUTTON_COUNT) return 0;
	return (test_button[key][0] == 1);
}

static int test_key_pressed(int key)
{
	if (key > TEST_BUTTON_COUNT) return 0;
	return (test_button[key][1] > 0);
}

static void draw_texture(GLuint texture)
{
	float vertices[8] = {0,0, 0,1, 1,0, 1,1};

	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glTexCoordPointer(2, GL_FLOAT, 0, vertices);
	glVertexPointer(2, GL_FLOAT, 0, vertices);
	
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

static void updateGL(void)
{
	glClearColor(0.1f, 0.2f, 0.3f, 0.0f);
 	glClear(GL_COLOR_BUFFER_BIT);
	
	glViewport(0, 0, test_width, test_height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 1, 1, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor4f(1, 1, 1, 1);
	draw_texture(test_texture);
}

void test_setup_texture(int width, int height)
{
	glBindTexture(GL_TEXTURE_2D, test_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	test_tex_width = width;
	test_tex_height = height;
}

void test_swap_buffer(struct m_image *image)
{
	static struct m_image test_buffer_ubyte = M_IMAGE_IDENTITY();

	if (image) {
		
		if (image->width != test_tex_width || image->height != test_tex_height)
			test_setup_texture(image->width, image->height);

		glBindTexture(GL_TEXTURE_2D, test_texture);

		if (image->type == M_FLOAT)
		{
			m_image_float_to_srgb(&test_buffer_ubyte, image);
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, test_buffer_ubyte.width, test_buffer_ubyte.height, GL_RGB, GL_UNSIGNED_BYTE, test_buffer_ubyte.data);
		}
		else if (image->type == M_UBYTE)
		{
			glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->data);
		}
	
		updateGL();
	}
	
	glfwSwapBuffers(test_glfw_window);
}

double test_get_time(void)
{
	return glfwGetTime();
}

void test_get_directory(char *dest, const char *src)
{
	char *s;
    strcpy(dest, src);
	#ifdef WIN32
		s = strrchr(dest,'\\');
	#else
		s = strrchr(dest,'/');
	#endif
    if (s) *s = '\0';
}

void test_set_working_dir(const char *dir)
{
    #ifdef WIN32
		SetCurrentDirectory(dir);
	#else
		chdir(dir);
	#endif
}

void test_window_title(const char *title)
{
	glfwSetWindowTitle(test_glfw_window, title);
	sprintf(test_title, "%s", title);
}

int test_window(const char *title, int fullscreen)
{
	GLFWmonitor *monitor = NULL;
	GLFWwindow *win;

	if (fullscreen) {
		const GLFWvidmode *mode;
		monitor = glfwGetPrimaryMonitor();
		mode = glfwGetVideoMode(monitor);
		
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
		
		test_width = mode->width;
		test_height = mode->height;
	}
	
	win = glfwCreateWindow(test_width, test_height, title, monitor, test_glfw_window);
	if (win == NULL)
		return 0;
	
	if (test_glfw_window)
		glfwDestroyWindow(test_glfw_window);
		
	test_glfw_window = win;
	sprintf(test_title, "%s", title);
	
	glfwSetWindowCloseCallback(test_glfw_window, close_callback);
	glfwSetWindowSizeCallback(test_glfw_window, size_callback);
	glfwSetKeyCallback(test_glfw_window, key_callback);
	glfwSetCursorPosCallback(test_glfw_window, cursorpos_callback);
	glfwMakeContextCurrent(test_glfw_window);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glEnable(GL_TEXTURE_2D);
	
	return 1;
}

int test_create(const char *title, int width, int height)
{
	memset(test_button, 0, TEST_BUTTON_COUNT*2*sizeof(char));

	glfwInit();

	test_width = width;
	test_height = height;
	test_tex_width = width;
	test_tex_height = height;
	
	if (! test_window(title, 0))
		return 0;
	
	glGenTextures(1, &test_texture);
	test_setup_texture(test_tex_width, test_tex_height);
	
	return 1;
}

void test_window_size(int width, int height)
{
	glfwSetWindowSize(test_glfw_window, width, height);
}

void test_destroy(void)
{
	glDeleteTextures(1, &test_texture);
	glfwMakeContextCurrent(NULL);
	glfwTerminate();
	test_glfw_window = NULL;
}

void test_update(void)
{
	int i;
	for (i = 0; i < TEST_BUTTON_COUNT; i++)
		test_button[i][0] = 0; // flush event
	glfwPollEvents();
	test_t++;
}
