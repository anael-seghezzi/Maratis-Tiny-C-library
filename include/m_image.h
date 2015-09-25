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

#ifndef _M_IMAGE_H_
#define _M_IMAGE_H_

/* image basics 
 
   Example: create a 256x256 float image with 1 component:
 
   struct m_image foo = M_IMAGE_IDENTITY();
   m_image_create(&foo, M_FLOAT, 256, 256, 1);
   memset(foo.data, 0, foo.size * sizeof(float));
   m_image_destroy(&foo);
*/

#include "m_math.h"

struct m_image
{
	void *data;
	int size;
	int width;
	int height;
	int comp;
	char type;
};

/* identity, must be used before calling m_image_create */
#define M_IMAGE_IDENTITY() {0, 0, 0, 0, 0, 0}

#ifndef __OPENCL_VERSION__

/* fully supported types are: M_UBYTE, M_USHORT, M_HALF, M_FLOAT
   partially supported types: M_BYTE, M_SHORT, M_INT, M_UINT (no support for conversion) */
M_API void m_image_create(struct m_image *image, char type, int width, int height, int comp);
M_API void m_image_destroy(struct m_image *image);

M_API void m_image_ubyte_to_float(struct m_image *dest, const struct m_image *src);
M_API void m_image_ushort_to_float(struct m_image *dest, const struct m_image *src);
M_API void m_image_half_to_float(struct m_image *dest, const struct m_image *src);
M_API void m_image_float_to_ubyte(struct m_image *dest, const struct m_image *src);
M_API void m_image_float_to_ushort(struct m_image *dest, const struct m_image *src);
M_API void m_image_float_to_half(struct m_image *dest, const struct m_image *src);
M_API void m_image_float_to_srgb(struct m_image *dest, const struct m_image *src);

M_API void m_image_copy(struct m_image *dest, const struct m_image *src);
M_API void m_image_copy_sub_image(struct m_image *dest, const struct m_image *src, int x, int y, int w, int h);
M_API void m_image_reframe(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom);
M_API void m_image_extract_component(struct m_image *dest, const struct m_image *src, int c);
M_API void m_image_rotate_left(struct m_image *dest, const struct m_image *src);
M_API void m_image_rotate_right(struct m_image *dest, const struct m_image *src);
M_API void m_image_rotate_180(struct m_image *dest, const struct m_image *src);
M_API void m_image_mirror_x(struct m_image *dest, const struct m_image *src);
M_API void m_image_mirror_y(struct m_image *dest, const struct m_image *src);

/* float image only */
M_API void m_image_sub_pixel(const struct m_image *src, float x, float y, float *result);

#endif
#endif
