/*======================================================================
 Maratis Tiny C Library
 version 1.0
------------------------------------------------------------------------
 Copyright (c) 2015 Anael Seghezzi <www.maratis3d.org>
 Copyright (c) 2015 Marti Maria Saguer

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
/*
   Image manipulation :
   - transformation (re-frame, mirror, rotation)
   - conversions (float, half, ubyte, linear, greyscale...)
   - filtering (convolution, Gaussian blur, Harris)
   - scaling (pyramid, generic, bilinear)
   - morphology (flood-fill, dilate, erode, thinning)
   - edge and corner detection (Sobel, Harris)

   to create the implementation,
   #define M_IMAGE_IMPLEMENTATION
   in *one* C/CPP file that includes this file.

   optional:
   include after *m_math.h*
   
   //////////////////////////////////////////////////////
   Example: create a 256x256 float image with 1 component:
 
   struct m_image foo1 = M_IMAGE_IDENTITY();
   struct m_image foo2 = M_IMAGE_IDENTITY();   
   int x, y;
   
   m_image_create(&foo1, M_FLOAT, 256, 256, 1);
   memset(foo1.data, 0, foo1.size * sizeof(float)); // clear to zero
   
   y = 128; x = 128;
   ((float *)foo1.data)[y * foo1.width + x] = 1.0f; // set (x, y) pixel to one
   
   m_image_gaussian_blur(&foo2, &foo1, 3, 3); // apply Gaussian blur
   
   m_image_destroy(&foo2);
   m_image_destroy(&foo1);
*/

#ifndef M_IMAGE_H
#define M_IMAGE_H

#include <stdint.h>

#define M_IMAGE_VERSION 1

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MIAPI
#define MIAPI extern
#endif

#define M_VOID   0
#define M_BOOL   1
#define M_BYTE   2
#define M_UBYTE  3
#define M_SHORT  4
#define M_USHORT 5
#define M_INT    6
#define M_UINT   7
#define M_HALF   8
#define M_FLOAT  9
#define M_DOUBLE 10

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

/* m_image type util */
MIAPI int m_type_sizeof(char type);

/* fully supported types are: M_UBYTE, M_USHORT, M_HALF, M_FLOAT
   partially supported types: M_BYTE, M_SHORT, M_INT, M_UINT (no support for conversion) */
MIAPI void m_image_create(struct m_image *image, char type, int width, int height, int comp);
MIAPI void m_image_destroy(struct m_image *image);

