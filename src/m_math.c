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

#include <m_math.h>


unsigned int m_next_power_of_two(unsigned int x)
{
	if (x == 0)
		return 0;
	x--;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	x++;
	return x;
}

void m_normalize(float *dest, float *src, int size)
{
	float sum = 0.0f; int i;
	for(i = 0; i < size; i++)
		sum += src[i] * src[i];

	if (sum > 0.0f) {
		sum = 1.0f / sqrtf(sum);
		for(i = 0; i < size; i++)
			dest[i] = src[i] * sum;
	}
}

void m_normalize_sum(float *dest, float *src, int size)
{
	float sum = 0.0f; int i;
	for(i = 0; i < size; i++)
		sum += src[i];

	if (sum > 0.0f) {
		sum = 1.0f / sum;
		for(i = 0; i < size; i++)
			dest[i] = src[i] * sum;
	}
}

float m_mean(float *src, int size)
{
	float mean = 0; int i;
	for (i = 0; i < size; i++)
		mean += (*src++);
	return size > 0 ? mean / (float)size : 0;
}

float m_chi_squared_distance(float *src1, float *src2, int size)
{
	float sum1 = 0, sum2 = 0, score; int i;
	for (i = 0; i < size; i++) {
		sum1 += src1[i];
		sum2 += src2[i];
	}

	/* exit */
	if (sum1 == 0 || sum2 == 0)
		return 1;

	score = 0;
	for (i = 0; i < size; i++) {
		/* normalize */
		float val1 = src1[i] / sum1;
		float val2 = src2[i] / sum2;

		/* chi squared distance */
		if ((val1 + val2) > 0) {
			float x = val2 - val1;
			score += (x * x) / (val1 + val2);
		}
	}

	return score * 0.5f;
}

float m_convolution(float *src1, float *src2, int size)
{
	float c = 0; int i;
	for (i = 0; i < size; i++)
		c += src1[i] * src2[i];
	return c;
}

void m_gaussian_kernel(float *dest, int size)
{
	if(size == 3) {
		dest[0] = 0.25f;
		dest[1] = 0.50f;
		dest[2] = 0.25f;
	}
	else {

		float *k = dest;
		float sigma = 1.6f;
		float rs, s2;
		int radius = (size - 1) / 2;
		int i, r;
		
		s2 = 1.0f / expf(sigma * sigma * 2.25f);
		rs = sigma / (float)radius;

		/* compute gaussian kernel */
		for(r = -radius; r <= radius; r++) {
			float x = fabs((float)r * rs);
			float v = (1.0f / expf(x * x)) - s2;
			*k = v;
			k++;
		}

		/* normalize */
		m_normalize_sum(dest, dest, size);
	}
}

void m_sst(float *dest, float *src, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		float dx = src[0];
		float dy = src[1];
		dest[0] = dx*dx;
		dest[1] = dy*dy;
		dest[2] = dx*dy;
		src += 2;
		dest += 3;
	}
}

void m_harris_response(float *dest, float *src, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		float dx2 = src[0];
		float dy2 = src[1];
		float dxy = src[2];
		*dest = (dx2*dy2 - dxy*dxy) / (dx2 + dy2 + 1e-8f);
		src += 3;
		dest++;
	}
}

void m_tfm(float *dest, float *src, int count)
{
	int i;
	for (i = 0; i < count; i++) {
		float dx2 = src[0];
		float dy2 = src[1];
		float dxy = src[2];
		float lambda = 0.5 * (dy2 + dx2 + sqrtf(dy2*dy2 - 2.0*dx2*dy2 + dx2*dx2 + 4.0*dxy*dxy));
		dest[0] = dx2 - lambda;
		dest[1] = dxy;
		src += 3;
		dest += 2;
	}
}

