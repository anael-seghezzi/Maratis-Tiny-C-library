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

#ifndef _M_CORNER_DETECTION_H_
#define _M_CORNER_DETECTION_H_

#include "m_image_filter.h"

/* float 1-component image only */

/* non maxima suppression */
M_API void m_image_non_max_supp(struct m_image *dest, const struct m_image *src, int radius, float threshold);

/* detect harris corners
   margin: margin around the image to exclude corners
   radius: maxima radius
   threshold: harris response threshold
   corners: corners coordinates of size max_count * 2
   max_count: maximum number of corners
   return corner count */
M_API int m_image_corner_harris(const struct m_image *src, int margin, int radius, float threshold, int *corners, int max_count);

#endif
