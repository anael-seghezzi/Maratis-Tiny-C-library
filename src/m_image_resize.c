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
#include <memory.h>
#include <assert.h>
#include <m_image_filter.h>
#include <m_image_resize.h>


void m_image_pyrdown(struct m_image *dest, const struct m_image *src)
{
	struct m_image tmp = M_IMAGE_IDENTITY();
	float *src_data;
	float *dest_pixel;
	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int comp2 = comp * 2;
	int ystep = width * comp * 2;
	int w2 = width / 2;
	int h2 = height / 2;
	int x, y, i;

	m_image_gaussian_blur(&tmp, src, 1, 1);
	m_image_create(dest, M_FLOAT, w2, h2, comp);

	src_data = (float *)tmp.data;
	dest_pixel = (float *)dest->data;

    for (y = 0; y < h2; y++) {
		float *src_pixel = src_data + y * ystep;
        for (x = 0; x < w2; x++) {
			for (i = 0; i < comp; i++)
				dest_pixel[i] = src_pixel[i];
			dest_pixel += comp;
			src_pixel += comp2;
        }
    }

	m_image_destroy(&tmp);
}

static void _bilinear(struct m_image *dest, const struct m_image *src, float dx, float dy, float offset)
{
	float *dest_data = (float *)dest->data;
	int width = dest->width;
	int height = dest->height;
	int comp = src->comp;
	int y, ystep = width * comp;

	#pragma omp parallel for schedule(dynamic, 8)
	for (y = 0; y < height; y++) {

		float *dest_pixel = dest_data + y * ystep; int x;
        for (x = 0; x < width; x++) {
			m_image_sub_pixel(src, ((float)x + 0.5f) * dx + offset, ((float)y + 0.5f) * dy + offset, dest_pixel);
			dest_pixel += comp;
		}
	}
}

void m_image_resize(struct m_image *dest, const struct m_image *src, int new_width, int new_height)
{
	struct m_image tmp = M_IMAGE_IDENTITY();
	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	float rx = (float)width / (float)new_width;
	float ry = (float)height / (float)new_height;

	/* TODO: use pyrdown for large scaling down ? */

	assert(src->size > 0 && src->type == M_FLOAT);
	m_image_create(dest, M_FLOAT, new_width, new_height, comp);

	if (new_width < width || new_height < height) {
		float r = M_MAX(rx, ry);
		m_image_gaussian_blur(&tmp, src, (int)r - 1, (int)r - 1);
		_bilinear(dest, &tmp, rx, ry, -0.5f);
	}
	else {
		_bilinear(dest, src, rx, ry, -0.5f);
	}

	m_image_destroy(&tmp);
}