float m_interpolation_cubic(float y0, float y1, float y2, float y3, float mu)
{
	float a0, a1, a2, a3, mu2;
	mu2 = mu * mu;
	a0 = y3 - y2 - y0 + y1;
	a1 = y0 - y1 - a0;
	a2 = y2 - y0;
	a3 = y1;
	return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

float m_interpolation_catmullrom(float y0, float y1, float y2, float y3, float mu)
{
	float a0, a1, a2, a3, mu2;
	mu2 = mu * mu;
	a0 = -0.5f * y0 + 1.5f * y1 - 1.5f * y2 + 0.5f * y3;
	a1 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3;
	a2 = -0.5f * y0 + 0.5f * y2;
	a3 = y1;
	return (a0 * mu * mu2 + a1 * mu2 + a2 * mu + a3);
}

float m_2d_polygon_area(float2 *points, int count)
{
	float fx, fy, a; int p;
	if (count < 3)
		return 0;

	fx = points[count - 1].x * points[0].y;
	fy = points[count - 1].y * points[0].x;

	for (p = 1; p < count; p++) {
		fx += points[p - 1].x * points[p].y;
		fy += points[p - 1].y * points[p].x;
	}

	a = (fx - fy) * 0.5f;
	return M_ABS(a);
}

float m_2d_polyline_length(float2 *points, int count)
{
	float l = 0; int p;
	for (p = 1; p < count; p++) {
		float2 v; M_SUB2(v, points[p], points[p - 1]);
		l += M_LENGHT2(v);
	}
	return l;
}

int m_2d_line_to_line_intersection(float2 *dest, float2 *p11, float2 *p12, float2 *p21, float2 *p22)
{
	float2 DP, QA, QB;
	float d, la, lb;

	DP.x = p21->x - p11->x ; DP.y = p21->y - p11->y;
	QA.x = p12->x - p11->x ; QA.y = p12->y - p11->y;
	QB.x = p22->x - p21->x ; QB.y = p22->y - p21->y;

	d = QA.y * QB.x - QB.y * QA.x;
	if (d == 0)
		return 0;
		
	la = (QB.x * DP.y - QB.y * DP.x) / d;
	if (la < 0 || la > 1)
		return 0;

	lb = (QA.x * DP.y - QA.y * DP.x) / d;
	if (lb < 0 || lb > 1)
		return 0;

	dest->x = p11->x + la * QA.x;
	dest->y = p11->y + la * QA.y;
	return 1;
}

int m_2d_box_to_box_collision(float2 *min1, float2 *max1, float2 *min2, float2 *max2)
{
	return (fabs(min1->x - min2->x) * 2 < ((max1->x - min1->x) + (max2->x - min2->x))) &&
           (fabs(min1->y - min2->y) * 2 < ((max1->y - min1->y) + (max2->y - min2->y)));
}

float m_3d_polyline_length(float3 *points, int count)
{
	float l = 0; int p;
	for (p = 1; p < count; p++) {
		float3 v; M_ADD3(v, points[p], points[p - 1]);
		l += M_LENGHT3(v);
	}
	return l;
}

int m_3d_box_to_box_collision(float3 *min1, float3 *max1, float3 *min2, float3 *max2)
{
	return (fabs(min1->x - min2->x) * 2 < ((max1->x - min1->x) + (max2->x - min2->x))) &&
           (fabs(min1->y - min2->y) * 2 < ((max1->y - min1->y) + (max2->y - min2->y))) &&
           (fabs(min1->z - min2->z) * 2 < ((max1->z - min1->z) + (max2->z - min2->z)));
}

int m_3d_ray_box_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max, float *in, float *out)
{
	float3 idir;
	float3 tmin, tmax;
	float3 tnear, tfar;
	float tnx, tny, tfy;
	float _in, _out;

	idir.x = 1.0f / ray_direction->x;
	idir.y = 1.0f / ray_direction->y;
	idir.z = 1.0f / ray_direction->z;
	tmin.x = (box_min->x - ray_origin->x) * idir.x;
	tmin.y = (box_min->y - ray_origin->y) * idir.y;
	tmin.z = (box_min->z - ray_origin->z) * idir.z;
	tmax.x = (box_max->x - ray_origin->x) * idir.x;
	tmax.y = (box_max->y - ray_origin->y) * idir.y;
	tmax.z = (box_max->z - ray_origin->z) * idir.z;

	M_MIN3(tnear, tmin, tmax);
	M_MAX3(tfar,  tmin, tmax);

	tnx = M_MAX(tnear.x, 0.0f);
	tny = M_MAX(tnear.y, tnear.z);
	tfy = M_MIN(tfar.y, tfar.z);

    _in = M_MAX(tnx, tny);
    _out = M_MIN(tfar.x, tfy);
    
    if (_out > 0.0f && _in < _out) {
		*in = _in;
		*out = _out;
		return 1;
	}
	return 0;
}

