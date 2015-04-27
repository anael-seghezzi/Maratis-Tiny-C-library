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

#include <stdlib.h>
#include <memory.h>
#include <m_raster.h>


#define IS_IN_FRAME(x, y, w, h) (x >= 0 && x < w && y >= 0 && y < h)

#define FINDMINMAX(x0, x1, x2, min, max)\
	min = max = x0;\
	if (x1 < min) min=x1;\
	if (x1 > max) max=x1;\
	if (x2 < min) min=x2;\
	if (x2 > max) max=x2;


void m_raster_triangle_bbox_att4(struct m_image *dest, int minx, int miny, int maxx, int maxy, float4 *v0, float4 *v1, float4 *v2, float4 *a0, float4 *a1, float4 *a2)
{
	float *data = (float *)dest->data;
	float *yline;
	float4 up0, up1, up2;
	float denom, idenom;
	float t0_row, t1_row;
	float deltaX0, deltaX2, deltaY1, deltaY2;
	int w = dest->width;
	int w4, mx4, y;

	/* denominator */
	denom = (v1->x - v2->x) * (v2->y - v0->y) - (v1->y - v2->y) * (v2->x - v0->x);
	if (denom >= 0.0f)
		return;
	
	idenom = 1.0f / denom;

	/* scanline */
    t0_row = ((v2->x - v1->x) * (miny-v1->y) - (v2->y - v1->y) * (minx - v1->x)) * idenom;
    t1_row = ((v0->x - v2->x) * (miny-v2->y) - (v0->y - v2->y) * (minx - v2->x)) * idenom;
	
	deltaX0 = (v0->x - v2->x) * idenom;
	deltaX2 = (v2->x - v1->x) * idenom;
	deltaY1 = (v1->y - v2->y) * idenom;
	deltaY2 = (v2->y - v0->y) * idenom;

	up0.x = a0->x * v0->w; up0.y = a0->y * v0->w; up0.z = a0->z * v0->w; up0.w = a0->w * v0->w;
	up1.x = a1->x * v1->w; up1.y = a1->y * v1->w; up1.z = a1->z * v1->w; up1.w = a1->w * v1->w;
	up2.x = a2->x * v2->w; up2.y = a2->y * v2->w; up2.z = a2->z * v2->w; up2.w = a2->w * v2->w;

	w4 = w * 4;
	mx4 = minx * 4;
	
	yline = data + miny * w4;
	for (y = miny; y <= maxy; y++) {

		float *pixel = yline + mx4;
		float t0 = t0_row;
		float t1 = t1_row;
		int x;
		
		for (x = minx; x <= maxx; x++) {

			float t2 = t0 + t1;
			if (t0 >= 0.0f && t1 >= 0.0f && t2 <= 1.0f) {

				float it2 = 1.0f - t2;
				float w = 1.0f / (t0 * v0->w + t1 * v1->w + it2 * v2->w);

				pixel[0] = (up0.x * t0 + up1.x * t1 + up2.x * it2) * w;
				pixel[1] = (up0.y * t0 + up1.y * t1 + up2.y * it2) * w;
				pixel[2] = (up0.z * t0 + up1.z * t1 + up2.z * it2) * w;
				pixel[3] = (up0.w * t0 + up1.w * t1 + up2.w * it2) * w;
			}

			t0 += deltaY1;
			t1 += deltaY2;
			pixel += 4;
		}

		t0_row += deltaX2;
		t1_row += deltaX0;
		yline += w4;
    }
}

void m_raster_triangle_att4(struct m_image *dest, float4 *v0, float4 *v1, float4 *v2, float4 *a0, float4 *a1, float4 *a2)
{
	int minx, maxx, miny, maxy;
	int w = dest->width;
	int h = dest->height;

	if (v0->w <= 0.0f || v1->w <= 0.0f || v2->w <= 0.0f) /* no clip */
		return;

	/* bounding box */
	FINDMINMAX((int)v0->x, (int)v1->x, (int)v2->x, minx, maxx);
	FINDMINMAX((int)v0->y, (int)v1->y, (int)v2->y, miny, maxy);
	minx = M_MAX(minx, 0);
	miny = M_MAX(miny, 0);
	maxx = M_MIN(maxx, w - 1);
	maxy = M_MIN(maxy, h - 1);

	m_raster_triangle_bbox_att4(dest, minx, miny, maxx, maxy, v0, v1, v2, a0, a1, a2);
}