MIAPI void m_image_ubyte_to_float(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_ushort_to_float(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_half_to_float(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_float_to_ubyte(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_float_to_ushort(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_float_to_half(struct m_image *dest, const struct m_image *src);

MIAPI void m_image_copy(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_copy_sub_image(struct m_image *dest, const struct m_image *src, int x, int y, int w, int h);
MIAPI void m_image_reframe_zero(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom);
MIAPI void m_image_reframe(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom);
MIAPI void m_image_extract_component(struct m_image *dest, const struct m_image *src, int c);
MIAPI void m_image_rotate_left(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_rotate_right(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_rotate_180(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_mirror_x(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_mirror_y(struct m_image *dest, const struct m_image *src);

MIAPI void m_image_premultiply(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_unpremultiply(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_sRGB_to_linear(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_linear_to_sRGB(struct m_image *dest, const struct m_image *src);

/* float/half conversion */
MIAPI float    m_half2float(uint16_t h);
MIAPI uint16_t m_float2half(float flt);

/* raw processing */
MIAPI void  m_sRGB_to_linear(float *dest, const float *src, int size);
MIAPI void  m_linear_to_sRGB(float *dest, const float *src, int size);
MIAPI void  m_RGB_to_HSV(float *dest, const float *src);
MIAPI void  m_HSV_to_RGB(float *dest, const float *src);
MIAPI void  m_RGB_to_HSL(float *dest, const float *src);
MIAPI void  m_HSL_to_RGB(float *dest, const float *src);
MIAPI void  m_gaussian_kernel(float *dest, int size, float radius);
MIAPI void  m_sst(float *dest, const float *src, int count);
MIAPI void  m_harris_response(float *dest, const float *src, int count);
MIAPI void  m_tfm(float *dest, const float *src, int count);
MIAPI void  m_normalize(float *dest, const float *src, int size); /* dest = src / norm(src) */
MIAPI void  m_normalize_sum(float *dest, const float *src, int size); /* dest = src / sum(src) */
MIAPI float m_mean(const float *src, int size);
MIAPI float m_squared_distance(const float *src1, const float *src2, int size);
MIAPI float m_squared_distance_dispatch(const float *src1, const float *src2, int size);
MIAPI float m_convolution(const float *src1, const float *src2, int size); /* a dot product really */
MIAPI float m_chi_squared_distance(const float *src1, const float *src2, int size); /* good at estimating signed hystograms difference */

/* conversion to 1 component (float image only) */
MIAPI void m_image_grey(struct m_image *dest, const struct m_image *src); /* from RGB src */
MIAPI void m_image_max(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_max_abs(struct m_image *dest, const struct m_image *src);

/* summed area table (also called "integral image") */
MIAPI void m_image_summed_area(struct m_image *dest, const struct m_image *src);

/* convolutions (float image only) */
/* if alpha channel, src image must be pre-multiplied */
MIAPI void m_image_convolution_h_raw(struct m_image *dest, const struct m_image *src, float *kernel, int size);
MIAPI void m_image_convolution_v_raw(struct m_image *dest, const struct m_image *src, float *kernel, int size);
MIAPI void m_image_convolution_h(struct m_image *dest, const struct m_image *src, float *kernel, int size); /* horizontal */
MIAPI void m_image_convolution_v(struct m_image *dest, const struct m_image *src, float *kernel, int size); /* vertical */
MIAPI void m_image_gaussian_blur(struct m_image *dest, const struct m_image *src, float dx, float dy);

/* edge and corner (float 1 component image only) */
MIAPI void m_image_sobel(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_harris(struct m_image *dest, const struct m_image *src, float radius);

/* morphology (ubyte 1 component image only) */
MIAPI int  m_image_floodfill_4x(struct m_image *dest, int x, int y, uint8_t ref, uint8_t value, uint16_t *stack, int stack_size);
MIAPI int  m_image_floodfill_8x(struct m_image *dest, int x, int y, uint8_t ref, uint8_t value, uint16_t *stack, int stack_size);
MIAPI void m_image_dilate(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_erode(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_edge_4x(struct m_image *dest, const struct m_image *src, uint8_t ref);
MIAPI void m_image_thin(struct m_image *dest);

/* non maxima suppression (float image only) */
MIAPI void m_image_non_max_supp(struct m_image *dest, const struct m_image *src, int radius, float threshold);

/* detect Harris corners
   margin: margin around the image to exclude corners
   radius: maxima radius
   threshold: Harris response threshold
   corners: corners coordinates of size max_count * 2
   max_count: maximum number of corners
   return corner count */
MIAPI int m_image_corner_harris(const struct m_image *src, int margin, float radius, float threshold, int *corners, int max_count);

/* resizing (float image only) */
MIAPI void m_image_sub_pixel(const struct m_image *src, float x, float y, float *result);
MIAPI void m_image_pyrdown(struct m_image *dest, const struct m_image *src);
MIAPI void m_image_resize(struct m_image *dest, const struct m_image *src, int new_width, int new_height);

#ifdef __cplusplus
}
#endif
/*
----------------------------------------------------------------------*/
#endif /* M_IMAGE_H */

#ifdef M_IMAGE_IMPLEMENTATION

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include <float.h>
#include <assert.h>

#ifndef M_SAFE_FREE
#define M_SAFE_FREE(p) {if (p) {free(p); (p) = NULL;}}
#endif

#ifndef M_MIN
#define M_MIN(a, b) (((a) < (b)) ? (a) : (b))
#endif
#ifndef M_MAX
#define M_MAX(a, b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef M_ABS
#define M_ABS(a) (((a) < 0) ? -(a) : (a))
#endif
#ifndef M_CLAMP
#define M_CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#endif

MIAPI void m_linear_to_sRGB(float *dest, const float *src, int size)
{
   int i;
   for (i = 0; i < size; i++) {
      if (*src < 0.0031308f)
         *dest = 12.92f * (*src);
      else
         *dest = (1.0f + 0.055f) * powf(*src, 1.0f/2.4f) - 0.055f;
      dest++;
      src++;
   }
}

MIAPI void m_sRGB_to_linear(float *dest, const float *src, int size)
{
   int i;
   for (i = 0; i < size; i++) {
      if (*src <= 0.03928f)
         *dest = *src / 12.92f;
      else
         *dest = powf((*src + 0.055f) / 1.055f, 2.4f);
      dest++;
      src++;
   }
}

MIAPI void m_RGB_to_HSV(float *dest, const float *src)
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

MIAPI void m_HSV_to_RGB(float *dest, const float *src)
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

MIAPI void m_RGB_to_HSL(float *dest, const float *src)
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

   if(r == max)
      h = fmodf(((g - b) / delta), 6.0f);
   else if(g == max)
      h = ((b - r) / delta) + 2.0f;
   else
      h = ((r - g) / delta) + 4.0f;

   h *= 60.0f;
   if (h < 0) h += 360;
    
   s = delta / (1.0f - fabsf(2.0f * l - 1.0f));
   
   dest[0] = h;
   dest[1] = s;
   dest[2] = l;
}

MIAPI void m_HSL_to_RGB(float *dest, const float *src)
{
   float h = src[0];
   float s = src[1];
   float l = src[2];
   float c, m, x;
   
   if (s == 0) {
      dest[0] = l; dest[1] = l; dest[2] = l;
      return;
   }
   
   c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
   m = 1.0f * (l - 0.5f * c);
   x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2) - 1.0f));

   if (h >= 0.0f && h < 60.0f) {
      dest[0] = c + m;
      dest[1] = x + m;
      dest[2] = m;
   }
   else if (h >= 60.0f && h < 120.0f) {
      dest[0] = x + m;
      dest[1] = c + m;
      dest[2] = m;
   }
   else if (h < 120.0f && h < 180.0f) {
      dest[0] = m;
      dest[1] = c + m;
      dest[2] = x + m;
   }
   else if (h >= 180.0f && h < 240.0f) {
      dest[0] = m;
      dest[1] = x + m;
      dest[2] = c + m;
   }
   else if (h >= 240.0f && h < 300.0f) {
      dest[0] = x + m;
      dest[1] = m;
      dest[2] = c + m;
   }
   else if (h >= 300.0f && h < 360.0f) {
      dest[0] = c + m;
      dest[1] = m;
      dest[2] = x + m;
   }
   else {
     dest[0] = m;
     dest[1] = m;
     dest[2] = m;
   }
}

MIAPI void m_gaussian_kernel(float *dest, int size, float radius)
{
   float *k;
   float rs, s2, sum;
   float sigma = 1.6f;
   float tetha = 2.25f;
   int r, hsize = size / 2;
   
   s2 = 1.0f / expf(sigma * sigma * tetha);
   rs = sigma / radius;

   k = dest;
   sum = 0.0f;

   /* compute gaussian kernel */
   for (r = -hsize; r <= hsize; r++) {
      float x = r * rs;
      float v = (1.0f / expf(x * x)) - s2;
      v = M_MAX(v, 0);
      *k = v;
      sum += v;
      k++;
   }

   /* normalize */
   if (sum > 0.0f) {
      float isum = 1.0f / sum;
      for (r = 0; r < size; r++)
         dest[r] *= isum;
   }
}

MIAPI void m_sst(float *dest, const float *src, int count)
{
   register int i;
   register float dx;
   register float dy;
   for (i = 0; i < count; i++) {
      dx = src[0];
      dy = src[1];
      dest[0] = dx*dx;
      dest[1] = dy*dy;
      dest[2] = dx*dy;
      src += 2;
      dest += 3;
   }
}

MIAPI void m_harris_response(float *dest, const float *src, int count)
{
   int i;
   for (i = 0; i < count; i++) {
      float dx2 = src[0];
      float dy2 = src[1];
      float dxy = src[2];
      *dest = (dx2 * dy2 - dxy * dxy) / (dx2 + dy2 + 1e-8f);
      src += 3;
      dest++;
   }
}

MIAPI void m_tfm(float *dest, const float *src, int count)
{
   int i;
   for (i = 0; i < count; i++) {

      if (src[0] < src[1]) {
         float dx2 = src[0];
         float dy2 = src[1];
         float dxy = src[2];
         float sqd = (dy2 * dy2) - (2.0f * dx2 * dy2) + (dx2 * dx2) + (4.0f * dxy * dxy);
         float lambda = 0.5f * (dy2 + dx2 + sqrtf(M_MAX(0, sqd)));
         dest[0] = dx2 - lambda;
         dest[1] = dxy;
      }
      else {
         float dy2 = src[0];
         float dx2 = src[1];
         float dxy = src[2];
         float sqd = (dy2 * dy2) - (2.0f * dx2 * dy2) + (dx2 * dx2) + (4.0f * dxy * dxy);
         float lambda = 0.5f * (dy2 + dx2 + sqrtf(M_MAX(0, sqd)));
         dest[0] = dxy;
         dest[1] = dx2 - lambda;
      }

      src += 3;
      dest += 2;
   }
}

MIAPI float m_chi_squared_distance(const float *src1, const float *src2, int size)
{
   int i;
   float score = 0;
   for (i = 0; i < size; i++) {

      float val1 = src1[i];
      float val2 = src2[i];

      /* chi squared distance */
      if ((val1 + val2) > 0) {
         float x = val2 - val1;
         score += (x * x) / (val1 + val2);
      }
   }

   return score * 0.5f;
}

MIAPI float m_convolution(const float *src1, const float *src2, int size)
{
   float c = 0; int i;
   for (i = 0; i < size; i++)
      c += src1[i] * src2[i];
   return c;
}

MIAPI void m_normalize(float *dest, const float *src, int size)
{
   float sum = 0.0f; int i;
   for(i = 0; i < size; i++)
      sum += src[i] * src[i];

   if (sum > 0.0f) {
      sum = 1.0f / sqrtf(sum);
      for(i = 0; i < size; i++)
         dest[i] = src[i] * sum;
   }
   else if (dest != src) {
      memset(dest, 0, size * sizeof(float));
   }
}

MIAPI void m_normalize_sum(float *dest, const float *src, int size)
{
   float sum = 0.0f; int i;
   for(i = 0; i < size; i++)
      sum += src[i];

   if (sum > 0.0f) {
      sum = 1.0f / sum;
      for(i = 0; i < size; i++)
         dest[i] = src[i] * sum;
   }
   else {
      memset(dest, 0, size * sizeof(float));
   }
}

MIAPI float m_mean(const float *src, int size)
{
   float mean = 0; int i;
   for (i = 0; i < size; i++)
      mean += (*src++);
   return mean / size;
}

MIAPI float m_squared_distance(const float *src1, const float *src2, int size)
{
   register float score = 0.0f; 
   register int i;
   register float x;

   for (i = 0; i < size; i++) {
      x = src2[i] - src1[i];
      score += x * x;
   }
   return score;
}

/* m_half2float / m_float2half :
   a big thanks to Marti Maria Saguer for allowing the use of this code
   under the zlib license from "Little Color Management System" (cmshalf.c) */

/* This code is inspired in the paper "Fast Half Float Conversions"
   by Jeroen van der Zijp */

static uint32_t m__mantissa[2048] = {
0x00000000, 0x33800000, 0x34000000, 0x34400000, 0x34800000, 0x34a00000,
0x34c00000, 0x34e00000, 0x35000000, 0x35100000, 0x35200000, 0x35300000,
0x35400000, 0x35500000, 0x35600000, 0x35700000, 0x35800000, 0x35880000,
0x35900000, 0x35980000, 0x35a00000, 0x35a80000, 0x35b00000, 0x35b80000,
0x35c00000, 0x35c80000, 0x35d00000, 0x35d80000, 0x35e00000, 0x35e80000,
0x35f00000, 0x35f80000, 0x36000000, 0x36040000, 0x36080000, 0x360c0000,
0x36100000, 0x36140000, 0x36180000, 0x361c0000, 0x36200000, 0x36240000,
0x36280000, 0x362c0000, 0x36300000, 0x36340000, 0x36380000, 0x363c0000,
0x36400000, 0x36440000, 0x36480000, 0x364c0000, 0x36500000, 0x36540000,
0x36580000, 0x365c0000, 0x36600000, 0x36640000, 0x36680000, 0x366c0000,
0x36700000, 0x36740000, 0x36780000, 0x367c0000, 0x36800000, 0x36820000,
0x36840000, 0x36860000, 0x36880000, 0x368a0000, 0x368c0000, 0x368e0000,
0x36900000, 0x36920000, 0x36940000, 0x36960000, 0x36980000, 0x369a0000,
0x369c0000, 0x369e0000, 0x36a00000, 0x36a20000, 0x36a40000, 0x36a60000,
0x36a80000, 0x36aa0000, 0x36ac0000, 0x36ae0000, 0x36b00000, 0x36b20000,
0x36b40000, 0x36b60000, 0x36b80000, 0x36ba0000, 0x36bc0000, 0x36be0000,
0x36c00000, 0x36c20000, 0x36c40000, 0x36c60000, 0x36c80000, 0x36ca0000,
0x36cc0000, 0x36ce0000, 0x36d00000, 0x36d20000, 0x36d40000, 0x36d60000,
0x36d80000, 0x36da0000, 0x36dc0000, 0x36de0000, 0x36e00000, 0x36e20000,
0x36e40000, 0x36e60000, 0x36e80000, 0x36ea0000, 0x36ec0000, 0x36ee0000,
0x36f00000, 0x36f20000, 0x36f40000, 0x36f60000, 0x36f80000, 0x36fa0000,
0x36fc0000, 0x36fe0000, 0x37000000, 0x37010000, 0x37020000, 0x37030000,
0x37040000, 0x37050000, 0x37060000, 0x37070000, 0x37080000, 0x37090000,
0x370a0000, 0x370b0000, 0x370c0000, 0x370d0000, 0x370e0000, 0x370f0000,
0x37100000, 0x37110000, 0x37120000, 0x37130000, 0x37140000, 0x37150000,
0x37160000, 0x37170000, 0x37180000, 0x37190000, 0x371a0000, 0x371b0000,
0x371c0000, 0x371d0000, 0x371e0000, 0x371f0000, 0x37200000, 0x37210000,
0x37220000, 0x37230000, 0x37240000, 0x37250000, 0x37260000, 0x37270000,
0x37280000, 0x37290000, 0x372a0000, 0x372b0000, 0x372c0000, 0x372d0000,
0x372e0000, 0x372f0000, 0x37300000, 0x37310000, 0x37320000, 0x37330000,
0x37340000, 0x37350000, 0x37360000, 0x37370000, 0x37380000, 0x37390000,
0x373a0000, 0x373b0000, 0x373c0000, 0x373d0000, 0x373e0000, 0x373f0000,
0x37400000, 0x37410000, 0x37420000, 0x37430000, 0x37440000, 0x37450000,
0x37460000, 0x37470000, 0x37480000, 0x37490000, 0x374a0000, 0x374b0000,
0x374c0000, 0x374d0000, 0x374e0000, 0x374f0000, 0x37500000, 0x37510000,
0x37520000, 0x37530000, 0x37540000, 0x37550000, 0x37560000, 0x37570000,
0x37580000, 0x37590000, 0x375a0000, 0x375b0000, 0x375c0000, 0x375d0000,
0x375e0000, 0x375f0000, 0x37600000, 0x37610000, 0x37620000, 0x37630000,
0x37640000, 0x37650000, 0x37660000, 0x37670000, 0x37680000, 0x37690000,
0x376a0000, 0x376b0000, 0x376c0000, 0x376d0000, 0x376e0000, 0x376f0000,
0x37700000, 0x37710000, 0x37720000, 0x37730000, 0x37740000, 0x37750000,
0x37760000, 0x37770000, 0x37780000, 0x37790000, 0x377a0000, 0x377b0000,
0x377c0000, 0x377d0000, 0x377e0000, 0x377f0000, 0x37800000, 0x37808000,
0x37810000, 0x37818000, 0x37820000, 0x37828000, 0x37830000, 0x37838000,
0x37840000, 0x37848000, 0x37850000, 0x37858000, 0x37860000, 0x37868000,
0x37870000, 0x37878000, 0x37880000, 0x37888000, 0x37890000, 0x37898000,
0x378a0000, 0x378a8000, 0x378b0000, 0x378b8000, 0x378c0000, 0x378c8000,
0x378d0000, 0x378d8000, 0x378e0000, 0x378e8000, 0x378f0000, 0x378f8000,
0x37900000, 0x37908000, 0x37910000, 0x37918000, 0x37920000, 0x37928000,
0x37930000, 0x37938000, 0x37940000, 0x37948000, 0x37950000, 0x37958000,
0x37960000, 0x37968000, 0x37970000, 0x37978000, 0x37980000, 0x37988000,
0x37990000, 0x37998000, 0x379a0000, 0x379a8000, 0x379b0000, 0x379b8000,
0x379c0000, 0x379c8000, 0x379d0000, 0x379d8000, 0x379e0000, 0x379e8000,
0x379f0000, 0x379f8000, 0x37a00000, 0x37a08000, 0x37a10000, 0x37a18000,
0x37a20000, 0x37a28000, 0x37a30000, 0x37a38000, 0x37a40000, 0x37a48000,
0x37a50000, 0x37a58000, 0x37a60000, 0x37a68000, 0x37a70000, 0x37a78000,
0x37a80000, 0x37a88000, 0x37a90000, 0x37a98000, 0x37aa0000, 0x37aa8000,
0x37ab0000, 0x37ab8000, 0x37ac0000, 0x37ac8000, 0x37ad0000, 0x37ad8000,
0x37ae0000, 0x37ae8000, 0x37af0000, 0x37af8000, 0x37b00000, 0x37b08000,
0x37b10000, 0x37b18000, 0x37b20000, 0x37b28000, 0x37b30000, 0x37b38000,
0x37b40000, 0x37b48000, 0x37b50000, 0x37b58000, 0x37b60000, 0x37b68000,
0x37b70000, 0x37b78000, 0x37b80000, 0x37b88000, 0x37b90000, 0x37b98000,
0x37ba0000, 0x37ba8000, 0x37bb0000, 0x37bb8000, 0x37bc0000, 0x37bc8000,
0x37bd0000, 0x37bd8000, 0x37be0000, 0x37be8000, 0x37bf0000, 0x37bf8000,
0x37c00000, 0x37c08000, 0x37c10000, 0x37c18000, 0x37c20000, 0x37c28000,
0x37c30000, 0x37c38000, 0x37c40000, 0x37c48000, 0x37c50000, 0x37c58000,
0x37c60000, 0x37c68000, 0x37c70000, 0x37c78000, 0x37c80000, 0x37c88000,
0x37c90000, 0x37c98000, 0x37ca0000, 0x37ca8000, 0x37cb0000, 0x37cb8000,
0x37cc0000, 0x37cc8000, 0x37cd0000, 0x37cd8000, 0x37ce0000, 0x37ce8000,
0x37cf0000, 0x37cf8000, 0x37d00000, 0x37d08000, 0x37d10000, 0x37d18000,
0x37d20000, 0x37d28000, 0x37d30000, 0x37d38000, 0x37d40000, 0x37d48000,
0x37d50000, 0x37d58000, 0x37d60000, 0x37d68000, 0x37d70000, 0x37d78000,
0x37d80000, 0x37d88000, 0x37d90000, 0x37d98000, 0x37da0000, 0x37da8000,
0x37db0000, 0x37db8000, 0x37dc0000, 0x37dc8000, 0x37dd0000, 0x37dd8000,
0x37de0000, 0x37de8000, 0x37df0000, 0x37df8000, 0x37e00000, 0x37e08000,
0x37e10000, 0x37e18000, 0x37e20000, 0x37e28000, 0x37e30000, 0x37e38000,
0x37e40000, 0x37e48000, 0x37e50000, 0x37e58000, 0x37e60000, 0x37e68000,
0x37e70000, 0x37e78000, 0x37e80000, 0x37e88000, 0x37e90000, 0x37e98000,
0x37ea0000, 0x37ea8000, 0x37eb0000, 0x37eb8000, 0x37ec0000, 0x37ec8000,
0x37ed0000, 0x37ed8000, 0x37ee0000, 0x37ee8000, 0x37ef0000, 0x37ef8000,
0x37f00000, 0x37f08000, 0x37f10000, 0x37f18000, 0x37f20000, 0x37f28000,
0x37f30000, 0x37f38000, 0x37f40000, 0x37f48000, 0x37f50000, 0x37f58000,
0x37f60000, 0x37f68000, 0x37f70000, 0x37f78000, 0x37f80000, 0x37f88000,
0x37f90000, 0x37f98000, 0x37fa0000, 0x37fa8000, 0x37fb0000, 0x37fb8000,
0x37fc0000, 0x37fc8000, 0x37fd0000, 0x37fd8000, 0x37fe0000, 0x37fe8000,
0x37ff0000, 0x37ff8000, 0x38000000, 0x38004000, 0x38008000, 0x3800c000,
0x38010000, 0x38014000, 0x38018000, 0x3801c000, 0x38020000, 0x38024000,
0x38028000, 0x3802c000, 0x38030000, 0x38034000, 0x38038000, 0x3803c000,
0x38040000, 0x38044000, 0x38048000, 0x3804c000, 0x38050000, 0x38054000,
0x38058000, 0x3805c000, 0x38060000, 0x38064000, 0x38068000, 0x3806c000,
0x38070000, 0x38074000, 0x38078000, 0x3807c000, 0x38080000, 0x38084000,
0x38088000, 0x3808c000, 0x38090000, 0x38094000, 0x38098000, 0x3809c000,
0x380a0000, 0x380a4000, 0x380a8000, 0x380ac000, 0x380b0000, 0x380b4000,
0x380b8000, 0x380bc000, 0x380c0000, 0x380c4000, 0x380c8000, 0x380cc000,
0x380d0000, 0x380d4000, 0x380d8000, 0x380dc000, 0x380e0000, 0x380e4000,
0x380e8000, 0x380ec000, 0x380f0000, 0x380f4000, 0x380f8000, 0x380fc000,
0x38100000, 0x38104000, 0x38108000, 0x3810c000, 0x38110000, 0x38114000,
0x38118000, 0x3811c000, 0x38120000, 0x38124000, 0x38128000, 0x3812c000,
0x38130000, 0x38134000, 0x38138000, 0x3813c000, 0x38140000, 0x38144000,
0x38148000, 0x3814c000, 0x38150000, 0x38154000, 0x38158000, 0x3815c000,
0x38160000, 0x38164000, 0x38168000, 0x3816c000, 0x38170000, 0x38174000,
0x38178000, 0x3817c000, 0x38180000, 0x38184000, 0x38188000, 0x3818c000,
0x38190000, 0x38194000, 0x38198000, 0x3819c000, 0x381a0000, 0x381a4000,
0x381a8000, 0x381ac000, 0x381b0000, 0x381b4000, 0x381b8000, 0x381bc000,
0x381c0000, 0x381c4000, 0x381c8000, 0x381cc000, 0x381d0000, 0x381d4000,
0x381d8000, 0x381dc000, 0x381e0000, 0x381e4000, 0x381e8000, 0x381ec000,
0x381f0000, 0x381f4000, 0x381f8000, 0x381fc000, 0x38200000, 0x38204000,
0x38208000, 0x3820c000, 0x38210000, 0x38214000, 0x38218000, 0x3821c000,
0x38220000, 0x38224000, 0x38228000, 0x3822c000, 0x38230000, 0x38234000,
0x38238000, 0x3823c000, 0x38240000, 0x38244000, 0x38248000, 0x3824c000,
0x38250000, 0x38254000, 0x38258000, 0x3825c000, 0x38260000, 0x38264000,
0x38268000, 0x3826c000, 0x38270000, 0x38274000, 0x38278000, 0x3827c000,
0x38280000, 0x38284000, 0x38288000, 0x3828c000, 0x38290000, 0x38294000,
0x38298000, 0x3829c000, 0x382a0000, 0x382a4000, 0x382a8000, 0x382ac000,
0x382b0000, 0x382b4000, 0x382b8000, 0x382bc000, 0x382c0000, 0x382c4000,
0x382c8000, 0x382cc000, 0x382d0000, 0x382d4000, 0x382d8000, 0x382dc000,
0x382e0000, 0x382e4000, 0x382e8000, 0x382ec000, 0x382f0000, 0x382f4000,
0x382f8000, 0x382fc000, 0x38300000, 0x38304000, 0x38308000, 0x3830c000,
0x38310000, 0x38314000, 0x38318000, 0x3831c000, 0x38320000, 0x38324000,
0x38328000, 0x3832c000, 0x38330000, 0x38334000, 0x38338000, 0x3833c000,
0x38340000, 0x38344000, 0x38348000, 0x3834c000, 0x38350000, 0x38354000,
0x38358000, 0x3835c000, 0x38360000, 0x38364000, 0x38368000, 0x3836c000,
0x38370000, 0x38374000, 0x38378000, 0x3837c000, 0x38380000, 0x38384000,
0x38388000, 0x3838c000, 0x38390000, 0x38394000, 0x38398000, 0x3839c000,
0x383a0000, 0x383a4000, 0x383a8000, 0x383ac000, 0x383b0000, 0x383b4000,
0x383b8000, 0x383bc000, 0x383c0000, 0x383c4000, 0x383c8000, 0x383cc000,
0x383d0000, 0x383d4000, 0x383d8000, 0x383dc000, 0x383e0000, 0x383e4000,
0x383e8000, 0x383ec000, 0x383f0000, 0x383f4000, 0x383f8000, 0x383fc000,
0x38400000, 0x38404000, 0x38408000, 0x3840c000, 0x38410000, 0x38414000,
0x38418000, 0x3841c000, 0x38420000, 0x38424000, 0x38428000, 0x3842c000,
0x38430000, 0x38434000, 0x38438000, 0x3843c000, 0x38440000, 0x38444000,
0x38448000, 0x3844c000, 0x38450000, 0x38454000, 0x38458000, 0x3845c000,
0x38460000, 0x38464000, 0x38468000, 0x3846c000, 0x38470000, 0x38474000,
0x38478000, 0x3847c000, 0x38480000, 0x38484000, 0x38488000, 0x3848c000,
0x38490000, 0x38494000, 0x38498000, 0x3849c000, 0x384a0000, 0x384a4000,
0x384a8000, 0x384ac000, 0x384b0000, 0x384b4000, 0x384b8000, 0x384bc000,
0x384c0000, 0x384c4000, 0x384c8000, 0x384cc000, 0x384d0000, 0x384d4000,
0x384d8000, 0x384dc000, 0x384e0000, 0x384e4000, 0x384e8000, 0x384ec000,
0x384f0000, 0x384f4000, 0x384f8000, 0x384fc000, 0x38500000, 0x38504000,
0x38508000, 0x3850c000, 0x38510000, 0x38514000, 0x38518000, 0x3851c000,
0x38520000, 0x38524000, 0x38528000, 0x3852c000, 0x38530000, 0x38534000,
0x38538000, 0x3853c000, 0x38540000, 0x38544000, 0x38548000, 0x3854c000,
0x38550000, 0x38554000, 0x38558000, 0x3855c000, 0x38560000, 0x38564000,
0x38568000, 0x3856c000, 0x38570000, 0x38574000, 0x38578000, 0x3857c000,
0x38580000, 0x38584000, 0x38588000, 0x3858c000, 0x38590000, 0x38594000,
0x38598000, 0x3859c000, 0x385a0000, 0x385a4000, 0x385a8000, 0x385ac000,
0x385b0000, 0x385b4000, 0x385b8000, 0x385bc000, 0x385c0000, 0x385c4000,
0x385c8000, 0x385cc000, 0x385d0000, 0x385d4000, 0x385d8000, 0x385dc000,
0x385e0000, 0x385e4000, 0x385e8000, 0x385ec000, 0x385f0000, 0x385f4000,
0x385f8000, 0x385fc000, 0x38600000, 0x38604000, 0x38608000, 0x3860c000,
0x38610000, 0x38614000, 0x38618000, 0x3861c000, 0x38620000, 0x38624000,
0x38628000, 0x3862c000, 0x38630000, 0x38634000, 0x38638000, 0x3863c000,
0x38640000, 0x38644000, 0x38648000, 0x3864c000, 0x38650000, 0x38654000,
0x38658000, 0x3865c000, 0x38660000, 0x38664000, 0x38668000, 0x3866c000,
0x38670000, 0x38674000, 0x38678000, 0x3867c000, 0x38680000, 0x38684000,
0x38688000, 0x3868c000, 0x38690000, 0x38694000, 0x38698000, 0x3869c000,
0x386a0000, 0x386a4000, 0x386a8000, 0x386ac000, 0x386b0000, 0x386b4000,
0x386b8000, 0x386bc000, 0x386c0000, 0x386c4000, 0x386c8000, 0x386cc000,
0x386d0000, 0x386d4000, 0x386d8000, 0x386dc000, 0x386e0000, 0x386e4000,
0x386e8000, 0x386ec000, 0x386f0000, 0x386f4000, 0x386f8000, 0x386fc000,
0x38700000, 0x38704000, 0x38708000, 0x3870c000, 0x38710000, 0x38714000,
0x38718000, 0x3871c000, 0x38720000, 0x38724000, 0x38728000, 0x3872c000,
0x38730000, 0x38734000, 0x38738000, 0x3873c000, 0x38740000, 0x38744000,
0x38748000, 0x3874c000, 0x38750000, 0x38754000, 0x38758000, 0x3875c000,
0x38760000, 0x38764000, 0x38768000, 0x3876c000, 0x38770000, 0x38774000,
0x38778000, 0x3877c000, 0x38780000, 0x38784000, 0x38788000, 0x3878c000,
0x38790000, 0x38794000, 0x38798000, 0x3879c000, 0x387a0000, 0x387a4000,
0x387a8000, 0x387ac000, 0x387b0000, 0x387b4000, 0x387b8000, 0x387bc000,
0x387c0000, 0x387c4000, 0x387c8000, 0x387cc000, 0x387d0000, 0x387d4000,
0x387d8000, 0x387dc000, 0x387e0000, 0x387e4000, 0x387e8000, 0x387ec000,
0x387f0000, 0x387f4000, 0x387f8000, 0x387fc000, 0x38000000, 0x38002000,
0x38004000, 0x38006000, 0x38008000, 0x3800a000, 0x3800c000, 0x3800e000,
0x38010000, 0x38012000, 0x38014000, 0x38016000, 0x38018000, 0x3801a000,
0x3801c000, 0x3801e000, 0x38020000, 0x38022000, 0x38024000, 0x38026000,
0x38028000, 0x3802a000, 0x3802c000, 0x3802e000, 0x38030000, 0x38032000,
0x38034000, 0x38036000, 0x38038000, 0x3803a000, 0x3803c000, 0x3803e000,
0x38040000, 0x38042000, 0x38044000, 0x38046000, 0x38048000, 0x3804a000,
0x3804c000, 0x3804e000, 0x38050000, 0x38052000, 0x38054000, 0x38056000,
0x38058000, 0x3805a000, 0x3805c000, 0x3805e000, 0x38060000, 0x38062000,
0x38064000, 0x38066000, 0x38068000, 0x3806a000, 0x3806c000, 0x3806e000,
0x38070000, 0x38072000, 0x38074000, 0x38076000, 0x38078000, 0x3807a000,
0x3807c000, 0x3807e000, 0x38080000, 0x38082000, 0x38084000, 0x38086000,
0x38088000, 0x3808a000, 0x3808c000, 0x3808e000, 0x38090000, 0x38092000,
0x38094000, 0x38096000, 0x38098000, 0x3809a000, 0x3809c000, 0x3809e000,
0x380a0000, 0x380a2000, 0x380a4000, 0x380a6000, 0x380a8000, 0x380aa000,
0x380ac000, 0x380ae000, 0x380b0000, 0x380b2000, 0x380b4000, 0x380b6000,
0x380b8000, 0x380ba000, 0x380bc000, 0x380be000, 0x380c0000, 0x380c2000,
0x380c4000, 0x380c6000, 0x380c8000, 0x380ca000, 0x380cc000, 0x380ce000,
0x380d0000, 0x380d2000, 0x380d4000, 0x380d6000, 0x380d8000, 0x380da000,
0x380dc000, 0x380de000, 0x380e0000, 0x380e2000, 0x380e4000, 0x380e6000,
0x380e8000, 0x380ea000, 0x380ec000, 0x380ee000, 0x380f0000, 0x380f2000,
0x380f4000, 0x380f6000, 0x380f8000, 0x380fa000, 0x380fc000, 0x380fe000,
0x38100000, 0x38102000, 0x38104000, 0x38106000, 0x38108000, 0x3810a000,
0x3810c000, 0x3810e000, 0x38110000, 0x38112000, 0x38114000, 0x38116000,
0x38118000, 0x3811a000, 0x3811c000, 0x3811e000, 0x38120000, 0x38122000,
0x38124000, 0x38126000, 0x38128000, 0x3812a000, 0x3812c000, 0x3812e000,
0x38130000, 0x38132000, 0x38134000, 0x38136000, 0x38138000, 0x3813a000,
0x3813c000, 0x3813e000, 0x38140000, 0x38142000, 0x38144000, 0x38146000,
0x38148000, 0x3814a000, 0x3814c000, 0x3814e000, 0x38150000, 0x38152000,
0x38154000, 0x38156000, 0x38158000, 0x3815a000, 0x3815c000, 0x3815e000,
0x38160000, 0x38162000, 0x38164000, 0x38166000, 0x38168000, 0x3816a000,
0x3816c000, 0x3816e000, 0x38170000, 0x38172000, 0x38174000, 0x38176000,
0x38178000, 0x3817a000, 0x3817c000, 0x3817e000, 0x38180000, 0x38182000,
0x38184000, 0x38186000, 0x38188000, 0x3818a000, 0x3818c000, 0x3818e000,
0x38190000, 0x38192000, 0x38194000, 0x38196000, 0x38198000, 0x3819a000,
0x3819c000, 0x3819e000, 0x381a0000, 0x381a2000, 0x381a4000, 0x381a6000,
0x381a8000, 0x381aa000, 0x381ac000, 0x381ae000, 0x381b0000, 0x381b2000,
0x381b4000, 0x381b6000, 0x381b8000, 0x381ba000, 0x381bc000, 0x381be000,
0x381c0000, 0x381c2000, 0x381c4000, 0x381c6000, 0x381c8000, 0x381ca000,
0x381cc000, 0x381ce000, 0x381d0000, 0x381d2000, 0x381d4000, 0x381d6000,
0x381d8000, 0x381da000, 0x381dc000, 0x381de000, 0x381e0000, 0x381e2000,
0x381e4000, 0x381e6000, 0x381e8000, 0x381ea000, 0x381ec000, 0x381ee000,
0x381f0000, 0x381f2000, 0x381f4000, 0x381f6000, 0x381f8000, 0x381fa000,
0x381fc000, 0x381fe000, 0x38200000, 0x38202000, 0x38204000, 0x38206000,
0x38208000, 0x3820a000, 0x3820c000, 0x3820e000, 0x38210000, 0x38212000,
0x38214000, 0x38216000, 0x38218000, 0x3821a000, 0x3821c000, 0x3821e000,
0x38220000, 0x38222000, 0x38224000, 0x38226000, 0x38228000, 0x3822a000,
0x3822c000, 0x3822e000, 0x38230000, 0x38232000, 0x38234000, 0x38236000,
0x38238000, 0x3823a000, 0x3823c000, 0x3823e000, 0x38240000, 0x38242000,
0x38244000, 0x38246000, 0x38248000, 0x3824a000, 0x3824c000, 0x3824e000,
0x38250000, 0x38252000, 0x38254000, 0x38256000, 0x38258000, 0x3825a000,
0x3825c000, 0x3825e000, 0x38260000, 0x38262000, 0x38264000, 0x38266000,
0x38268000, 0x3826a000, 0x3826c000, 0x3826e000, 0x38270000, 0x38272000,
0x38274000, 0x38276000, 0x38278000, 0x3827a000, 0x3827c000, 0x3827e000,
0x38280000, 0x38282000, 0x38284000, 0x38286000, 0x38288000, 0x3828a000,
0x3828c000, 0x3828e000, 0x38290000, 0x38292000, 0x38294000, 0x38296000,
0x38298000, 0x3829a000, 0x3829c000, 0x3829e000, 0x382a0000, 0x382a2000,
0x382a4000, 0x382a6000, 0x382a8000, 0x382aa000, 0x382ac000, 0x382ae000,
0x382b0000, 0x382b2000, 0x382b4000, 0x382b6000, 0x382b8000, 0x382ba000,
0x382bc000, 0x382be000, 0x382c0000, 0x382c2000, 0x382c4000, 0x382c6000,
0x382c8000, 0x382ca000, 0x382cc000, 0x382ce000, 0x382d0000, 0x382d2000,
0x382d4000, 0x382d6000, 0x382d8000, 0x382da000, 0x382dc000, 0x382de000,
0x382e0000, 0x382e2000, 0x382e4000, 0x382e6000, 0x382e8000, 0x382ea000,
0x382ec000, 0x382ee000, 0x382f0000, 0x382f2000, 0x382f4000, 0x382f6000,
0x382f8000, 0x382fa000, 0x382fc000, 0x382fe000, 0x38300000, 0x38302000,
0x38304000, 0x38306000, 0x38308000, 0x3830a000, 0x3830c000, 0x3830e000,
0x38310000, 0x38312000, 0x38314000, 0x38316000, 0x38318000, 0x3831a000,
0x3831c000, 0x3831e000, 0x38320000, 0x38322000, 0x38324000, 0x38326000,
0x38328000, 0x3832a000, 0x3832c000, 0x3832e000, 0x38330000, 0x38332000,
0x38334000, 0x38336000, 0x38338000, 0x3833a000, 0x3833c000, 0x3833e000,
0x38340000, 0x38342000, 0x38344000, 0x38346000, 0x38348000, 0x3834a000,
0x3834c000, 0x3834e000, 0x38350000, 0x38352000, 0x38354000, 0x38356000,
0x38358000, 0x3835a000, 0x3835c000, 0x3835e000, 0x38360000, 0x38362000,
0x38364000, 0x38366000, 0x38368000, 0x3836a000, 0x3836c000, 0x3836e000,
0x38370000, 0x38372000, 0x38374000, 0x38376000, 0x38378000, 0x3837a000,
0x3837c000, 0x3837e000, 0x38380000, 0x38382000, 0x38384000, 0x38386000,
0x38388000, 0x3838a000, 0x3838c000, 0x3838e000, 0x38390000, 0x38392000,
0x38394000, 0x38396000, 0x38398000, 0x3839a000, 0x3839c000, 0x3839e000,
0x383a0000, 0x383a2000, 0x383a4000, 0x383a6000, 0x383a8000, 0x383aa000,
0x383ac000, 0x383ae000, 0x383b0000, 0x383b2000, 0x383b4000, 0x383b6000,
0x383b8000, 0x383ba000, 0x383bc000, 0x383be000, 0x383c0000, 0x383c2000,
0x383c4000, 0x383c6000, 0x383c8000, 0x383ca000, 0x383cc000, 0x383ce000,
0x383d0000, 0x383d2000, 0x383d4000, 0x383d6000, 0x383d8000, 0x383da000,
0x383dc000, 0x383de000, 0x383e0000, 0x383e2000, 0x383e4000, 0x383e6000,
0x383e8000, 0x383ea000, 0x383ec000, 0x383ee000, 0x383f0000, 0x383f2000,
0x383f4000, 0x383f6000, 0x383f8000, 0x383fa000, 0x383fc000, 0x383fe000,
0x38400000, 0x38402000, 0x38404000, 0x38406000, 0x38408000, 0x3840a000,
0x3840c000, 0x3840e000, 0x38410000, 0x38412000, 0x38414000, 0x38416000,
0x38418000, 0x3841a000, 0x3841c000, 0x3841e000, 0x38420000, 0x38422000,
0x38424000, 0x38426000, 0x38428000, 0x3842a000, 0x3842c000, 0x3842e000,
0x38430000, 0x38432000, 0x38434000, 0x38436000, 0x38438000, 0x3843a000,
0x3843c000, 0x3843e000, 0x38440000, 0x38442000, 0x38444000, 0x38446000,
0x38448000, 0x3844a000, 0x3844c000, 0x3844e000, 0x38450000, 0x38452000,
0x38454000, 0x38456000, 0x38458000, 0x3845a000, 0x3845c000, 0x3845e000,
0x38460000, 0x38462000, 0x38464000, 0x38466000, 0x38468000, 0x3846a000,
0x3846c000, 0x3846e000, 0x38470000, 0x38472000, 0x38474000, 0x38476000,
0x38478000, 0x3847a000, 0x3847c000, 0x3847e000, 0x38480000, 0x38482000,
0x38484000, 0x38486000, 0x38488000, 0x3848a000, 0x3848c000, 0x3848e000,
0x38490000, 0x38492000, 0x38494000, 0x38496000, 0x38498000, 0x3849a000,
0x3849c000, 0x3849e000, 0x384a0000, 0x384a2000, 0x384a4000, 0x384a6000,
0x384a8000, 0x384aa000, 0x384ac000, 0x384ae000, 0x384b0000, 0x384b2000,
0x384b4000, 0x384b6000, 0x384b8000, 0x384ba000, 0x384bc000, 0x384be000,
0x384c0000, 0x384c2000, 0x384c4000, 0x384c6000, 0x384c8000, 0x384ca000,
0x384cc000, 0x384ce000, 0x384d0000, 0x384d2000, 0x384d4000, 0x384d6000,
0x384d8000, 0x384da000, 0x384dc000, 0x384de000, 0x384e0000, 0x384e2000,
0x384e4000, 0x384e6000, 0x384e8000, 0x384ea000, 0x384ec000, 0x384ee000,
0x384f0000, 0x384f2000, 0x384f4000, 0x384f6000, 0x384f8000, 0x384fa000,
0x384fc000, 0x384fe000, 0x38500000, 0x38502000, 0x38504000, 0x38506000,
0x38508000, 0x3850a000, 0x3850c000, 0x3850e000, 0x38510000, 0x38512000,
0x38514000, 0x38516000, 0x38518000, 0x3851a000, 0x3851c000, 0x3851e000,
0x38520000, 0x38522000, 0x38524000, 0x38526000, 0x38528000, 0x3852a000,
0x3852c000, 0x3852e000, 0x38530000, 0x38532000, 0x38534000, 0x38536000,
0x38538000, 0x3853a000, 0x3853c000, 0x3853e000, 0x38540000, 0x38542000,
0x38544000, 0x38546000, 0x38548000, 0x3854a000, 0x3854c000, 0x3854e000,
0x38550000, 0x38552000, 0x38554000, 0x38556000, 0x38558000, 0x3855a000,
0x3855c000, 0x3855e000, 0x38560000, 0x38562000, 0x38564000, 0x38566000,
0x38568000, 0x3856a000, 0x3856c000, 0x3856e000, 0x38570000, 0x38572000,
0x38574000, 0x38576000, 0x38578000, 0x3857a000, 0x3857c000, 0x3857e000,
0x38580000, 0x38582000, 0x38584000, 0x38586000, 0x38588000, 0x3858a000,
0x3858c000, 0x3858e000, 0x38590000, 0x38592000, 0x38594000, 0x38596000,
0x38598000, 0x3859a000, 0x3859c000, 0x3859e000, 0x385a0000, 0x385a2000,
0x385a4000, 0x385a6000, 0x385a8000, 0x385aa000, 0x385ac000, 0x385ae000,
0x385b0000, 0x385b2000, 0x385b4000, 0x385b6000, 0x385b8000, 0x385ba000,
0x385bc000, 0x385be000, 0x385c0000, 0x385c2000, 0x385c4000, 0x385c6000,
0x385c8000, 0x385ca000, 0x385cc000, 0x385ce000, 0x385d0000, 0x385d2000,
0x385d4000, 0x385d6000, 0x385d8000, 0x385da000, 0x385dc000, 0x385de000,
0x385e0000, 0x385e2000, 0x385e4000, 0x385e6000, 0x385e8000, 0x385ea000,
0x385ec000, 0x385ee000, 0x385f0000, 0x385f2000, 0x385f4000, 0x385f6000,
0x385f8000, 0x385fa000, 0x385fc000, 0x385fe000, 0x38600000, 0x38602000,
0x38604000, 0x38606000, 0x38608000, 0x3860a000, 0x3860c000, 0x3860e000,
0x38610000, 0x38612000, 0x38614000, 0x38616000, 0x38618000, 0x3861a000,
0x3861c000, 0x3861e000, 0x38620000, 0x38622000, 0x38624000, 0x38626000,
0x38628000, 0x3862a000, 0x3862c000, 0x3862e000, 0x38630000, 0x38632000,
0x38634000, 0x38636000, 0x38638000, 0x3863a000, 0x3863c000, 0x3863e000,
0x38640000, 0x38642000, 0x38644000, 0x38646000, 0x38648000, 0x3864a000,
0x3864c000, 0x3864e000, 0x38650000, 0x38652000, 0x38654000, 0x38656000,
0x38658000, 0x3865a000, 0x3865c000, 0x3865e000, 0x38660000, 0x38662000,
0x38664000, 0x38666000, 0x38668000, 0x3866a000, 0x3866c000, 0x3866e000,
0x38670000, 0x38672000, 0x38674000, 0x38676000, 0x38678000, 0x3867a000,
0x3867c000, 0x3867e000, 0x38680000, 0x38682000, 0x38684000, 0x38686000,
0x38688000, 0x3868a000, 0x3868c000, 0x3868e000, 0x38690000, 0x38692000,
0x38694000, 0x38696000, 0x38698000, 0x3869a000, 0x3869c000, 0x3869e000,
0x386a0000, 0x386a2000, 0x386a4000, 0x386a6000, 0x386a8000, 0x386aa000,
0x386ac000, 0x386ae000, 0x386b0000, 0x386b2000, 0x386b4000, 0x386b6000,
0x386b8000, 0x386ba000, 0x386bc000, 0x386be000, 0x386c0000, 0x386c2000,
0x386c4000, 0x386c6000, 0x386c8000, 0x386ca000, 0x386cc000, 0x386ce000,
0x386d0000, 0x386d2000, 0x386d4000, 0x386d6000, 0x386d8000, 0x386da000,
0x386dc000, 0x386de000, 0x386e0000, 0x386e2000, 0x386e4000, 0x386e6000,
0x386e8000, 0x386ea000, 0x386ec000, 0x386ee000, 0x386f0000, 0x386f2000,
0x386f4000, 0x386f6000, 0x386f8000, 0x386fa000, 0x386fc000, 0x386fe000,
0x38700000, 0x38702000, 0x38704000, 0x38706000, 0x38708000, 0x3870a000,
0x3870c000, 0x3870e000, 0x38710000, 0x38712000, 0x38714000, 0x38716000,
0x38718000, 0x3871a000, 0x3871c000, 0x3871e000, 0x38720000, 0x38722000,
0x38724000, 0x38726000, 0x38728000, 0x3872a000, 0x3872c000, 0x3872e000,
0x38730000, 0x38732000, 0x38734000, 0x38736000, 0x38738000, 0x3873a000,
0x3873c000, 0x3873e000, 0x38740000, 0x38742000, 0x38744000, 0x38746000,
0x38748000, 0x3874a000, 0x3874c000, 0x3874e000, 0x38750000, 0x38752000,
0x38754000, 0x38756000, 0x38758000, 0x3875a000, 0x3875c000, 0x3875e000,
0x38760000, 0x38762000, 0x38764000, 0x38766000, 0x38768000, 0x3876a000,
0x3876c000, 0x3876e000, 0x38770000, 0x38772000, 0x38774000, 0x38776000,
0x38778000, 0x3877a000, 0x3877c000, 0x3877e000, 0x38780000, 0x38782000,
0x38784000, 0x38786000, 0x38788000, 0x3878a000, 0x3878c000, 0x3878e000,
0x38790000, 0x38792000, 0x38794000, 0x38796000, 0x38798000, 0x3879a000,
0x3879c000, 0x3879e000, 0x387a0000, 0x387a2000, 0x387a4000, 0x387a6000,
0x387a8000, 0x387aa000, 0x387ac000, 0x387ae000, 0x387b0000, 0x387b2000,
0x387b4000, 0x387b6000, 0x387b8000, 0x387ba000, 0x387bc000, 0x387be000,
0x387c0000, 0x387c2000, 0x387c4000, 0x387c6000, 0x387c8000, 0x387ca000,
0x387cc000, 0x387ce000, 0x387d0000, 0x387d2000, 0x387d4000, 0x387d6000,
0x387d8000, 0x387da000, 0x387dc000, 0x387de000, 0x387e0000, 0x387e2000,
0x387e4000, 0x387e6000, 0x387e8000, 0x387ea000, 0x387ec000, 0x387ee000,
0x387f0000, 0x387f2000, 0x387f4000, 0x387f6000, 0x387f8000, 0x387fa000,
0x387fc000, 0x387fe000
};

static uint16_t m__offset[64] = {
0x0000, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0000, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400, 0x0400, 0x0400,
0x0400, 0x0400, 0x0400, 0x0400
};

static uint32_t m__exponent[64] = {
0x00000000, 0x00800000, 0x01000000, 0x01800000, 0x02000000, 0x02800000,
0x03000000, 0x03800000, 0x04000000, 0x04800000, 0x05000000, 0x05800000,
0x06000000, 0x06800000, 0x07000000, 0x07800000, 0x08000000, 0x08800000,
0x09000000, 0x09800000, 0x0a000000, 0x0a800000, 0x0b000000, 0x0b800000,
0x0c000000, 0x0c800000, 0x0d000000, 0x0d800000, 0x0e000000, 0x0e800000,
0x0f000000, 0x47800000, 0x80000000, 0x80800000, 0x81000000, 0x81800000,
0x82000000, 0x82800000, 0x83000000, 0x83800000, 0x84000000, 0x84800000,
0x85000000, 0x85800000, 0x86000000, 0x86800000, 0x87000000, 0x87800000,
0x88000000, 0x88800000, 0x89000000, 0x89800000, 0x8a000000, 0x8a800000,
0x8b000000, 0x8b800000, 0x8c000000, 0x8c800000, 0x8d000000, 0x8d800000,
0x8e000000, 0x8e800000, 0x8f000000, 0xc7800000
};

static uint16_t m__base[512] = {
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
0x0000, 0x0000, 0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040,
0x0080, 0x0100, 0x0200, 0x0400, 0x0800, 0x0c00, 0x1000, 0x1400, 0x1800, 0x1c00,
0x2000, 0x2400, 0x2800, 0x2c00, 0x3000, 0x3400, 0x3800, 0x3c00, 0x4000, 0x4400,
0x4800, 0x4c00, 0x5000, 0x5400, 0x5800, 0x5c00, 0x6000, 0x6400, 0x6800, 0x6c00,
0x7000, 0x7400, 0x7800, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00,
0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x7c00, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8001,
0x8002, 0x8004, 0x8008, 0x8010, 0x8020, 0x8040, 0x8080, 0x8100, 0x8200, 0x8400,
0x8800, 0x8c00, 0x9000, 0x9400, 0x9800, 0x9c00, 0xa000, 0xa400, 0xa800, 0xac00,
0xb000, 0xb400, 0xb800, 0xbc00, 0xc000, 0xc400, 0xc800, 0xcc00, 0xd000, 0xd400,
0xd800, 0xdc00, 0xe000, 0xe400, 0xe800, 0xec00, 0xf000, 0xf400, 0xf800, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00, 0xfc00,
0xfc00, 0xfc00
};

static uint8_t  m__shift[512] = {
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x17,
0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0d, 0x0d, 0x0d,
0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x0d, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x17, 0x16, 0x15, 0x14, 0x13,
0x12, 0x11, 0x10, 0x0f, 0x0e, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d,
0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x0d, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
0x18, 0x18, 0x18, 0x18, 0x0d
};

MIAPI float m_half2float(uint16_t h)
{
   union {
      float flt;
      uint32_t  num;
   } out;

   int n = h >> 10;

   out.num = m__mantissa[ (h & 0x3ff) + m__offset[n]] + m__exponent[n];
   return out.flt;
}

MIAPI uint16_t m_float2half(float flt)
{
   union {
      float flt;
      uint32_t  num;
   } in;

   uint32_t n, j;

   in.flt = flt;
   n = in.num;
   j = (n >> 23) & 0x1ff;

   return (uint16_t) ((uint32_t) m__base[j] + ((n & 0x007fffff) >> m__shift[j]));
}

MIAPI int m_type_sizeof(char type)
{
   switch (type) {
   case M_BYTE:
   case M_UBYTE:
      return sizeof(uint8_t);
      break;
   case M_SHORT:
   case M_USHORT:
   case M_HALF:
      return sizeof(uint16_t);
      break;
   case M_BOOL:
   case M_INT:
   case M_UINT:
      return sizeof(uint32_t);
      break;
   case M_FLOAT:
      return sizeof(float);
      break;
   case M_DOUBLE:
      return sizeof(double);
      break;
   default:
      assert(0);
      return 0;
   }
}

MIAPI void m_image_create(struct m_image *image, char type, int width, int height, int comp)
{
   int size = width * height * comp;
   assert(size > 0);

   /* already allocated */
   if (image->data != 0 && type == image->type && width == image->width && height == image->height && comp == image->comp)
      return;

   M_SAFE_FREE(image->data);

   image->data = malloc(size * m_type_sizeof(type));
   if( !image->data ) 
      printf("BAD ALLOC:m_image_create\n");
   image->type = type;
   image->width = width;
   image->height = height;
   image->comp = comp;
   image->size = size;
}

MIAPI void m_image_destroy(struct m_image *image)
{
   M_SAFE_FREE(image->data);
   memset(image, 0, sizeof(struct m_image));
}

MIAPI void m_image_copy(struct m_image *dest, const struct m_image *src)
{
   m_image_create(dest, src->type, src->width, src->height, src->comp);
   
   switch (dest->type) {
   case M_BYTE:
   case M_UBYTE:
      memcpy(dest->data, src->data, dest->size*sizeof(uint8_t));
      break;
   case M_SHORT:
   case M_USHORT:
   case M_HALF:
      memcpy(dest->data, src->data, dest->size*sizeof(uint16_t));
      break;
   case M_INT:
   case M_UINT:
      memcpy(dest->data, src->data, dest->size*sizeof(uint32_t));
      break;
   case M_FLOAT:
      memcpy(dest->data, src->data, dest->size*sizeof(float));
      break;
   default:
      assert(0);
      break;
   }
}

MIAPI void m_image_copy_sub_image(struct m_image *dest, const struct m_image *src, int x, int y, int w, int h)
{
   #define M_COPY_SUBI(T)\
   {\
      T *sData = (T *)src->data + (miny * src->width + minx) * comp;\
      T *dData = (T *)dest->data;\
      int y;\
      for (y = miny; y <= maxy; y++) {\
         memcpy(dData, sData, dstep * sizeof(T));\
         dData += dstep;\
         sData += sstep;\
      }\
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_copy_sub_image(dest, &tmp, x, y, w, h);
      m_image_destroy(&tmp);
   }
   else {
      int comp = src->comp;
      int minx = M_MAX(0, x);
      int miny = M_MAX(0, y);
      int maxx = M_CLAMP(x + w - 1, 0, src->width - 1);
      int maxy = M_CLAMP(y + h - 1, 0, src->height - 1);
      int dwidth = 1 + maxx - minx;
      int dheight = 1 + maxy - miny;
      int sstep = src->width * comp;
      int dstep = dwidth * comp;

      m_image_create(dest, src->type, dwidth, dheight, src->comp);
      
      switch(src->type)
      {
      case M_BYTE:
      case M_UBYTE:
         M_COPY_SUBI(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_COPY_SUBI(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_COPY_SUBI(uint32_t);
         break;
      case M_FLOAT:
         M_COPY_SUBI(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_COPY_SUBI
}

MIAPI void m_image_ubyte_to_float(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_ubyte_to_float(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      uint8_t *src_data;
      float *dest_data;
      float ubyte_div = 1.0f / 255.0f;
      int i;

      m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);

      src_data = (uint8_t *)src->data;
      dest_data = (float *)dest->data;
      for (i = 0; i < src->size; i++)
         dest_data[i] = (float)src_data[i] * ubyte_div;
   }
}

MIAPI void m_image_ushort_to_float(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_ushort_to_float(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      uint16_t *src_data;
      float *dest_data;
      float ushort_div = 1.0f / (float)65535;
      int i;

      m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);

      src_data = (uint16_t *)src->data;
      dest_data = (float *)dest->data;
      for (i = 0; i < src->size; i++)
         dest_data[i] = (float)src_data[i] * ushort_div;
   }
}

MIAPI void m_image_half_to_float(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_half_to_float(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      uint16_t *src_data;
      float *dest_data;
      int i;

      m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);

      src_data = (uint16_t *)src->data;
      dest_data = (float *)dest->data;
      for (i = 0; i < src->size; i++)
         dest_data[i] = m_half2float(src_data[i]);
   }
}

MIAPI void m_image_float_to_ubyte(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_float_to_ubyte(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      float *src_data;
      uint8_t *dest_data;
      int i;

      m_image_create(dest, M_UBYTE, src->width, src->height, src->comp);

      src_data = (float *)src->data;
      dest_data = (uint8_t *)dest->data;
      for (i = 0; i < src->size; i++) {
         int x = (int)(src_data[i] * 255);
         dest_data[i] = (uint8_t)M_CLAMP(x, 0, 255);
      }
   }
}

MIAPI void m_image_float_to_ushort(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_float_to_ushort(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      float *src_data;
      uint16_t *dest_data;
      int i;

      m_image_create(dest, M_USHORT, src->width, src->height, src->comp);

      src_data = (float *)src->data;
      dest_data = (uint16_t *)dest->data;
      for (i = 0; i < src->size; i++) {
         int x = (int)(src_data[i] * 65535);
         dest_data[i] = (uint16_t)M_CLAMP(x, 0, 65535);
      }
   }
}

MIAPI void m_image_float_to_half(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_float_to_half(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      float *src_data;
      uint16_t *dest_data;
      int i;

      m_image_create(dest, M_HALF, src->width, src->height, src->comp);

      src_data = (float *)src->data;
      dest_data = (uint16_t *)dest->data;
      for (i = 0; i < src->size; i++)
         dest_data[i] = m_float2half(src_data[i]);
   }
}

MIAPI void m_image_extract_component(struct m_image *dest, const struct m_image *src, int c)
{
   #define M_EXTRACT(T)\
   {\
      T *dest_pixel = (T *)dest->data;\
      T *src_pixel = (T *)src->data;\
      for (i = 0; i < size; i += comp) {\
         (*dest_pixel) = src_pixel[c];\
         dest_pixel++;\
         src_pixel += comp;\
      }\
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_extract_component(dest, &tmp, c);
      m_image_destroy(&tmp);
   }
   else {
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
         M_EXTRACT(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_EXTRACT(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_EXTRACT(uint32_t);
         break;
      case M_FLOAT:
         M_EXTRACT(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_EXTRACT
}

MIAPI void m_image_reframe_zero(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom)
{
   #define M_REFRAME(T)\
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
         int ys = y - top;\
         for (x = 0; x < width2; x++) {\
            int xs = x - left;\
            if (ys >= 0 && ys < height && xs >= 0 && xs < width) {\
               src_pixel = src_data + (ys * width + xs) * comp;\
               for (c = 0; c < comp; c++)\
                  dest_pixel[c] = src_pixel[c];\
            }\
            else {\
               for (c = 0; c < comp; c++)\
                  dest_pixel[c] = 0;\
            }\
            dest_pixel += comp;\
         }\
      }\
   }

   if(left != 0 || top != 0 || right != 0 || bottom != 0) {

      if (dest == src) {
         struct m_image tmp = M_IMAGE_IDENTITY();
         m_image_copy(&tmp, src);
         m_image_reframe_zero(dest, &tmp, left, top, right, bottom);
         m_image_destroy(&tmp);
      }
      else {
         int comp = src->comp;
         int width = src->width;
         int height = src->height;
         int width2 = width + left + right;
         int height2 = height + top + bottom;

         if(width2 > 0 && height2 > 0) {

            switch(src->type) {
            case M_BYTE:
            case M_UBYTE:
               M_REFRAME(uint8_t);
               break;
            case M_SHORT:
            case M_USHORT:
            case M_HALF:
               M_REFRAME(uint16_t);
               break;
            case M_INT:
            case M_UINT:
               M_REFRAME(uint32_t);
               break;
            case M_FLOAT:
               M_REFRAME(float);
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
   }
   else {
      m_image_copy(dest, src);
   }

   #undef M_REFRAME
}

MIAPI void m_image_reframe(struct m_image *dest, const struct m_image *src, int left, int top, int right, int bottom)
{
   #define M_REFRAME(T)\
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

      if (dest == src) {
         struct m_image tmp = M_IMAGE_IDENTITY();
         m_image_copy(&tmp, src);
         m_image_reframe(dest, &tmp, left, top, right, bottom);
         m_image_destroy(&tmp);
      }
      else {
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
               M_REFRAME(uint8_t);
               break;
            case M_SHORT:
            case M_USHORT:
            case M_HALF:
               M_REFRAME(uint16_t);
               break;
            case M_INT:
            case M_UINT:
               M_REFRAME(uint32_t);
               break;
            case M_FLOAT:
               M_REFRAME(float);
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
   }
   else {
      m_image_copy(dest, src);
   }

   #undef M_REFRAME
}

MIAPI void m_image_rotate_left(struct m_image *dest, const struct m_image *src)
{
   #define M_ROTATE_L(T)\
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
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_rotate_left(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      int width = src->width;
      int height = src->height;
      int comp = src->comp;
      int x, y, c;

      m_image_create(dest, src->type, height, width, comp);

      switch(src->type)
      {
      case M_BYTE:
      case M_UBYTE:
         M_ROTATE_L(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_ROTATE_L(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_ROTATE_L(uint32_t);
         break;
      case M_FLOAT:
         M_ROTATE_L(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_ROTATE_L
}

MIAPI void m_image_rotate_right(struct m_image *dest, const struct m_image *src)
{
   #define M_ROTATE_R(T)\
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
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_rotate_right(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      int width = src->width;
      int height = src->height;
      int comp = src->comp;
      int x, y, c;

      m_image_create(dest, src->type, height, width, comp);

      switch(src->type)
      {
      case M_BYTE:
      case M_UBYTE:
         M_ROTATE_R(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_ROTATE_R(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_ROTATE_R(uint32_t);
         break;
      case M_FLOAT:
         M_ROTATE_R(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_ROTATE_R
}

MIAPI void m_image_rotate_180(struct m_image *dest, const struct m_image *src)
{
   #define M_ROTATE_180(T)\
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
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_rotate_180(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      int width = src->width;
      int height = src->height;
      int comp = src->comp;
      int x, y, c;

      m_image_create(dest, src->type, width, height, comp);

      switch(src->type)
      {
      case M_BYTE:
      case M_UBYTE:
         M_ROTATE_180(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_ROTATE_180(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_ROTATE_180(uint32_t);
         break;
      case M_FLOAT:
         M_ROTATE_180(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_ROTATE_180
}

MIAPI void m_image_mirror_x(struct m_image *dest, const struct m_image *src)
{
   #define M_MIRROR_X(T)\
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
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_mirror_x(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      int width = src->width;
      int height = src->height;
      int comp = src->comp;
      int x, y, c;

      m_image_create(dest, src->type, width, height, comp);

      switch(src->type)
      {
      case M_BYTE:
      case M_UBYTE:
         M_MIRROR_X(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_MIRROR_X(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_MIRROR_X(uint32_t);
         break;
      case M_FLOAT:
         M_MIRROR_X(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_MIRROR_X
}

MIAPI void m_image_mirror_y(struct m_image *dest, const struct m_image *src)
{
   #define M_MIRROR_Y(T)\
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
   }

   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
	  m_image_mirror_y(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      int width = src->width;
      int height = src->height;
      int comp = src->comp;
      int x, y, c;

      m_image_create(dest, src->type, width, height, comp);

      switch(src->type)
      {
      case M_BYTE:
      case M_UBYTE:
         M_MIRROR_Y(uint8_t);
         break;
      case M_SHORT:
      case M_USHORT:
      case M_HALF:
         M_MIRROR_Y(uint16_t);
         break;
      case M_INT:
      case M_UINT:
         M_MIRROR_Y(uint32_t);
         break;
      case M_FLOAT:
         M_MIRROR_Y(float);
         break;
      default:
         assert(0);
         break;
      }
   }

   #undef M_MIRROR_Y
}

MIAPI void m_image_premultiply(struct m_image *dest, const struct m_image *src)
{
   float *dest_p, *src_p;
   int i;
   
   assert(src->size > 0 && src->type == M_FLOAT && src->comp == 4);

   m_image_create(dest, M_FLOAT, src->width, src->height, 4);
   dest_p = (float *)dest->data;
   src_p = (float *)src->data;

   for (i = 0; i < src->size; i+=4) {
      dest_p[0] = src_p[0] * src_p[3];
      dest_p[1] = src_p[1] * src_p[3];
      dest_p[2] = src_p[2] * src_p[3];
      dest_p[3] = src_p[3];
      dest_p += 4;
      src_p += 4;
   }
}

MIAPI void m_image_unpremultiply(struct m_image *dest, const struct m_image *src)
{
   float *dest_p, *src_p;
   int i;
   
   assert(src->size > 0 && src->type == M_FLOAT && src->comp == 4);

   m_image_create(dest, M_FLOAT, src->width, src->height, 4);
   dest_p = (float *)dest->data;
   src_p = (float *)src->data;

   for (i = 0; i < src->size; i+=4) {
      if (src_p[3] > 0.0f) {
         float x = 1.0f / src_p[3];
         dest_p[0] = src_p[0] * x;
         dest_p[1] = src_p[1] * x;
         dest_p[2] = src_p[2] * x;
      }
      else {
         dest_p[0] = 0;
         dest_p[1] = 0;
         dest_p[2] = 0;
      }
      dest_p[3] = src_p[3];
      dest_p += 4;
      src_p += 4;
   }
}

MIAPI void m_image_sRGB_to_linear(struct m_image *dest, const struct m_image *src)
{
   float *dest_p, *src_p;
   int i, c, comp3 = M_MIN(src->comp, 3);

   assert(src->size > 0 && src->type == M_FLOAT);

   m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);
   dest_p = (float *)dest->data;
   src_p = (float *)src->data;

   if (dest == src) {
      #pragma omp parallel for schedule(dynamic, 8)
      for (i = 0; i < src->size; i+=src->comp) {
         m_sRGB_to_linear(dest_p+i, src_p+i, comp3);
      }
   }
   else {
      #pragma omp parallel for schedule(dynamic, 8)
      for (i = 0; i < src->size; i+=src->comp) {
         m_sRGB_to_linear(dest_p+i, src_p+i, comp3);
         for (c = comp3; c < src->comp; c++)
            dest_p[i+c] = src_p[i+c];
      }
   }
}

MIAPI void m_image_linear_to_sRGB(struct m_image *dest, const struct m_image *src)
{
   float *dest_p, *src_p;
   int i, c, comp3 = M_MIN(src->comp, 3);

   assert(src->size > 0 && src->type == M_FLOAT);

   m_image_create(dest, M_FLOAT, src->width, src->height, src->comp);
   dest_p = (float *)dest->data;
   src_p = (float *)src->data;

   if (dest == src) {
      #pragma omp parallel for schedule(dynamic, 8)
      for (i = 0; i < src->size; i+=src->comp) {
         m_linear_to_sRGB(dest_p+i, src_p+i, comp3);
      }
   }
   else {
      #pragma omp parallel for schedule(dynamic, 8)
      for (i = 0; i < src->size; i+=src->comp) {
         m_linear_to_sRGB(dest_p+i, src_p+i, comp3);
         for (c = comp3; c < src->comp; c++)
            dest_p[i+c] = src_p[i+c];
      }
   }
}

MIAPI void m_image_summed_area(struct m_image *dest, const struct m_image *src)
{
   float *src_pixel;
   float *dest_pixel;
   int width = src->width;
   int height = src->height;
   int comp = src->comp;
   int x, y, c;

   assert(src->size > 0 && src->type == M_FLOAT);
   if (dest != src)
      m_image_copy(dest, src);

   /* horiz sum */
   dest_pixel = (float *)dest->data;
   for (y = 0; y < height; y++) {
      float *prev_pixel = dest_pixel;
      dest_pixel += comp;

      for (x = 1; x < width; x++) {
         for (c = 0; c < comp; c++)
            dest_pixel[c] += prev_pixel[c];

         prev_pixel = dest_pixel;
         dest_pixel += comp;
      }
   }

   /* vertical sum */
   src_pixel = (float *)dest->data;
   dest_pixel = (float *)dest->data + width * comp;
   for (y = 1; y < height; y++)
   for (x = 0; x < width; x++) {

      for (c = 0; c < comp; c++)
         dest_pixel[c] += src_pixel[c];

      src_pixel += comp;
      dest_pixel += comp;
   }
}

MIAPI void m_image_convolution_h_raw(struct m_image *dest, const struct m_image *src, float *kernel, int size)
{
   float *src_data;
   float *dest_data;
   int radius = (size - 1) / 2;
   int width = src->width - radius * 2;
   int height = src->height;
   int comp = src->comp;
   int y, ystep, ystepc;

   assert(src->size > 0 && src->type == M_FLOAT);

   /* create destination images */
   m_image_create(dest, M_FLOAT, width, height, comp);
   
   /* clear */
   memset(dest->data, 0, dest->size * sizeof(float));

   src_data = (float *)src->data;
   dest_data = (float *)dest->data;
   ystep = width * comp;
   ystepc = src->width * comp;

   #pragma omp parallel for schedule(dynamic, 8)
   for (y = 0; y < height; y++) {

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
}

MIAPI void m_image_convolution_v_raw(struct m_image *dest, const struct m_image *src, float *kernel, int size)
{
   float *src_data;
   float *dest_data;
   int radius = (size - 1) / 2; 
   int width = src->width;
   int height = src->height - radius * 2;
   int comp = src->comp;

   int y, ystep;

   assert(src->size > 0 && src->type == M_FLOAT);

   /* create destination images */
   m_image_create(dest, M_FLOAT, width, height, comp);
   
   /* clear */
   memset(dest->data, 0, dest->size * sizeof(float));

   src_data = (float *)src->data;
   dest_data = (float *)dest->data;
   ystep = width * comp;

   #pragma omp parallel for schedule(dynamic, 8)
   for (y = 0; y < height; y++) {

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
}

MIAPI void m_image_convolution_h(struct m_image *dest, const struct m_image *src, float *kernel, int size)
{
   struct m_image mask = M_IMAGE_IDENTITY();
   struct m_image tmp = M_IMAGE_IDENTITY();
   float *destp;
   int radius = (size - 1) / 2;
   int x, y, c;

   assert(src->size > 0 && src->type == M_FLOAT);

   /* create source and destination images */
   m_image_reframe_zero(&tmp, src, radius, 0, radius, 0); /* apply clamped margin */
   m_image_convolution_h_raw(dest, &tmp, kernel, size);

   /* create gaussian mask */
   m_image_create(&tmp, M_FLOAT, src->width + radius * 2, 1, 1);
   for (x = 0; x < radius; x++)
      ((float *)tmp.data)[x] = 0;
   for (; x < (tmp.width - radius); x++)
      ((float *)tmp.data)[x] = 1;
   for (; x < tmp.width; x++)
      ((float *)tmp.data)[x] = 0; 
   m_image_convolution_h_raw(&mask, &tmp, kernel, size);

   /* mask */
   for (x = 0; x < mask.width; x++)
      ((float *)mask.data)[x] = 1.0f / ((float *)mask.data)[x]; 

   destp = (float *)dest->data;
   for (y = 0; y < dest->height; y++) {
      for (x = 0; x < dest->width; x++) {
         for (c = 0; c < dest->comp; c++)
            destp[c] *= ((float *)mask.data)[x];
         destp += dest->comp;
      }
   }

   m_image_destroy(&mask);
   m_image_destroy(&tmp);
}

MIAPI void m_image_convolution_v(struct m_image *dest, const struct m_image *src, float *kernel, int size)
{
   struct m_image tmp = M_IMAGE_IDENTITY();
   struct m_image mask = M_IMAGE_IDENTITY();
   float *destp;
   int radius = (size - 1) / 2;
   int x, y, c;

   assert(src->size > 0 && src->type == M_FLOAT);

   /* create source and destination images */
   m_image_reframe_zero(&tmp, src, 0, radius, 0, radius); /* apply clamped margin */
   m_image_convolution_v_raw(dest, &tmp, kernel, size);

   /* create gaussian mask */
   m_image_create(&tmp, M_FLOAT, 1, src->height + radius * 2, 1);
   for (y = 0; y < radius; y++)
      ((float *)tmp.data)[y] = 0;
   for (; y < (tmp.height - radius); y++)
      ((float *)tmp.data)[y] = 1;
   for (; y < tmp.height; y++)
      ((float *)tmp.data)[y] = 0; 
   m_image_convolution_v_raw(&mask, &tmp, kernel, size);

   /* mask */
   destp = (float *)dest->data;
   for (y = 0; y < dest->height; y++) {
      float idiv = 1.0f / ((float *)mask.data)[y];
      for (x = 0; x < dest->width; x++) {
         for (c = 0; c < dest->comp; c++)
            destp[c] *= idiv;
         destp += dest->comp;
      }
   }

   m_image_destroy(&mask);
   m_image_destroy(&tmp);
}

MIAPI void m_image_gaussian_blur(struct m_image *dest, const struct m_image *src, float dx, float dy)
{
   struct m_image tmp = M_IMAGE_IDENTITY();
   float *kernelx = NULL, *kernely = NULL;
   int kernelx_size = (int)(dx / 0.65f + 0.5f) * 2 + 1;
   int kernely_size = (int)(dy / 0.65f + 0.5f) * 2 + 1;
   
   assert(src->size > 0 && src->type == M_FLOAT);

   /* exit */
   if (dx < FLT_EPSILON && dy < FLT_EPSILON) {
      if (dest != src) m_image_copy(dest, src);
     return;
   }

   /* x blur */
   if (dx > 0) {
      kernelx = (float *)malloc(kernelx_size * sizeof(float));
      m_gaussian_kernel(kernelx, kernelx_size, dx);
      if (dy > 0)
         m_image_convolution_h(&tmp, src, kernelx, kernelx_size);
      else
         m_image_convolution_h(dest, src, kernelx, kernelx_size);
   }

   /* y blur */
   if (dy > 0) {
      kernely = (float *)malloc(kernely_size * sizeof(float));
      m_gaussian_kernel(kernely, kernely_size, dy);
      if (dx > 0)
         m_image_convolution_v(dest, &tmp, kernely, kernely_size);
      else
         m_image_convolution_v(dest, src, kernely, kernely_size);
   }

   m_image_destroy(&tmp);
   if(kernely) free(kernely);
   if(kernelx) free(kernelx);
}

MIAPI void m_image_grey(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_grey(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      float *src_pixel;
      float *dest_pixel;
      int size = src->size;
      int i, c = src->comp;

      assert(src->size > 0 && src->type == M_FLOAT && src->comp > 2);
      
      m_image_create(dest, M_FLOAT, src->width, src->height, 1);

      src_pixel = (float *)src->data;
      dest_pixel = (float *)dest->data;

      for (i = 0; i < size; i+=c) {
         float v = src_pixel[0] * 0.3f + src_pixel[1] * 0.5f + src_pixel[2] * 0.2f;
         *dest_pixel = v;
         dest_pixel++;
         src_pixel+=c;
      }
   }
}

MIAPI void m_image_max(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_max(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      float *src_pixel;
      float *dest_pixel;
      int size = src->size;
      int i, j, c = src->comp;

      assert(src->size > 0 && src->type == M_FLOAT);

      m_image_create(dest, M_FLOAT, src->width, src->height, 1);

      src_pixel = (float *)src->data;
      dest_pixel = (float *)dest->data;

      for (i = 0; i < size; i+=c) {
         float v = src_pixel[0];
         for (j = 1; j < c; j++)
            v = M_MAX(v, src_pixel[j]);
         *dest_pixel = v;
         dest_pixel++;
         src_pixel+=c;
      }
   }
}

MIAPI void m_image_max_abs(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m_image_max_abs(dest, &tmp);
      m_image_destroy(&tmp);
   }
   else {
      float *src_pixel;
      float *dest_pixel;
      int size = src->size;
      int i, j, c = src->comp;

      assert(src->size > 0 && src->type == M_FLOAT);

      m_image_create(dest, M_FLOAT, src->width, src->height, 1);

      src_pixel = (float *)src->data;
      dest_pixel = (float *)dest->data;

      for (i = 0; i < size; i+=c) {
         float v = fabsf(src_pixel[0]);
         for (j = 1; j < c; j++)
            v = M_MAX(v, fabsf(src_pixel[j]));
         *dest_pixel = v;
         dest_pixel++;
         src_pixel+=c;
      }
   }
}

static float m__convolve_pixel(float *data, int width, float *kernel)
{
   float sum = 0; int i, j;
   for (i = 0; i < 3; i++) {
      float *pixel = data + width * i;
      for (j = 0; j < 3; j++) {
         sum += (*pixel) * (*kernel);
         pixel++;
         kernel++;
      }
   }
   return sum;
}

MIAPI void m_image_sobel(struct m_image *dest, const struct m_image *src)
{
   struct m_image copy = M_IMAGE_IDENTITY();
   float ky[9] = {-1, -2, -1, 0, 0, 0, 1, 2, 1};
   float kx[9] = {-1, 0, 1, -2, 0, 2, -1, 0, 1};
   int width = src->width;
   int height = src->height;
   int w2 = width + 2;
   int y;
   
   assert(src->size > 0 && src->type == M_FLOAT && src->comp == 1);

   /* create source and destination images */
   m_image_reframe(&copy, src, 1, 1, 1, 1); /* apply clamped margin */
   m_image_create(dest, M_FLOAT, width, height, 2);
   
   #pragma omp parallel for schedule(dynamic, 8)
   for (y = 0; y < height; y++) {
     float * src_pixel = (float*)copy.data + y * w2;
     float * dest_pixel = (float*)dest->data + y * width * 2;
     int x;
      for (x = 0; x < width; x++) {
         dest_pixel[0] = m__convolve_pixel(src_pixel, w2, kx);
         dest_pixel[1] = m__convolve_pixel(src_pixel, w2, ky);
         src_pixel++;
         dest_pixel += 2;
      }
   }

   m_image_destroy(&copy);
}

MIAPI void m_image_harris(struct m_image *dest, const struct m_image *src, float radius)
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

#define M_WRITE_PIXEL(dest, x0, y0, v) {*(dest + w * (y0) + (x0)) = v;}
#define M_PUSH_PIXEL(x2, y2) if((stack_i+3) < stack_size && m__test_pixel(data, w, h, x2, y2, ref)) {\
   stack_i+=2;\
   stack[stack_i] = (uint16_t)(x2);\
   stack[stack_i+1] = (uint16_t)(y2);\
   M_WRITE_PIXEL(data, x2, y2, value);\
}

static int m__test_pixel(uint8_t *src, int w, int h, int x, int y, uint8_t ref)
{
   if (! (x >= 0 && x < w && y >= 0 && y < h))
      return 0;
   return (*(src + w * y + x) == ref);
}

MIAPI int m_image_floodfill_4x(struct m_image *dest, int x, int y, uint8_t ref, uint8_t value, uint16_t *stack, int stack_size)
{
   uint8_t *data = (uint8_t *)dest->data;
   int w = dest->width;
   int h = dest->height;
   int stack_i = 0;

   assert(dest->size > 0 && dest->type == M_UBYTE);

   if(! m__test_pixel(data, w, h, x, y, ref))
      return 0;

   stack[0] = (uint16_t)x;
   stack[1] = (uint16_t)y;
   M_WRITE_PIXEL(data, x, y, value);

   while (stack_i >= 0) {

      x = stack[stack_i];
      y = stack[stack_i+1];
      stack_i-=2;

      M_PUSH_PIXEL(x + 1, y)
      M_PUSH_PIXEL(x - 1, y)
      M_PUSH_PIXEL(x, y + 1)
      M_PUSH_PIXEL(x, y - 1)
   }

   return 1;
}

MIAPI int m_image_floodfill_8x(struct m_image *dest, int x, int y, uint8_t ref, uint8_t value, uint16_t *stack, int stack_size)
{
   uint8_t *data = (uint8_t *)dest->data;
   int w = dest->width;
   int h = dest->height;
   int stack_i = 0;

   assert(dest->size > 0 && dest->type == M_UBYTE);

   if(! m__test_pixel(data, w, h, x, y, ref))
      return 0;

   stack[0] = (uint16_t)x;
   stack[1] = (uint16_t)y;
   M_WRITE_PIXEL(data, x, y, value);

   while (stack_i >= 0) {

      x = stack[stack_i];
      y = stack[stack_i+1];
      stack_i-=2;

      M_PUSH_PIXEL(x + 1, y)
      M_PUSH_PIXEL(x - 1, y)
      M_PUSH_PIXEL(x, y + 1)
      M_PUSH_PIXEL(x, y - 1)
      M_PUSH_PIXEL(x + 1, y + 1)
      M_PUSH_PIXEL(x + 1, y - 1)
      M_PUSH_PIXEL(x - 1, y + 1)
      M_PUSH_PIXEL(x - 1, y - 1)
   }

   return 1;
}

#undef M_WRITE_PIXEL
#undef M_PUSH_PIXEL

static void m__dilate_erode(struct m_image *dest, const struct m_image *src, uint8_t ref, uint8_t value, int copy)
{
   uint8_t *src_data = (uint8_t *)src->data;
   uint8_t *src_pixel = src_data;
   uint8_t *dest_pixel;
   int w = src->width;
   int h = src->height;
   int y;

   assert(src->size > 0 && src->type == M_UBYTE);

   m_image_create(dest, M_UBYTE, w, h, 1);
   dest_pixel = (uint8_t *)dest->data;
   if (copy)
      memcpy(dest_pixel, src_data, dest->size * sizeof(char));
   else
      memset(dest_pixel, 0, dest->size * sizeof(char));

   for (y=0; y<h; y++) {

      int x;
      for (x=0; x<w; x++) {

         uint8_t c1, c2, c3, c4, c5;
         c1 = *src_pixel;

         if (c1 == ref) {
            c2 = x > 0 ? *(src_data + y * w + (x - 1)) : c1;
            c3 = y > 0 ? *(src_data + (y - 1) * w + x) : c1;
            c4 = (x + 1) < w ? *(src_data + y * w + x + 1) : c1;
            c5 = (y + 1) < h ? *(src_data + (y + 1) * w + x) : c1;
            if (c2 != c1 || c3 != c1 || c4 != c1 || c5 != c1)
               *dest_pixel = value;
         }

         src_pixel++;
         dest_pixel++;
      }
   }
}

MIAPI void m_image_dilate(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m__dilate_erode(dest, &tmp, 0, 255, 1);
      m_image_destroy(&tmp);
   }
   else {
      m__dilate_erode(dest, src, 0, 255, 1);
   }
}

MIAPI void m_image_erode(struct m_image *dest, const struct m_image *src)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m__dilate_erode(dest, &tmp, 255, 0, 1);
      m_image_destroy(&tmp);
   }
   else { 
      m__dilate_erode(dest, src, 255, 0, 1);
   }
}

MIAPI void m_image_edge_4x(struct m_image *dest, const struct m_image *src, uint8_t ref)
{
   if (dest == src) {
      struct m_image tmp = M_IMAGE_IDENTITY();
      m_image_copy(&tmp, src);
      m__dilate_erode(dest, &tmp, ref, 255, 0);
      m_image_destroy(&tmp);
   }
   else {
      m__dilate_erode(dest, src, ref, 255, 0);
   }
}

/* Following C code from the article
   "Efficient Binary Image Thinning using Neighborhood Maps"
   by Joseph M. Cychosz, in "Graphics Gems IV", Academic Press, 1994
   Thins the image using Rosenfeld's parallel thinning algorithm.
*/

/* Direction m__masks:
   N    S    W    E
*/
static int m__masks[] = {0200, 0002, 0040, 0010};

/* True if pixel neighbor map indicates the pixel is 8-simple and
   not an end point and thus can be deleted.  The neighborhood
   map is defined as an integer of bits abcdefghi with a non-zero
   bit representing a non-zero pixel.  The bit assignment for the
   neighborhood is:

            a b c
            d e f
            g h i
*/
static uint8_t m__delete_map[512] = {
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 1, 0, 0, 1, 1, 0, 1, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 1, 1, 1, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1,
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
      1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

MIAPI void m_image_thin(struct m_image *dest)
{
   uint8_t *data; /* image data */
   uint8_t ** ip; /* scanline pointers, ip[y][x] */
   uint8_t * qb;  /* Neighborhood maps of previous scanline */
   int xsize, ysize;    /* Image resolution */
   int x, y;            /* Pixel location */
   int i;               /* Pass index */
   int pc = 0;          /* Pass count */
   int count = 1;       /* Deleted pixel count */
   int p, q;            /* Neighborhood maps of adjacent cells */
   int m;               /* Deletion direction mask */

   assert(dest->size > 0 && dest->type == M_UBYTE);

   data = (uint8_t *)dest->data;
   xsize = dest->width;
   ysize = dest->height;

   qb = (uint8_t *)malloc(xsize * sizeof(char));
   qb[xsize-1] = 0; /* Used for lower-right pixel */

   /* alloc scanline pointers */
   ip = (uint8_t **)malloc(sizeof(void *) * ysize);
   
   /* set scanline pointers */
   for (y=0; y<ysize; y++) {
      ip[y] = data + y*xsize;
   }

   while (count) { /* Scan image while deletions */
   
      pc++;
      count = 0;

      for (i=0; i<4; i++) {

         m = m__masks[i];
         
         /* Build initial previous scan buffer */
         p = ip[0][0] != 0;
         for (x=0; x<xsize-1; x++) {
            p = ((p<<1)&0006) | (ip[0][x+1] != 0);
            qb[x] = (uint8_t)p;
         }
         
         /* Scan image for pixel deletion candidates */
         for (y=0; y<ysize-1; y++) {
            q = qb[0];
            p = ((q<<3)&0110) | (ip[y+1][0] != 0);
            
            for (x=0; x<xsize-1; x++) {
               q = qb[x];
               p = ((p<<1)&0666) | ((q<<3)&0110) | (ip[y+1][x+1] != 0);
               qb[x] = (uint8_t)p;

               if (((p&m) == 0) && m__delete_map[p]) {
                  if (ip[y][x] != 0) {
                     count++;
                     ip[y][x] = 0;
                  }
               }
            }
            
            /* Process right edge pixel */
            p = (p<<1)&0666;
            if ((p&m) == 0 && m__delete_map[p]) {
               if (ip[y][xsize-1] != 0) {
                  count++;
                  ip[y][xsize-1] = 0;
               }
            }
         }
         
         /* Process bottom scan line */
         for (x=0; x<xsize; x++) {
            q = qb[x];
            p = ((p<<1)&0666) | ((q<<3)&0110);
            if ((p&m) == 0 && m__delete_map[p]) {
               if (ip[ysize-1][x] != 0) {
                  count++;
                  ip[ysize-1][x] = 0;
               }
            }
         }
      }
   }

   free(qb);
   free(ip);
}

MIAPI void m_image_non_max_supp(struct m_image *dest, const struct m_image *src, int radius, float threshold)
{
   float *src_data, *dest_data;
   float *src_pixel, *dest_pixel;
   int width = src->width;
   int height = src->height;
   int x, y;

   assert(src->size > 0 && src->type == M_FLOAT && src->comp == 1);

   m_image_copy(dest, src);

   src_data = (float *)src->data;
   dest_data = (float *)dest->data;
   src_pixel = src_data;
   dest_pixel = dest_data;

   for (y = 0; y < height; y++)
   for (x = 0; x < width; x++) {

      int minx, miny, maxx, maxy, xx, yy;

      if (*src_pixel < threshold) {
         *dest_pixel = 0;
         goto end;
      }

      minx = M_MAX(0, x - radius);
      miny = M_MAX(0, y - radius);
      maxx = M_MIN(width - 1, x + radius);
      maxy = M_MIN(height - 1, y + radius);

      for (yy = miny; yy <= maxy; yy++)
      for (xx = minx; xx <= maxx; xx++) {

         float *src_pixel2 = src_data + yy*width + xx;
         if (*src_pixel2 > *src_pixel) {
            *dest_pixel = 0;
            goto end;
         }
      }

      end:
      src_pixel++;
      dest_pixel++;
   }
}

MIAPI int m_image_corner_harris(const struct m_image *src, int margin, float radius, float threshold, int *corners, int max_count)
{
   struct m_image harris = M_IMAGE_IDENTITY();
   struct m_image nms = M_IMAGE_IDENTITY();
   float *pixel;
   int width = src->width;
   int height = src->height;
   int wm = width - margin;
   int hm = height - margin;
   int x, y, count;

   if (width <= (margin * 2) || height <= (margin * 2))
      return 0;

   m_image_harris(&harris, src, radius);
   m_image_non_max_supp(&nms, &harris, (int)(radius) + 1, threshold);

   count = 0;
   pixel = (float *)nms.data;

   for (y = 0; y < height; y++)
   for (x = 0; x < width; x++) {

      if (count == max_count)
         goto end;

      if ((*pixel) > 0 && x >= margin && y >= margin && x < wm && y < hm) {
         corners[count*2]   = x;
         corners[count*2+1] = y;
         count++;
      }
      pixel++;
   }

   end:
   m_image_destroy(&nms);
   m_image_destroy(&harris);
   return count;
}

MIAPI void m_image_sub_pixel(const struct m_image *src, float x, float y, float *result)
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
   fx = M_MAX(fx, 0);
   fy = M_MAX(fy, 0);
   
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

/* slow TODO better */
static void m__bilinear(struct m_image *dest, const struct m_image *src, float dx, float dy, float offset)
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

MIAPI void m_image_pyrdown(struct m_image *dest, const struct m_image *src)
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

   m_image_gaussian_blur(&tmp, src, 1.5f, 1.5f);
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

MIAPI void m_image_resize(struct m_image *dest, const struct m_image *src, int new_width, int new_height)
{
   struct m_image tmp = M_IMAGE_IDENTITY();
   int width = src->width;
   int height = src->height;
   int comp = src->comp;
   float rx = (float)width / (float)new_width;
   float ry = (float)height / (float)new_height;

   assert(src->size > 0 && src->type == M_FLOAT);

   if (rx > 1.0f || ry > 1.0f) {
      m_image_gaussian_blur(&tmp, src, M_MAX(0.0f, rx - 1.0f), M_MAX(0.0f, ry - 1.0f));
      m_image_create(dest, M_FLOAT, new_width, new_height, comp);
      m__bilinear(dest, &tmp, rx, ry, -0.5f);
   }
   else {
      if (dest == src) {
         m_image_copy(&tmp, src);
         src = &tmp;
      }
      m_image_create(dest, M_FLOAT, new_width, new_height, comp);
      m__bilinear(dest, src, rx, ry, -0.5f);
   }

   m_image_destroy(&tmp);
}

#endif /* M_IMAGE_IMPLEMENTATION */