int m_3d_ray_sphere_intersection_in_out(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2, float *in, float *out)
{
	float3 vec;
	float b, det;

	M_SUB3(vec, *sphere_origin, *ray_origin);
	b = M_DOT3(vec, *ray_direction);
	det = b * b - M_DOT3(vec, vec) + sphere_radius2;

	if (det < 0) return 0;
	else det = sqrtf(det);
	*in = b - det;
	*out = b + det;
	return 1;
}

float m_3d_ray_plane_intersection(float3 *ray_origin, float3 *ray_direction, float3 *plane_origin, float3 *plane_normal)
{
	float constant = -M_DOT3(*plane_normal, *plane_origin);
	float normal_dot = M_DOT3(*plane_normal, *ray_direction);
	float plane_dist = M_DOT3(*plane_normal, *ray_origin) + constant;
	return -plane_dist / normal_dot;
}

float m_3d_ray_sphere_intersection(float3 *ray_origin, float3 *ray_direction, float3 *sphere_origin, float sphere_radius2)
{
	float3 vec;
	float b, det;

	M_SUB3(vec, *sphere_origin, *ray_origin);
	b = M_DOT3(vec, *ray_direction);
	det = b * b - M_DOT3(vec, vec) + sphere_radius2;

	if (det < 0) return 0;
	else det = sqrtf(det);
	return b - det;
}

/* Ray-box intersection using IEEE numerical properties to ensure that the
   test is both robust and efficient, as described in:
   Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
   "An Efficient and Robust Ray-Box Intersection Algorithm"
   Journal of graphics tools, 10(1):49-54, 2005
*/
float m_3d_ray_box_intersection(float3 *ray_origin, float3 *ray_direction, float3 *box_min, float3 *box_max)
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;
	float3 parameters[2];
	float3 inv_direction = {
		1 / ray_direction->x,
		1 / ray_direction->y,
		1 / ray_direction->z
	};
	int sign[3];

	parameters[0] = *box_min;
	parameters[1] = *box_max;

	sign[0] = (inv_direction.x < 0);
	sign[1] = (inv_direction.y < 0);
	sign[2] = (inv_direction.z < 0);

	tmin =  (parameters[sign[0]].x - ray_origin->x) * inv_direction.x;
	tymin = (parameters[sign[1]].y - ray_origin->y) * inv_direction.y;
	tmax =  (parameters[1 - sign[0]].x - ray_origin->x) * inv_direction.x;
	tymax = (parameters[1 - sign[1]].y - ray_origin->y) * inv_direction.y;
	
	if ((tmin > tymax) || (tymin > tmax))
		return 0;
  
	if (tymin > tmin) tmin = tymin;
	if (tymax < tmax) tmax = tymax;

	tzmin = (parameters[sign[2]].z - ray_origin->z) * inv_direction.z;
	tzmax = (parameters[1 - sign[2]].z - ray_origin->z) * inv_direction.z;

	if ((tmin > tzmax) || (tzmin > tmax))
		return 0;
		
	if (tzmin > tmin) tmin = tzmin;
	if (tzmax < tmax) tmax = tzmax;

	return tmin;
}

