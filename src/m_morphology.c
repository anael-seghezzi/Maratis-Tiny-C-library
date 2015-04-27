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
#include <assert.h>
#include <m_morphology.h>


#define WRITE_PIXEL(dest, w, h, x, y, v) {*(dest + w * y + x) = v;}
#define PUSH_PIXEL(x2, y2) if((stack_i+3) < stack_size && _test_pixel(data, w, h, x2, y2, ref)) {\
	stack_i+=2;\
	stack[stack_i] = (unsigned short)x2;\
	stack[stack_i+1] = (unsigned short)y2;\
	WRITE_PIXEL(data, w, h, x2, y2, value);\
}

static int _test_pixel(unsigned char *src, int w, int h, int x, int y, unsigned char ref)
{
	if (! (x >= 0 && x < w && y >= 0 && y < h))
		return 0;
	return (*(src + w * y + x) == ref);
}

int m_image_floodfill_4x(struct m_image *dest, int x, int y, unsigned char ref, unsigned char value, unsigned short *stack, int stack_size)
{
	unsigned char *data = (unsigned char *)dest->data;
	int w = dest->width;
	int h = dest->height;
	int stack_i = 0;

	assert(dest->size > 0 && dest->type == M_UBYTE);

	if(! _test_pixel(data, w, h, x, y, ref))
		return 0;

	stack[0] = (unsigned short)x;
	stack[1] = (unsigned short)y;
	WRITE_PIXEL(data, w, h, x, y, value);

	while (stack_i >= 0) {

		x = stack[stack_i];
		y = stack[stack_i+1];
		stack_i-=2;

		PUSH_PIXEL(x + 1, y)
		PUSH_PIXEL(x - 1, y)
		PUSH_PIXEL(x, y + 1)
		PUSH_PIXEL(x, y - 1)
	}

	return 1;
}

int m_image_floodfill_8x(struct m_image *dest, int x, int y, unsigned char ref, unsigned char value, unsigned short *stack, int stack_size)
{
	unsigned char *data = (unsigned char *)dest->data;
	int w = dest->width;
	int h = dest->height;
	int stack_i = 0;

	assert(dest->size > 0 && dest->type == M_UBYTE);

	if(! _test_pixel(data, w, h, x, y, ref))
		return 0;

	stack[0] = (unsigned short)x;
	stack[1] = (unsigned short)y;
	WRITE_PIXEL(data, w, h, x, y, value);

	while (stack_i >= 0) {

		x = stack[stack_i];
		y = stack[stack_i+1];
		stack_i-=2;

		PUSH_PIXEL(x + 1, y)
		PUSH_PIXEL(x - 1, y)
		PUSH_PIXEL(x, y + 1)
		PUSH_PIXEL(x, y - 1)
		PUSH_PIXEL(x + 1, y + 1)
		PUSH_PIXEL(x + 1, y - 1)
		PUSH_PIXEL(x - 1, y + 1)
		PUSH_PIXEL(x - 1, y - 1)
	}

	return 1;
}

static void _dilate_erode(struct m_image *dest, struct m_image *src, unsigned char ref, unsigned char value, int copy)
{
	unsigned char *src_data = (unsigned char *)src->data;
	unsigned char *src_pixel = src_data;
	unsigned char *dest_pixel;
	int w = src->width;
	int h = src->height;
	int y;

	assert(src->size > 0 && src->type == M_UBYTE);

	m_image_create(dest, M_UBYTE, w, h, 1);
	dest_pixel = (unsigned char *)dest->data;
	if (copy)
		memcpy(dest_pixel, src_data, dest->size * sizeof(char));
	else
		memset(dest_pixel, 0, dest->size * sizeof(char));

	for (y=0; y<h; y++) {

		int x;
		for (x=0; x<w; x++) {

			unsigned char c1, c2, c3, c4, c5;
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

void m_image_dilate(struct m_image *dest, struct m_image *src)
{
	_dilate_erode(dest, src, 0, 255, 1);
}

void m_image_erode(struct m_image *dest, struct m_image *src)
{
	_dilate_erode(dest, src, 255, 0, 1);
}

void m_image_edge_4x(struct m_image *dest, struct m_image *src, unsigned char ref)
{
	_dilate_erode(dest, src, ref, 255, 0);
}

/* Following C code from the article
   "Efficient Binary Image Thinning using Neighborhood Maps"
   by Joseph M. Cychosz, in "Graphics Gems IV", Academic Press, 1994
   Thins the image using Rosenfeld's parallel thinning algorithm.
*/

/* Direction masks:
   N    S    W    E
*/
   static int masks[] = {0200, 0002, 0040, 0010};

/* True if pixel neighbor map indicates the pixel is 8-simple and
   not an end point and thus can be deleted.  The neighborhood
   map is defined as an integer of bits abcdefghi with a non-zero
   bit representing a non-zero pixel.  The bit assignment for the
   neighborhood is:

				a b c
				d e f
				g h i
*/
static unsigned char deleteMap[512] = {
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

void m_image_thin(struct m_image *dest)
{
	unsigned char *data;	/* image data */
	unsigned char ** ip;	/* scanline pointers, ip[y][x] */
	unsigned char * qb;		/* Neighborhood maps of previous scanline */
	int xsize, ysize;		/* Image resolution */
	int x, y;				/* Pixel location */
	int	i;					/* Pass index */
	int	pc = 0;				/* Pass count */
	int	count = 1;			/* Deleted pixel count */
	int	p, q;				/* Neighborhood maps of adjacent cells */
	int	m;					/* Deletion direction mask */

	assert(dest->size > 0 && dest->type == M_UBYTE);

	data = (unsigned char *)dest->data;
	xsize = dest->width;
	ysize = dest->height;

	qb = (unsigned char *)malloc(xsize * sizeof(char));
	qb[xsize-1] = 0; /* Used for lower-right pixel */

	/* alloc scanline pointers */
	ip = (unsigned char **)malloc(sizeof(void *) * ysize);
	
	/* set scanline pointers */
	for (y=0; y<ysize; y++) {
		ip[y] = data + y*xsize;
	}

	while (count) { /* Scan image while deletions */
	
	    pc++;
	    count = 0;

	    for (i=0; i<4; i++) {

			m = masks[i];
			
			/* Build initial previous scan buffer */
			p = ip[0][0] != 0;
			for (x=0; x<xsize-1; x++) {
				p = ((p<<1)&0006) | (ip[0][x+1] != 0);
				qb[x] = (unsigned char)p;
			}
			
			/* Scan image for pixel deletion candidates */
			for (y=0; y<ysize-1; y++) {
				q = qb[0];
				p = ((q<<3)&0110) | (ip[y+1][0] != 0);
				
				for (x=0; x<xsize-1; x++) {
					q = qb[x];
					p = ((p<<1)&0666) | ((q<<3)&0110) | (ip[y+1][x+1] != 0);
					qb[x] = (unsigned char)p;

					if (((p&m) == 0) && deleteMap[p]) {
						if (ip[y][x] != 0) {
							count++;
							ip[y][x] = 0;
						}
					}
				}
				
				/* Process right edge pixel */
				p = (p<<1)&0666;
				if ((p&m) == 0 && deleteMap[p]) {
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
				if ((p&m) == 0 && deleteMap[p]) {
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
