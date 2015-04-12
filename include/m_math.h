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

#ifndef _M_MATH_H_
#define _M_MATH_H_

#ifndef __OPENCL_VERSION__
#include <math.h>
#endif
#include "m_base.h"

/* basic math */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#define M_DEG_TO_RAD 0.01745329251994329576
#define M_RAD_TO_DEG 57.29577951308232087679

#define M_MIN(a, b) (((a) < (b)) ? (a) : (b))
#define M_MAX(a, b) (((a) > (b)) ? (a) : (b))
#define M_CLAMP(x, low, high) (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define M_ABS(a) (((a) < 0) ? -(a) : (a))
#define M_ISPOWEROFTWO(x) (((x&(x - 1)) == 0) && (x != 0))
#define M_SHUFFLE(data, size, T) {\
	T *ptr = (T *)(data); int i;\
	for (i = 0; i < size; i++) {\
		int ri = rand() % size;\
		T tmp = ptr[i]; ptr[i] = ptr[ri]; ptr[ri] = tmp;\
	}\
}

M_API unsigned int m_next_power_of_two(unsigned int x);

/* float3 */
typedef struct {float x, y, z;} float3;

/* float2, float4 */
#ifndef __OPENCL_VERSION__
typedef struct {float x, y;} float2;
typedef struct {float x, y, z, w;} float4;
#endif

/* vector math */
#ifndef __OPENCL_VERSION__
#define M_DOT2(A, B) ((A).x * (B).x + (A).y * (B).y)
#define M_DOT3(A, B) ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z)
#define M_DOT4(A, B) ((A).x * (B).x + (A).y * (B).y + (A).z * (B).z + (A).w * (B).w)
#define M_LENGHT2(src) sqrtf((src).x * (src).x + (src).y * (src).y)
#define M_LENGHT3(src) sqrtf((src).x * (src).x + (src).y * (src).y + (src).z * (src).z)
#define M_LENGHT4(src) sqrtf((src).x * (src).x + (src).y * (src).y + (src).z * (src).z + (src).w * (src).w)
#define M_MIN2(dest, A, B) (dest).x = M_MIN((A).x, (B).x); (dest).y = M_MIN((A).y, (B).y);
#define M_MIN3(dest, A, B) M_MIN2(dest, A, B); (dest).z = M_MIN((A).z, (B).z);
#define M_MIN4(dest, A, B) M_MIN3(dest, A, B); (dest).w = M_MIN((A).w, (B).w);
#define M_MAX2(dest, A, B) (dest).x = M_MAX((A).x, (B).x); (dest).y = M_MAX((A).y, (B).y);
#define M_MAX3(dest, A, B) M_MAX2(dest, A, B); (dest).z = M_MAX((A).z, (B).z);
#define M_MAX4(dest, A, B) M_MAX3(dest, A, B); (dest).w = M_MAX((A).w, (B).w);
#define M_NORMALIZE2(dest, src){\
	float l = M_LENGHT2(src);\
	if (l > 0) { l = 1.0f / l; (dest).x = (src).x * l; (dest).y = (src).y * l; }\
	else { (dest).x = (dest).y = 0.0f; }\
}
#define M_NORMALIZE4(dest, src){\
	float l = M_LENGHT4(src);\
	if (l > 0) { l = 1.0f / l; (dest).x = (src).x * l; (dest).y = (src).y * l; (dest).z = (src).z * l; (dest).w = (src).w * l; }\
	else { (dest).x = (dest).y = (dest).z = (dest).w = 0.0f; }\
}
#else
#define M_DOT2(A, B) dot((A).xy, (B).xy)
#define M_DOT3(A, B) dot((float4)((A).x, (A).y, (A).z, 0), (float4)((B).x, (B).y, (B).z, 0.0f))
#define M_DOT4(A, B) dot(A, B)
#define M_LENGHT2(src) length(src)
#define M_LENGHT3(src) length((float4)((src).x, (src).y, (src).z, 0.0f))
#define M_LENGHT4(src) length(src)
#define M_MIN2(dest, A, B) (dest).xy = min(A.xy, B.xy);
#define M_MIN3(dest, A, B) (dest).x = M_MIN((A).x, (B).x); (dest).y = M_MIN((A).y, (B).y); (dest).z = M_MIN((A).z, (B).z);
#define M_MIN4(dest, A, B) (dest) = min(A, B);
#define M_MAX2(dest, A, B) (dest).xy = max(A.xy, B.xy);
#define M_MAX3(dest, A, B) (dest).x = M_MAX((A).x, (B).x); (dest).y = M_MAX((A).y, (B).y); (dest).z = M_MAX((A).z, (B).z);
#define M_MAX4(dest, A, B) (dest) = max(A, B);
#define M_NORMALIZE2(dest, src) (dest).xy = normalize((src).xy);
#define M_NORMALIZE4(dest, src) (dest) = normalize(src);
#endif