float m_3d_ray_triangle_intersection(float3 *ray_origin, float3 *ray_direction, float3 *vert1, float3 *vert2, float3 *vert3, float *u, float *v)
{
	float3 edge1, edge2;
	float3 pvec, tvec, qvec;
	float det, inv_det;

	M_SUB3(edge1, *vert2, *vert1);
	M_SUB3(edge2, *vert3, *vert1);
	M_CROSS3(pvec, *ray_direction, edge2);

	det = M_DOT3(edge1, pvec);
	if (det == 0)
		return 0;
		
	inv_det = 1.0f / det;
	M_SUB3(tvec, *ray_origin, *vert1);

	/* u parameter */
	*u = M_DOT3(tvec, pvec) * inv_det;
	if (*u < 0.0f || *u > 1.0f)
		return 0;
		
	M_CROSS3(qvec, tvec, edge1);
	
	/* v parameter */
	*v = M_DOT3(*ray_direction, qvec) * inv_det;
	
	/* inverted comparison (to catch NaNs) */
	if (*v >= 0.0f && ((*u) + (*v)) <= 1.0f)
		return M_DOT3(edge2, qvec) * inv_det;

	return 0;
}

/********************************************************/
/* AABB-triangle overlap test code                      */
/* by Tomas Akenine-Möller                              */
/* Function: int triBoxOverlap(float boxcenter[3],      */
/*          float boxhalfsize[3],float triverts[3][3]); */
/* History:                                             */
/*   2001-03-05: released the code in its first version */
/*   2001-06-18: changed the order of the tests, faster */
/*                                                      */
/* Acknowledgement: Many thanks to Pierre Terdiman for  */
/* suggestions and discussions on how to optimize code. */
/* Thanks to David Hunt for finding a ">="-bug!         */
/********************************************************/

#define FINDMINMAX(x0, x1, x2, min, max)\
	min = max = x0;\
	if (x1 < min) min=x1;\
	if (x1 > max) max=x1;\
	if (x2 < min) min=x2;\
	if (x2 > max) max=x2;

int _plane_box_overlap(float3 *normal, float3 *vert, float3 *maxbox)
{
	float3 vmin, vmax;
	if (normal->x > 0.0f) { vmin.x = -maxbox->x - vert->x; vmax.x =  maxbox->x - vert->x; }
	else                  { vmin.x =  maxbox->x - vert->x; vmax.x = -maxbox->x - vert->x; }
	if (normal->y > 0.0f) { vmin.y = -maxbox->y - vert->y; vmax.y =  maxbox->y - vert->y; }
	else                  { vmin.y =  maxbox->y - vert->y; vmax.y = -maxbox->y - vert->y; }
	if (normal->z > 0.0f) { vmin.z = -maxbox->z - vert->z; vmax.z =  maxbox->z - vert->z; }
	else                  { vmin.z =  maxbox->z - vert->z; vmax.z = -maxbox->z - vert->z; }
	if (M_DOT3(*normal, vmin) > 0.0f) return 0;
	if (M_DOT3(*normal, vmax) >= 0.0f) return 1;
	return 0;
}

/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)\
	p0 = a*v0.y - b*v0.z;\
	p2 = a*v2.y - b*v2.z;\
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}\
	rad = fa * boxhalfsize->y + fb * boxhalfsize->z;\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)\
	p0 = a*v0.y - b*v0.z;\
	p1 = a*v1.y - b*v1.z;\
	if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}\
	rad = fa * boxhalfsize->y + fb * boxhalfsize->z;\
	if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)\
	p0 = -a*v0.x + b*v0.z;\
	p2 = -a*v2.x + b*v2.z;\
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;}\
	rad = fa * boxhalfsize->x + fb * boxhalfsize->z;\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)\
	p0 = -a*v0.x + b*v0.z;\
	p1 = -a*v1.x + b*v1.z;\
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}\
	rad = fa * boxhalfsize->x + fb * boxhalfsize->z;\
	if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/
