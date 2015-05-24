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

#include <m_color.h>


static float _sRGB_to_linear(float src)
{
    float l;
    if (src <= 0.03928)
        l = src / 12.92;
    else
        l = pow((src + 0.055) / 1.055, 2.4);
    return l;
}

static float _linear_to_sRGB(float src)
{
    const float a = 0.055;
    if (src < 0.0031308)
        return 12.92 * src;
    else
        return (1.0 + a) * pow(src, 1.0/2.4) - a;
}

void m_color_sRGB_to_linear(float *src, float *dest)
{
	dest[0] = _sRGB_to_linear(src[0]);
	dest[1] = _sRGB_to_linear(src[1]);
	dest[2] = _sRGB_to_linear(src[2]);
}

void m_color_linear_to_sRGB(float *src, float *dest)
{
	dest[0] = _linear_to_sRGB(src[0]);
	dest[1] = _linear_to_sRGB(src[1]);
	dest[2] = _linear_to_sRGB(src[2]);
}

void m_color_RGB_to_HSV(float *src, float *dest)
{
	float r = src[0];
	float g = src[1];
	float b = src[2];
	float h = 0;
	float s = 0;
	float v = 0;
	float min = r;
	float max = r;
	float delta;

	min = M_MIN(min, g);
	min = M_MIN(min, b);
	max = M_MAX(max, g);
	max = M_MAX(max, b);
	delta = max - min;
	v = max;

	if (delta == 0 || max == 0) {
		dest[0] = h; dest[1] = s; dest[2] = v;
		return;
	}

	s = delta / max;
	
	if (r == max)
		h = (g - b) / delta;
	else if (g == max)
		h = 2 + (b - r) / delta;
	else
		h = 4 + (r - g) / delta;
	
	h *= 60;
	if (h < 0) h += 360;

	dest[0] = h; dest[1] = s; dest[2] = v;
}

void m_color_HSV_to_RGB(float *src, float *dest)
{
	float r, g, b;
	float f, p, q, t;
	float h = src[0];
	float s = src[1];
	float v = src[2];
	int i;
	
	if (s == 0) {
		dest[0] = v; dest[1] = v; dest[2] = v;
		return;
	}
	
	h /= 60.0f;
	i = (int)floorf(h);
	f = h - i;
	p = v * (1 - s);
	q = v * (1 - s * f);
	t = v * (1 - s * (1 - f));
	
	switch (i) {
	case 0:
		r = v; g = t; b = p;
		break;
	case 1:
		r = q; g = v; b = p;
		break;
	case 2:
		r = p; g = v; b = t;
		break;
	case 3:
		r = p; g = q; b = v;
		break;
	case 4:
		r = t; g = p; b = v;
		break;
	default:
		r = v; g = p; b = q;
		break;
	}
	
	dest[0] = r; dest[1] = g; dest[2] = b;
}

void m_color_RGB_to_HSL(float *src, float *dest)
{
	float h, s, l, dr, dg, db;
	float r = src[0];
	float g = src[1];
	float b = src[2];
	float min = r;
	float max = r;
	float delta;

	min = M_MIN(min, g);
	min = M_MIN(min, b);
	max = M_MAX(max, g);
	max = M_MAX(max, b);

	delta = max - min;
	h = 0;
	s = 0;
	l = (max + min) * 0.5f;
	
	if (max == 0) {
		dest[0] = h; dest[1] = s; dest[2] = l;
		return;
	}

	if (l < 0.5f)
		s = delta / (max + min);
	else
		s = delta / (2 - delta);
	
	dr = (((max - r) / 6.0f) + (delta * 0.5f) ) / delta;
	dg = (((max - g) / 6.0f) + (delta * 0.5f) ) / delta;
	db = (((max - b) / 6.0f) + (delta * 0.5f) ) / delta;
	
	if(r == max)
		h = db - dg;
	else if(g == max)
		h = (1.0f / 3.0f) + dr - db;
	else if(b == max)
		h = (2.0f / 3.0f) + dg - dr;
	
	if (h < 0) h++;
	if (h > 1) h--;
	
	dest[0] = h; dest[1] = s; dest[2] = l;
}

static float _hue_to_RGB(float v1, float v2, float v3)
{
	float v3x6 = v3 * 6;
	if (v3 < 0) v3 += 1;
	if (v3 > 1) v3 -= 1;
	if (v3x6 < 1) return (v1 + (v2 - v1) * v3x6);
	if ((2 * v3) < 1) return v2;
	if ((3 * v3) < 2) return (v1 + (v2 - v1) * ((2.0f / 3.0f) - v3) * 6);
	return v1;
}

void m_color_HSL_to_RGB(float *src, float *dest)
{
	float h = src[0];
	float s = src[1];
	float l = src[2];
	float v1, v2;
	
	if (s == 0) {
		dest[0] = l; dest[1] = l; dest[2] = l;
		return;
	}

	if (l < 0.5f)
		v2 = l * (1.0f + s);
	else
		v2 = (l + s) - (s * l);
	
	v1 = 2.0f * l - v2;
	dest[0] = _hue_to_RGB(v1, v2, h + (1.0f / 3.0f));
	dest[1] = _hue_to_RGB(v1, v2, h);
	dest[2] = _hue_to_RGB(v1, v2, h - (1.0f / 3.0f));
}
