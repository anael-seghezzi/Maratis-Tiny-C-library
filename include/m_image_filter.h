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

#ifndef _M_IMAGE_FILTER_H_
#define _M_IMAGE_FILTER_H_

#include "m_image.h"

/* float image only */
/* if alpha channel, src image must be premultiplied */
M_API void m_image_convolution_h(struct m_image *dest, const struct m_image *src, float *kernel, int size); /* horizontal */
M_API void m_image_convolution_v(struct m_image *dest, const struct m_image *src, float *kernel, int size); /* vertical */
M_API void m_image_gaussian_blur(struct m_image *dest, const struct m_image *src, int dx, int dy);

/* create 1 component image */
M_API void m_image_grey(struct m_image *dest, const struct m_image *src);
M_API void m_image_max(struct m_image *dest, const struct m_image *src);
M_API void m_image_max_abs(struct m_image *dest, const struct m_image *src);

/* float 1-component image only */
M_API void m_image_sobel(struct m_image *dest, const struct m_image *src);
M_API void m_image_harris(struct m_image *dest, const struct m_image *src, int radius);

#endif