void m_raster_line(struct m_image *dest, float2 *p0, float2 *p1, float *color)
{
	float *data = (float *)dest->data;
	int x0 = (int)p0->x;
	int y0 = (int)p0->y;
	int x1 = (int)p1->x;
	int y1 = (int)p1->y;
	int w = dest->width;
	int h = dest->height;
	int comp = dest->comp;
	int dx =  M_ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
	int dy = -M_ABS(y1 - y0), sy = y0 < y1 ? 1 : -1;
	int err = dx + dy, e2;

	while (1) {

		if (IS_IN_FRAME(x0, y0, w, h)) { /* safe, but should be taken out of the loop for speed (clipping ?) */
			float *pixel = data + (y0 * w + x0) * comp; int c;
			for (c = 0; c < comp; c++)
				pixel[c] = color[c];
		}
		
		if (x0 == x1 && y0 == y1)
			break;
		
		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x0 += sx; }
		if (e2 <= dx) { err += dx; y0 += sy; }
	}
}

// Midpoint Circle Algorithm : http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
void m_raster_circle(struct m_image *dest, float2 *p, float r, float *color)
{
	#define PIXEL(px, py)\
	{\
		int _x = px;\
		int _y = py;\
		if (IS_IN_FRAME(_x, _y, w, h)) {\
			float *pixel = data + (_y * w + _x) * comp; int c;\
			for (c = 0; c < comp; c++)\
				pixel[c] = color[c];\
		}\
	}

	float *data = (float *)dest->data;
	int x0 = (int)p->x;
	int y0 = (int)p->y;
	int radius = (int)r;
	int w = dest->width;
	int h = dest->height;
	int comp = dest->comp;
	int x = radius, y = 0;
	int radiusError = 1 - x;
 
	while (x >= y) {

		PIXEL( x + x0,  y + y0)
		PIXEL( y + x0,  x + y0)
		PIXEL(-x + x0,  y + y0)
		PIXEL(-y + x0,  x + y0)
		PIXEL(-x + x0, -y + y0)
		PIXEL(-y + x0, -x + y0)
		PIXEL( x + x0, -y + y0)
		PIXEL( y + x0, -x + y0)
		y++;
        
		if (radiusError < 0)
			radiusError += 2 * y + 1;
        else {
			x--;
			radiusError += 2 * (y - x + 1);
        }
	}
}

/* adapted from : http://alienryderflex.com/polygon_fill/
   public-domain code by Darel Rex Finley, 2007
*/
void m_raster_polygon(struct m_image *dest, float2 *points, int count, float *color)
{
	float *data = (float *)dest->data;
	int *nodeX;
	int w = dest->width;
	int h = dest->height;
	int comp = dest->comp;
	int pixelY;
	int IMAGE_LEFT, IMAGE_RIGHT;
	int IMAGE_TOP, IMAGE_BOT;

	if (count < 3)
		return;
	
	nodeX = (int *)malloc(count * sizeof(int));
	
	/* bounding box */
	{
		float2 min, max; int i;
		min = max = points[0];
		for (i = 1; i < count; i++) {
			M_MIN2(min, min, points[i]);
			M_MAX2(max, max, points[i]);
		}
		IMAGE_LEFT = M_MAX(0, (int)min.x);
		IMAGE_TOP =  M_MAX(0, (int)min.y);
		IMAGE_RIGHT = M_MIN(w - 1, (int)max.x + 1);
		IMAGE_BOT =   M_MIN(h - 1, (int)max.y + 1);
	}

	/* loop through the rows of the image. */
	for (pixelY = IMAGE_TOP; pixelY < IMAGE_BOT; pixelY++) {

		/* build a list of nodes. */
		int nodes = 0;
		int i, j = count - 1;
		for (i = 0; i < count; i++) {
			if ((points[i].y < (float)pixelY && points[j].y >= (float)pixelY) ||
			    (points[j].y < (float)pixelY && points[i].y >= (float)pixelY))
				nodeX[nodes++] = (int)(points[i].x + (pixelY - points[i].y) / (points[j].y - points[i].y) * (points[j].x - points[i].x));
			j = i;
		}

		/*  sort the nodes, via a simple “Bubble” sort. */
		i = 0;
		while (i < (nodes - 1)) {
			if (nodeX[i] > nodeX[i + 1]) {
				int swap = nodeX[i]; nodeX[i] = nodeX[i + 1]; nodeX[i + 1] = swap; if (i) i--;
			}
			else i++;
		}

		/* fill the pixels between node pairs. */
		for (i = 0; i < nodes; i += 2) {

			if (nodeX[i] >= IMAGE_RIGHT)
				break;
				
			if (nodeX[i + 1] > IMAGE_LEFT) {

				float *pixel;
				nodeX[i] = M_MAX(nodeX[i], IMAGE_LEFT);
				nodeX[i + 1] = M_MIN(nodeX[i + 1], IMAGE_RIGHT);
				pixel = data + (pixelY * w + nodeX[i]) * comp;

				for (j = nodeX[i]; j < nodeX[i + 1]; j++) {
					int c;
					for (c = 0; c < comp; c++)
						pixel[c] = color[c];
					pixel += comp;
				}
			}
		}
	}
	
	free(nodeX);
}
