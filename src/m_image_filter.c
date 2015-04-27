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


void m_image_convolution_h(struct m_image *dest, const struct m_image *src, float *kernel, int size)
{
	struct m_image copy = M_IMAGE_IDENTITY();
	float *src_data;
	float *dest_data;
	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int radius = (size - 1) / 2;
	int y, ystep, ystepc;

	assert(src->size > 0 && src->type == M_FLOAT);

	/* create source and destination images */
	m_image_reframe(&copy, src, radius, 0, radius, 0); /* apply clamped margin */
	m_image_create(dest, M_FLOAT, width, height, comp);
	
	/* clear */
	memset(dest->data, 0, dest->size * sizeof(float));

	src_data = (float *)copy.data;
	dest_data = (float *)dest->data;
	ystep = width * comp;
	ystepc = copy.width * comp;

	#pragma omp parallel for schedule(dynamic, 8)
    for (y=0; y<height; y++) {

		float *dest_pixel = dest_data + y * ystep;
		float *src_pixel_y = src_data + y * ystepc;
		int x;

        for (x = 0; x < width; x++) {

			float *src_pixel;
			int i, k;

			src_pixel = src_pixel_y + (x * comp);

			/* apply kernel */
			for (k = 0; k < size; k++) {
				float v = kernel[k];
				for (i = 0; i < comp; i++)
					dest_pixel[i] += (*src_pixel++) * v;
			}
			
			dest_pixel += comp;
        }
    }

	m_image_destroy(&copy);
}

void m_image_convolution_v(struct m_image *dest, const struct m_image *src, float *kernel, int size)
{
	struct m_image copy = M_IMAGE_IDENTITY();
	float *src_data;
	float *dest_data;
	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int radius = (size - 1) / 2;
	int y, ystep;

	assert(src->size > 0 && src->type == M_FLOAT);

	/* create source and destination images */
	m_image_reframe(&copy, src, 0, radius, 0, radius); /* apply clamped margin */
	m_image_create(dest, M_FLOAT, width, height, comp);
	
	/* clear */
	memset(dest->data, 0, dest->size * sizeof(float));

	src_data = (float *)copy.data;
	dest_data = (float *)dest->data;
	ystep = width * comp;

	#pragma omp parallel for schedule(dynamic, 8)
    for (y=0; y<height; y++) {

		float *dest_pixel = dest_data + y * ystep;
		int x;

        for (x = 0; x < width; x++) {

			float *src_pixel;
			int i, k;

			src_pixel = src_data + (y * width + x) * comp;

			/* apply kernel */
			for (k = 0; k < size; k++) {
				float v = kernel[k];
				for (i = 0; i < comp; i++)
					dest_pixel[i] += src_pixel[i] * v;
				src_pixel += ystep;
			}
			
			dest_pixel += comp;
        }
    }

	m_image_destroy(&copy);
}

void m_image_gaussian_blur(struct m_image *dest, const struct m_image *src, int dx, int dy)
{
	struct m_image tmp = M_IMAGE_IDENTITY();
	float *kernelx = NULL, *kernely = NULL;
	int kernelx_size = dx * 2 + 1;
	int kernely_size = dy * 2 + 1;
	
	assert(src->size > 0 && src->type == M_FLOAT);

	/* exit */
	if (dx == 0 && dy == 0) {
		m_image_copy(dest, src);
		return;
	}

	/* x blur */
	if (dx > 0) {
		kernelx = (float *)malloc(kernelx_size * sizeof(float));
		m_gaussian_kernel(kernelx, kernelx_size);
		if (dy > 0)
			m_image_convolution_h(&tmp, src, kernelx, kernelx_size);
		else
			m_image_convolution_h(dest, src, kernelx, kernelx_size);
	}

	/* y blur */
	if (dy > 0) {
		kernely = (float *)malloc(kernely_size * sizeof(float));
		m_gaussian_kernel(kernely, kernely_size);
		if (dx > 0)
			m_image_convolution_v(dest, &tmp, kernely, kernely_size);
		else
			m_image_convolution_v(dest, src, kernely, kernely_size);
	}

	m_image_destroy(&tmp);
	M_SAFE_FREE(kernely);
	M_SAFE_FREE(kernelx);
}

static float _convolve_pixel(float *data, int width, float *kernel)
{
	float sum = 0; int i, j;
	for(i = 0; i < 3; i++) {
		float *pixel = data + width * i;
		for(j = 0; j < 3; j++) {
			sum += (*pixel) * (*kernel);
			pixel++;
			kernel++;
		}
	}
	return sum;
}

void m_image_sobel(struct m_image *dest, const struct m_image *src)
{
	struct m_image copy = M_IMAGE_IDENTITY();
	float ky[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
	float kx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
	float *src_pixel;
	float *dest_pixel;
	int width = src->width;
	int height = src->height;
	int w2 = width + 2;
	int x, y;

	assert(src->size > 0 && src->type == M_FLOAT && src->comp == 1);

	/* create source and destination images */
	m_image_reframe(&copy, src, 1, 1, 1, 1); /* apply clamped margin */
	m_image_create(dest, M_FLOAT, width, height, 2);

	src_pixel = (float *)copy.data;
	dest_pixel = (float *)dest->data;

	for(y = 0; y < height; y++) {
		for(x = 0; x < width; x++) {
			dest_pixel[0] = _convolve_pixel(src_pixel, w2, kx);
			dest_pixel[1] = _convolve_pixel(src_pixel, w2, ky);
			src_pixel++;
			dest_pixel += 2;
		}
		src_pixel += 2;
	}

	m_image_destroy(&copy);
}

void m_image_harris(struct m_image *dest, const struct m_image *src, int radius)
{
	struct m_image tmp1 = M_IMAGE_IDENTITY();
	struct m_image tmp2 = M_IMAGE_IDENTITY();

	/* sobel */
	m_image_sobel(&tmp1, src);
	
	/* sst */
	m_image_create(&tmp2, M_FLOAT, src->width, src->height, 3);
	m_sst((float *)tmp2.data, (float *)tmp1.data, src->width * src->height);
	
	/* blur */
	m_image_copy(&tmp1, &tmp2);
	m_image_gaussian_blur(&tmp2, &tmp1, radius, radius);
	
	/* harris response */
	m_image_create(dest, M_FLOAT, src->width, src->height, 1);
	m_harris_response((float *)dest->data, (float *)tmp2.data, src->width * src->height);

	m_image_destroy(&tmp1);
	m_image_destroy(&tmp2);
}
