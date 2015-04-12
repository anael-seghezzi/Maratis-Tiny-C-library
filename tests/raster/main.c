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


static struct m_image tmp_buffer = M_IMAGE_IDENTITY();
static void draw(void)
{
	float color[3] = {1, 1, 1};
	float2 p0, p1;
	float *data;
	int i;
	
	p0.x = sinf(test_t * 0.01f) * 64 + 128;
	p0.y = 10;
	p1.x = cosf(test_t * 0.025f) * 64 + 128;
	p1.y = 118;
		
	/* clear */
	memset(tmp_buffer.data, 0, tmp_buffer.size * sizeof(float));
	
	/* draw a line */
	m_raster_line(&tmp_buffer, &p0, &p1, color);
	
	/* blur */
	m_image_gaussian_blur(&test_buffer, &tmp_buffer, 3, 6);
	
	/* add some noise */
	data = (float *)test_buffer.data;
	for (i = 0; i < test_buffer.size; i++) {
		data[i] += ((float)rand() / (float)RAND_MAX) * 0.125f;
	}
}

int main(int argc, char **argv)
{	
	if (! test_create("M - RasterToy", 256, 128))
		return EXIT_FAILURE;
	
	m_image_create(&tmp_buffer, test_buffer.type, test_buffer.width, test_buffer.height, test_buffer.comp);
	
	while (test_state) {
		draw();
		test_update();
		thrd_yield();
	}
	
	m_image_destroy(&tmp_buffer);
	test_destroy();
	return EXIT_SUCCESS;
}
