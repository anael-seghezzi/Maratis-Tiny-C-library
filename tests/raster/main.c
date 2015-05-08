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

/* software rasterization test */

#include "../test.h"
#include <m_raster.h>
#include <m_image_filter.h>


float3 vertices[8] = {
	{-1, -1, -1},
	{ 1, -1, -1},
	{ 1,  1, -1},
	{-1,  1, -1},
	{-1, -1,  1},
	{ 1, -1,  1},
	{ 1,  1,  1},
	{-1,  1,  1}
};

void draw(void)
{
	float2 pr[8];
	float3 pos;
	float color[3] = {1, 1, 1};
	float *data;
	int i;
	
	/* move */
	pos.x = sinf(test_t * 0.025f);
	pos.y = sinf(test_t * 0.02f) * 0.25f;
	pos.z = 5 + sinf(test_t * 0.035f) * 2.0f;
	
	/* projection */
	for (i = 0; i < 8; i++) {
		float3 v; float iz;
		
		M_ADD3(v, pos, vertices[i]);
		iz = 1.0f / v.z;
		
		pr[i].x = ((v.x * iz) + 0.5f) * test_buffer.width;
		pr[i].y = ((v.y * iz) + 0.5f) * test_buffer.height;
	}
	
	/* clear */
	memset(test_buffer.data, 0, test_buffer.size * sizeof(float));
	
	/* draw lines */
	for (i = 0; i < 4; i++) {
		int i2 = i == 3 ? 0 : i + 1;
		m_raster_line(&test_buffer, &pr[i], &pr[i2], color);
	}
	for (i = 4; i < 8; i++) {
		int i2 = i == 7 ? 4 : i + 1;
		m_raster_line(&test_buffer, &pr[i], &pr[i2], color);
	}
	for (i = 0; i < 4; i++) {
		int i2 = i + 4;
		m_raster_line(&test_buffer, &pr[i], &pr[i2], color);
	}
	
	/* add some noise */
	data = (float *)test_buffer.data;
	for (i = 0; i < test_buffer.size; i++) {
		data[i] += ((float)rand() / (float)RAND_MAX) * 0.25f;
	}
}

void main_loop(void)
{	
	draw();
	test_update();
}

int main(int argc, char **argv)
{	
	if (! test_create("M - RasterToy", 256, 256))
		return EXIT_FAILURE;
	
	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (test_state) {
		main_loop();
		thrd_yield();
	}
	#endif

	test_destroy();
	return EXIT_SUCCESS;
}
