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

#ifndef _M_MORPHOLOGY_H_
#define _M_MORPHOLOGY_H_

#include "m_image.h"

/* ubyte 1-component image only */
M_API int m_image_floodfill_4x(struct m_image *dest, int x, int y, unsigned char ref, unsigned char value, unsigned short *stack, int stack_size);
M_API int m_image_floodfill_8x(struct m_image *dest, int x, int y, unsigned char ref, unsigned char value, unsigned short *stack, int stack_size);
M_API void m_image_dilate(struct m_image *dest, struct m_image *src);
M_API void m_image_erode(struct m_image *dest, struct m_image *src);
M_API void m_image_edge_4x(struct m_image *dest, struct m_image *src, unsigned char ref);
M_API void m_image_thin(struct m_image *dest);

#endif
