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

#ifndef _M_RASTER_H_
#define _M_RASTER_H_

#include "m_image.h"

/* float image only */
M_API void m_raster_triangle_bbox_att4(struct m_image *dest, int minx, int miny, int maxx, int maxy, float4 *v0, float4 *v1, float4 *v2, float4 *a0, float4 *a1, float4 *a2);
M_API void m_raster_triangle_att4(struct m_image *dest, float4 *v0, float4 *v1, float4 *v2, float4 *a0, float4 *a1, float4 *a2);
M_API void m_raster_line(struct m_image *dest, float2 *p0, float2 *p1, float *color);
M_API void m_raster_circle(struct m_image *dest, float2 *p, float r, float *color);
M_API void m_raster_polygon(struct m_image *dest, float2 *points, int count, float *color);

#endif