#define AXISTEST_Z12(a, b, fa, fb)\
	p1 = a*v1.x - b*v1.y;\
	p2 = a*v2.x - b*v2.y;\
    if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;}\
	rad = fa * boxhalfsize->x + fb * boxhalfsize->y;\
	if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)\
	p0 = a*v0.x - b*v0.y;\
	p1 = a*v1.x - b*v1.y;\
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;}\
	rad = fa * boxhalfsize->x + fb * boxhalfsize->y;\
	if(min>rad || max<-rad) return 0;

int m_3d_tri_box_overlap(float3 *boxcenter, float3 *boxhalfsize, float3 *vert1, float3 *vert2, float3 *vert3)
{
	/*    use separating axis theorem to test overlap between triangle and box */
	/*    need to test for overlap in these directions: */
	/*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
	/*       we do not even need to test these) */
	/*    2) normal of the triangle */
	/*    3) crossproduct(edge from tri, {x,y,z}-directin) */
	/*       this gives 3x3=9 more tests */

	float3 v0, v1, v2;
	float3 normal, e0, e1, e2;
	float min, max, p0, p1, p2, rad, fex, fey, fez;

	/* This is the fastest branch on Sun */
	/* move everything so that the boxcenter is in (0,0,0) */
	M_SUB3(v0, *vert1, *boxcenter);
	M_SUB3(v1, *vert2, *boxcenter);
	M_SUB3(v2, *vert3, *boxcenter);

	/* compute triangle edges */
	M_SUB3(e0, v1, v0);      /* tri edge 0 */
	M_SUB3(e1, v2, v1);      /* tri edge 1 */
	M_SUB3(e2, v0, v2);      /* tri edge 2 */

	/* Bullet 3:  */
	/*  test the 9 tests first (this was faster) */
	fex = fabsf(e0.x);
	fey = fabsf(e0.y);
	fez = fabsf(e0.z);
	AXISTEST_X01(e0.z, e0.y, fez, fey);
	AXISTEST_Y02(e0.z, e0.x, fez, fex);
	AXISTEST_Z12(e0.y, e0.x, fey, fex);

	fex = fabsf(e1.x);
	fey = fabsf(e1.y);
	fez = fabsf(e1.z);
	AXISTEST_X01(e1.z, e1.y, fez, fey);
	AXISTEST_Y02(e1.z, e1.x, fez, fex);
	AXISTEST_Z0(e1.y, e1.x, fey, fex);

	fex = fabsf(e2.x);
	fey = fabsf(e2.y);
	fez = fabsf(e2.z);
	AXISTEST_X2(e2.z, e2.y, fez, fey);
	AXISTEST_Y1(e2.z, e2.x, fez, fex);
	AXISTEST_Z12(e2.y, e2.x, fey, fex);

	/* Bullet 1: */
	/*  first test overlap in the {x,y,z}-directions */
	/*  find min, max of the triangle each direction, and test for overlap in */
	/*  that direction -- this is equivalent to testing a minimal AABB around */
	/*  the triangle against the AABB */

	/* test in X-direction */
	FINDMINMAX(v0.x, v1.x, v2.x, min, max);
	if (min > boxhalfsize->x || max < -boxhalfsize->x) return 0;

	/* test in Y-direction */
	FINDMINMAX(v0.y, v1.y, v2.y, min, max);
	if (min > boxhalfsize->y || max < -boxhalfsize->y) return 0;

	/* test in Z-direction */
	FINDMINMAX(v0.z, v1.z, v2.z, min, max);
	if (min > boxhalfsize->z || max < -boxhalfsize->z) return 0;

	/* Bullet 2: */
	/*  test if the box intersects the plane of the triangle */
	/*  compute plane equation of triangle: normal*x+d=0 */
	M_CROSS3(normal, e0, e1);
	if (! _plane_box_overlap(&normal, &v0, boxhalfsize)) return 0;

	return 1;   /* box and triangle overlaps */
}
