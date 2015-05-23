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

#ifndef _M_COLOR_H_
#define _M_COLOR_H_

#include "m_math.h"

/* color convertion */
M_API void m_color_sRGB_to_linear(float *src, float *dest);
M_API void m_color_linear_to_sRGB(float *src, float *dest);
M_API void m_color_RGB_to_HSV(float *src, float *dest);
M_API void m_color_HSV_to_RGB(float *src, float *dest);
M_API void m_color_RGB_to_HSL(float *src, float *dest);
M_API void m_color_HSL_to_RGB(float *src, float *dest);

#endif