#define M_NORMALIZE3(dest, src){\
	float l = M_LENGHT3(src);\
	if (l > 0) { l = 1.0f / l; (dest).x = (src).x * l; (dest).y = (src).y * l; (dest).z = (src).z * l; }\
	else { (dest).x = (dest).y = (dest).z = 0.0f; }\
}

#define M_CROSS2(A, B) ((A).x * (B).y - (A).y * (B).x)
#define M_CROSS3(dest, A, B) (dest).x = (A).y * (B).z - (A).z * (B).y; (dest).y = (A).z * (B).x - (A).x * (B).z; (dest).z = (A).x * (B).y - (A).y * (B).x;

#define M_ADD2(dest, A, B) (dest).x = (A).x + (B).x; (dest).y = (A).y + (B).y;
#define M_ADD3(dest, A, B) M_ADD2(dest, A, B) (dest).z = (A).z + (B).z;
#define M_ADD4(dest, A, B) M_ADD3(dest, A, B) (dest).w = (A).w + (B).w;

#define M_SUB2(dest, A, B) (dest).x = (A).x - (B).x; (dest).y = (A).y - (B).y;
#define M_SUB3(dest, A, B) M_SUB2(dest, A, B) (dest).z = (A).z - (B).z;
#define M_SUB4(dest, A, B) M_SUB3(dest, A, B) (dest).w = (A).w - (B).w;

#define M_MUL2(dest, A, B) (dest).x = (A).x * (B).x; (dest).y = (A).y * (B).y;
#define M_MUL3(dest, A, B) M_MUL2(dest, A, B) (dest).z = (A).z * (B).z;
#define M_MUL4(dest, A, B) M_MUL3(dest, A, B) (dest).w = (A).w * (B).w;

#define M_DIV2(dest, A, B) (dest).x = (A).x / (B).x; (dest).y = (A).y / (B).y;
#define M_DIV3(dest, A, B) M_DIV2(dest, A, B) (dest).z = (A).z / (B).z;
#define M_DIV4(dest, A, B) M_DIV3(dest, A, B) (dest).w = (A).w / (B).w;

/* float array utils */
M_API void m_normalize(float *dest, float *src, int size); /* dest = src / norm(src) */
M_API void m_normalize_sum(float *dest, float *src, int size); /* dest = src / sum(src) */
M_API float m_mean(float *src, int size);
M_API float m_convolution(float *src1, float *src2, int size); /* a dot product really */
M_API float m_chi_squared_distance(float *src1, float *src2, int size); /* good at estimating signed hystograms difference */

/* convolution kernel */
M_API void m_gaussian_kernel(float *dest, int size);

/* structure tensor field and harris response */
M_API void m_sst(float *dest, float *src, int count); /* compute a 3 component smooth struture tensor from a 2 component vector */
M_API void m_harris_response(float *dest, float *src, int count); /* compute Harris response (1 component) from a 3 component sst */
M_API void m_tfm(float *dest, float *src, int count); /* compute a 2 component vector from a 3 component sst */

/* interpolation */
M_API float m_interpolation_cubic(float y0, float y1, float y2, float y3, float mu);
M_API float m_interpolation_catmullrom(float y0, float y1, float y2, float y3, float mu);

/* 2d */
M_API int m_2d_line_to_line_intersection(float2 *dest, float2 *p11, float2 *p12, float2 *p21, float2 *p22);
M_API int m_2d_box_to_box_collision(float2 *min1, float2 *max1, float2 *min2, float2 *max2);
M_API float m_2d_polygon_area(float2 *points, int count);
M_API float m_2d_polyline_length(float2 *points, int count);

/* 3d */
M_API int m_3d_box_to_box_collision(float3 *min1, float3 *max1, float3 *min2, float3 *max2);
M_API int m_3d_ray_box_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max, float *in, float *out);
M_API int m_3d_tri_box_overlap(float3 *box_center, float3 *box_half_size, float3 *vert1, float3 *vert2, float3 *vert3);
M_API float m_3d_polyline_length(float3 *points, int count);
M_API float m_3d_ray_plane_intersection(float3 *ray_origin, float3 *ray_direction, float3 *plane_origin, float3 *plane_normal);
M_API float m_3d_ray_sphere_intersection(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2);
M_API float m_3d_ray_box_intersection(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max);
M_API float m_3d_ray_triangle_intersection(float3 *ray_origin, float3 *ray_direction, float3 *vert1, float3 *vert2, float3 *vert3, float *u, float *v);

#endif
