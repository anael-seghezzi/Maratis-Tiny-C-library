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
#include <m_image.h>

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
static int test_state = 1;
static int test_width = 0;
static int test_height = 0;
static unsigned long test_t = 0;

static GLuint test_texture;
static struct m_image test_buffer = M_IMAGE_IDENTITY();
static struct m_image test_buffer_ubyte = M_IMAGE_IDENTITY();


static void close_callback(GLFWwindow * window)
{
	test_state = 0;
}

static void size_callback(GLFWwindow * window, int width, int height)
{
	test_width = width;
	test_height = height;
}

static void draw_texture(GLuint texture)
{
	float vertices[8] = {0,0, 0,1, 1,0, 1,1};

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

static void send_texture(void)
{
	m_image_float_to_ubyte(&test_buffer_ubyte, &test_buffer);
	
	glBindTexture(GL_TEXTURE_2D, test_texture);
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, test_buffer_ubyte.width, test_buffer_ubyte.height, GL_RGB, GL_UNSIGNED_BYTE, test_buffer_ubyte.data);
}

void test_get_directory(char *dest, const char *src)
{
	char *s;
    strcpy(dest, src);
    s = strrchr(dest,'/');
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

int test_create(const char *title, int width, int height)
{
	glfwInit();
	test_glfw_window = glfwCreateWindow(width, height, title, NULL, NULL);
	if (test_glfw_window == NULL)
		return 0;
	
	test_width = width;
	test_height = height;
	glfwSetWindowCloseCallback(test_glfw_window, close_callback);
	glfwSetWindowSizeCallback(test_glfw_window, size_callback);
	glfwMakeContextCurrent(test_glfw_window);
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &test_texture);
	glBindTexture(GL_TEXTURE_2D, test_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, test_width, test_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	
	m_image_create(&test_buffer, M_FLOAT, test_width, test_height, 3);
	return 1;
}

void test_destroy(void)
{
	glDeleteTextures(1, &test_texture);
	m_image_destroy(&test_buffer_ubyte);
	m_image_destroy(&test_buffer);
	glfwMakeContextCurrent(NULL);
	glfwTerminate();
	test_glfw_window = NULL;
}

void test_update(void)
{
	send_texture();
	updateGL();
	glfwSwapBuffers(test_glfw_window);
	glfwPollEvents();
	test_t++;
}
