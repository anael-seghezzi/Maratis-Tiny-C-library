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

#include <m_image.h>
#include "m_half.c"


void m_image_create(struct m_image *image, char type, int width, int height, int comp)
{
	int size = width * height * comp;
	assert(size > 0);

	/* already allocated */
	if (image->data != 0 && type == image->type && width == image->width && height == image->height && comp == image->comp && size == image->size)
		return;

	M_SAFE_FREE(image->data);

	switch (type) {
	case M_BYTE:
	case M_UBYTE:
		image->data = malloc(size * sizeof(char));
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		image->data = malloc(size * sizeof(short));
		break;
	case M_INT:
	case M_UINT:
		image->data = malloc(size * sizeof(int));
		break;
	case M_FLOAT:
		image->data = malloc(size * sizeof(float));
		break;
	default:
		assert(0);
		return;
	}

	image->type = type;
	image->width = width;
	image->height = height;
	image->comp = comp;
	image->size = size;
}

void m_image_destroy(struct m_image *image)
{
	M_SAFE_FREE(image->data);
	*image = m_image_identity();
}

void m_image_copy(struct m_image *dest, const struct m_image *src)
{
	m_image_create(dest, src->type, src->width, src->height, src->comp);
	
	switch (dest->type) {
	case M_BYTE:
	case M_UBYTE:
		memcpy(dest->data, src->data, dest->size*sizeof(char));
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		memcpy(dest->data, src->data, dest->size*sizeof(short));
		break;
	case M_INT:
	case M_UINT:
		memcpy(dest->data, src->data, dest->size*sizeof(int));
		break;
	case M_FLOAT:
		memcpy(dest->data, src->data, dest->size*sizeof(float));
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_copy_sub_image(struct m_image *dest, const struct m_image *src, int x, int y, int w, int h)
{
	#define COPY_SUBI(T)\
	{\
		T *sData = (T *)src->data + (M_MINy * src->width + M_MINx) * comp;\
		T *dData = (T *)dest->data;\
		int y;\
		for (y = M_MINy; y <= M_MAXy; y++) {\
			memcpy(dData, sData, dstep * sizeof(T));\
			dData += dstep;\
			sData += sstep;\
		}\
	}

	int comp = src->comp;
	int M_MINx = M_MAX(0, x);
	int M_MINy = M_MAX(0, y);
	int M_MAXx = M_CLAMP(x + w - 1, 0, src->width - 1);
	int M_MAXy = M_CLAMP(y + h - 1, 0, src->height - 1);
	int dwidth = 1 + M_MAXx - M_MINx;
	int dheight = 1 + M_MAXy - M_MINy;
	int sstep = src->width * comp;
	int dstep = dwidth * comp;

	m_image_create(dest, src->type, dwidth, dheight, src->comp);
	
	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		COPY_SUBI(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		COPY_SUBI(short);
		break;
	case M_INT:
	case M_UINT:
		COPY_SUBI(int);
		break;
	case M_FLOAT:
		COPY_SUBI(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_ubyte_to_float(struct m_image *dest, const struct m_image *src)
{
	unsigned char *src_data;
	float *dest_data;
	float ubyte_div = 1.0f / 255.0f;
	int i;

	m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);

	src_data = (unsigned char *)src->data;
	dest_data = (float *)dest->data;
	for(i=0; i<src->size; i++)
		dest_data[i] = (float)src_data[i] * ubyte_div;
}

void m_image_ushort_to_float(struct m_image *dest, const struct m_image *src)
{
	unsigned short *src_data;
	float *dest_data;
	float ushort_div = 1.0f / (float)65535;
	int i;

	m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);

	src_data = (unsigned short *)src->data;
	dest_data = (float *)dest->data;
	for(i=0; i<src->size; i++)
		dest_data[i] = (float)src_data[i] * ushort_div;
}

void m_image_half_to_float(struct m_image *dest, const struct m_image *src)
{
	uint16_t *src_data;
	float *dest_data;
	int i;

	m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);

	src_data = (uint16_t *)src->data;
	dest_data = (float *)dest->data;
	for(i=0; i<src->size; i++)
		dest_data[i] = m_half2float(src_data[i]);
}

void m_image_float_to_ubyte(struct m_image *dest, const struct m_image *src)
{
	float *src_data;
	unsigned char *dest_data;
	int i;

	m_image_create(dest, M_UBYTE, src->width, src->height, src->comp);

	src_data = (float *)src->data;
	dest_data = (unsigned char *)dest->data;
	for(i=0; i<src->size; i++) {
		int x = (int)(src_data[i] * 255);
		dest_data[i] = (unsigned char)M_CLAMP(x, 0, 255);
	}
}

void m_image_float_to_ushort(struct m_image *dest, const struct m_image *src)
{
	float *src_data;
	unsigned short *dest_data;
	int i;

	m_image_create(dest, M_USHORT, src->width, src->height, src->comp);

	src_data = (float *)src->data;
	dest_data = (unsigned short *)dest->data;
	for(i=0; i<src->size; i++) {
		int x = (int)(src_data[i] * 65535);
		dest_data[i] = (unsigned short)M_CLAMP(x, 0, 65535);
	}
}

void m_image_float_to_half(struct m_image *dest, const struct m_image *src)
{
	float *src_data;
	uint16_t *dest_data;
	int i;

	m_image_create(dest, M_USHORT, src->width, src->height, src->comp);

	src_data = (float *)src->data;
	dest_data = (uint16_t *)dest->data;
	for(i=0; i<src->size; i++)
		dest_data[i] = m_float2half(src_data[i]);
}

void m_image_extract_component(struct m_image *dest, const struct m_image *src, int c)
{
	#define EXTRACT(T)\
	{\
		T *dest_pixel = (T *)dest->data;\
		T *src_pixel = (T *)src->data;\
		for (i = 0; i < size; i += comp) {\
			(*dest_pixel) = src_pixel[c];\
			dest_pixel++;\
			src_pixel += comp;\
		}\
	}

	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int size = src->size;
	int i;

	if(c >= src->comp) {
		assert(0);
		return;
	}

	m_image_create(dest, src->type, width, height, 1);

	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		EXTRACT(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		EXTRACT(short);
		break;
	case M_INT:
	case M_UINT:
		EXTRACT(int);
		break;
	case M_FLOAT:
		EXTRACT(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_reframe(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom)
{
	#define REFRAME(T)\
	{\
		T *src_data;\
		T *src_pixel;\
		T *dest_pixel;\
		int c;\
		int x, y;\
		m_image_create(dest, src->type, width2, height2, comp);\
		src_data = (T *)src->data;\
		dest_pixel = (T *)dest->data;\
		for (y = 0; y < height2; y++) {\
			T *src_y;\
			int ys = y - top;\
			src_y = src_data + M_CLAMP(ys, 0, hm1) * width * comp;\
			for (x = 0; x < width2; x++) {\
				int xs = x - left;\
				src_pixel = src_y + M_CLAMP(xs, 0, wm1) * comp;\
				for (c = 0; c < comp; c++)\
					dest_pixel[c] = src_pixel[c];\
				dest_pixel += comp;\
			}\
		}\
	}

	if(left != 0 || top != 0 || right != 0 || bottom != 0) {

		int comp = src->comp;
		int width = src->width;
		int height = src->height;
		int width2 = width + left + right;
		int height2 = height + top + bottom;
		int wm1 = width - 1;
		int hm1 = height - 1;

		if(width2 > 0 && height2 > 0) {

			switch(src->type) {
			case M_BYTE:
			case M_UBYTE:
				REFRAME(char);
				break;
			case M_SHORT:
			case M_USHORT:
			case M_HALF:
				REFRAME(short);
				break;
			case M_INT:
			case M_UINT:
				REFRAME(int);
				break;
			case M_FLOAT:
				REFRAME(float);
				break;
			default:
				assert(0);
				break;
			}
		}
		else {
			assert(0);
		}
	}
	else {
		m_image_copy(dest, src);
	}
}

void m_image_rotate_left(struct m_image *dest, const struct m_image *src)
{
	#define ROTATE_L(T)\
	{\
		T *src_data = (T *)src->data;\
		T *dest_pixel = (T *)dest->data;\
		for (y = 0; y < width;  y++)\
		for (x = 0; x < height; x++) {\
			T *src_pixel = src_data + (x * width + (width - 1 - y)) * comp;\
			for (c = 0; c < comp; c++)\
				dest_pixel[c] = src_pixel[c];\
			dest_pixel += comp;\
		}\
	}\

	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int x, y, c;

	m_image_create(dest, src->type, height, width, comp);

	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		ROTATE_L(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		ROTATE_L(short);
		break;
	case M_INT:
	case M_UINT:
		ROTATE_L(int);
		break;
	case M_FLOAT:
		ROTATE_L(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_rotate_right(struct m_image *dest, const struct m_image *src)
{
	#define ROTATE_R(T)\
	{\
		T *src_data = (T *)src->data;\
		T *dest_pixel = (T *)dest->data;\
		for (y = 0; y < width;  y++)\
		for (x = 0; x < height; x++) {\
			T *src_pixel = src_data + ((height - 1 - x) * width + y) * comp;\
			for (c = 0; c < comp; c++)\
				dest_pixel[c] = src_pixel[c];\
			dest_pixel += comp;\
		}\
	}\

	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int x, y, c;

	m_image_create(dest, src->type, height, width, comp);

	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		ROTATE_R(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		ROTATE_R(short);
		break;
	case M_INT:
	case M_UINT:
		ROTATE_R(int);
		break;
	case M_FLOAT:
		ROTATE_R(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_rotate_180(struct m_image *dest, const struct m_image *src)
{
	#define ROTATE_180(T)\
	{\
		T *src_data = (T *)src->data;\
		T *dest_pixel = (T *)dest->data;\
		for (y = 0; y < height;  y++)\
		for (x = 0; x < width; x++) {\
			T *src_pixel = src_data + ((height - 1 - y) * width + (width - 1 - x)) * comp;\
			for (c = 0; c < comp; c++)\
				dest_pixel[c] = src_pixel[c];\
			dest_pixel += comp;\
		}\
	}\

	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int x, y, c;

	m_image_create(dest, src->type, width, height, comp);

	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		ROTATE_180(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		ROTATE_180(short);
		break;
	case M_INT:
	case M_UINT:
		ROTATE_180(int);
		break;
	case M_FLOAT:
		ROTATE_180(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_mirror_x(struct m_image *dest, const struct m_image *src)
{
	#define MIRROR_X(T)\
	{\
		T *src_data = (T *)src->data;\
		T *dest_pixel = (T *)dest->data;\
		for (y = 0; y < height;  y++)\
		for (x = 0; x < width; x++) {\
			T *src_pixel = src_data + (y * width + (width - 1 - x)) * comp;\
			for (c = 0; c < comp; c++)\
				dest_pixel[c] = src_pixel[c];\
			dest_pixel += comp;\
		}\
	}\

	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int x, y, c;

	m_image_create(dest, src->type, width, height, comp);

	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		MIRROR_X(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		MIRROR_X(short);
		break;
	case M_INT:
	case M_UINT:
		MIRROR_X(int);
		break;
	case M_FLOAT:
		MIRROR_X(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_mirror_y(struct m_image *dest, const struct m_image *src)
{
	#define MIRROR_Y(T)\
	{\
		T *src_data = (T *)src->data;\
		T *dest_pixel = (T *)dest->data;\
		for (y = 0; y < height;  y++)\
		for (x = 0; x < width; x++) {\
			T *src_pixel = src_data + ((height - 1 - y) * width + x) * comp;\
			for (c = 0; c < comp; c++)\
				dest_pixel[c] = src_pixel[c];\
			dest_pixel += comp;\
		}\
	}\

	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int x, y, c;

	m_image_create(dest, src->type, width, height, comp);

	switch(src->type)
	{
	case M_BYTE:
	case M_UBYTE:
		MIRROR_Y(char);
		break;
	case M_SHORT:
	case M_USHORT:
	case M_HALF:
		MIRROR_Y(short);
		break;
	case M_INT:
	case M_UINT:
		MIRROR_Y(int);
		break;
	case M_FLOAT:
		MIRROR_Y(float);
		break;
	default:
		assert(0);
		break;
	}
}

void m_image_sub_pixel(const struct m_image *src, float x, float y, float *result)
{
	float *colors0, *colors1, *colors2, *colors3;
	float *src_data = (float *)src->data;
	int width = src->width;
	int height = src->height;
	int comp = src->comp;
	int c;
	float fx, fy;
	int wm = width - 1;
	int hm = height - 1;
	int ix, iy, ix2, iy2;

	ix = (int)x;
	iy = (int)y;
	fx = x - (float)ix;
	fy = y - (float)iy;
	ix = M_CLAMP(ix, 0, wm);
	iy = M_CLAMP(iy, 0, hm);
	ix2 = ix + 1;
	iy2 = iy + 1;
	ix2 = M_MIN(ix2, wm);
	iy2 = M_MIN(iy2, hm);
	
	colors0 = src_data + (width * iy  + ix)  * comp;
	colors1 = src_data + (width * iy  + ix2) * comp;
	colors2 = src_data + (width * iy2 + ix)  * comp;
	colors3 = src_data + (width * iy2 + ix2) * comp;
	
	for(c = 0; c < comp; c++) {
		float A = colors0[c] + (colors2[c] - colors0[c]) * fy;
		float B = colors1[c] + (colors3[c] - colors1[c]) * fy;
		result[c] = A + (B - A) * fx;
	}
}
