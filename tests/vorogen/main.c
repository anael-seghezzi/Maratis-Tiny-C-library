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

/* voronoi genetic algorithm painting */

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../test.h"
#include <m_math.h>
#include <m_dist.h>
#include <m_image_filter.h>
#include <m_image_resize.h>

#define POP_COUNT 128
#define POINT_COUNT 128
#define RANDF ((float)rand() / (float)RAND_MAX)

struct test_point
{
	float2 pos;
	float3 col;
};

static struct test_point population[POP_COUNT][POINT_COUNT];
static struct test_point population_copy[POP_COUNT][POINT_COUNT];

static struct m_image tmp_buffer = M_IMAGE_IDENTITY();
static struct m_image tmpi = M_IMAGE_IDENTITY();
static struct m_image src_image = M_IMAGE_IDENTITY();
static struct m_image dest_image = M_IMAGE_IDENTITY();


static int load_image(struct m_image *dest, const char *filename)
{
    int w, h, n;
    unsigned char *data = stbi_load(filename, &w, &h, &n, 0);
    if (data == NULL)
    {
        printf("unable to read image\n");
        return 0;
    }

    m_image_destroy(dest);
    dest->data = data;
    dest->size = w * h * n;
    dest->width = w;
    dest->height = h;
    dest->comp = n;
    dest->type = M_UBYTE;
    return 1;
}

static int get_best_tournament(float *error, int count, int tour_size)
{
	int best = rand()%count; int i;
	
	for (i = 1; i < tour_size; i++) {
		int id = rand()%count;
		if (error[id] < error[best])
			best = id;
	}
	
	return best;
}

static void mate(float *data, float *src1, float *src2, int s, int c)
{
    int i;
    
	for (i = 0; i < s; i+=c) {
	   int j;
	   
	   if (RANDF < 0.5f) {
	       for (j = 0; j < c; j++)
	           data[j] = src1[j];
	   }
	   else {
	       for (j = 0; j < c; j++)
	           data[j] = src2[j];
       }
       
	   data += c;
	   src1 += c;
	   src2 += c;
    }
}

static void mutate(struct test_point *pts, int size, struct m_image *src)
{
    float p = 2.0f / (float)size;
    float s1 = 0.015f;
    float s2 = 0.035f;
    int w = src->width;
    int h = src->height;
    int wh = w*h;
    int i;
    
    for (i = 0; i < size; i++) {
        
        if (RANDF < p*0.01f) {
            pts[i].pos.x = RANDF;
            pts[i].pos.y = RANDF;
        }
        
        if (RANDF < p*0.01f) {
            int id = rand() % wh;
            float *src_pixel = (float *)src->data + id * src->comp;
    		pts[i].col.x = src_pixel[0];
    		pts[i].col.y = src_pixel[1];
    		pts[i].col.z = src_pixel[2];
        }
        
        if (RANDF < p) {
            float2 pos = pts[i].pos;
            pos.x += (RANDF - 0.5f) * s2;
            pos.y += (RANDF - 0.5f) * s2;
            if (pos.x > 0 && pos.x < 1 && pos.y > 0 && pos.y < 1)
                pts[i].pos = pos;
        }
        
        if (RANDF < p) {
            float3 col = pts[i].col;
            col.x += (RANDF - 0.5f) * s1;
            col.y += (RANDF - 0.5f) * s1;
            col.z += (RANDF - 0.5f) * s1;
            if (col.x >= 0 && col.x <= 1 && col.y >= 0 && col.y <= 1 && col.y >= 0 && col.y <= 1)
                pts[i].col = col;
        }
    }
}

static void gen(struct test_point *pts, struct m_image *src)
{
    float spt = sqrtf((float)POINT_COUNT);
    int w = src->width;
    int h = src->height;
    int i;
    
    /* random points */
	for (i = 0; i < POINT_COUNT; i++) {
		//pts[i].pos.x = RANDF;
		//pts[i].pos.y = RANDF;
		float y = i / spt;
		float x = i - (int)y * spt;
		pts[i].pos.x = ((x + 0.5f) / spt) + RANDF * 0.2f - 0.1f;
		pts[i].pos.y = ((y + 0.5f) / spt) + RANDF * 0.2f - 0.1f;
		pts[i].pos.x = M_CLAMP(pts[i].pos.x, 0, 1);
		pts[i].pos.y = M_CLAMP(pts[i].pos.y, 0, 1);
	}
	
	/* pick initial color */
	for (i = 0; i < POINT_COUNT; i++) {
		int x = (int)(pts[i].pos.x * (w - 1));
		int y = (int)(pts[i].pos.y * (h - 1));
		float *src_pixel = (float *)src->data + (y * w + x) * src->comp;
		pts[i].col.x = src_pixel[0];
		pts[i].col.y = src_pixel[1];
		pts[i].col.z = src_pixel[2];
	} 
}

static float get_score(struct m_image *a, struct m_image *b)
{
    return m_chi_squared_distance((float *)a->data, (float *)b->data, a->size);
    //return m_squared_distance((float *)a->data, (float *)b->data, a->size);
}

static void render(struct m_image *dest, struct test_point *pts, int pts_count)
{
    float *tmp_data;
	float *dest_data;
	int w = dest->width;
	int h = dest->height;
	int size = w * h;
	int i;

	m_image_create(&tmp_buffer, M_FLOAT, w, h, 1);
	tmp_data = (float *)tmp_buffer.data;

	for (i = 0; i < size; i++)
		tmp_data[i] = M_DIST_MAX;

	for (i = 0; i < pts_count; i++) {

		/* draw point */
		int x = (int)(pts[i].pos.x * (w - 1));
		int y = (int)(pts[i].pos.y * (h - 1));

		/* mask */
		tmp_data[y * w + x] = 0;

		/* color */
		dest_data = (float *)dest->data + (y * w + x) * dest->comp;
		dest_data[0] = pts[i].col.x;
		dest_data[1] = pts[i].col.y;
		dest_data[2] = pts[i].col.z;
	}

	m_image_voronoi_transform(&tmp_buffer, &tmpi, &tmp_buffer);
	m_image_voronoi_fill(dest, dest, &tmpi);    
}

static void init(void)
{
    struct m_image ubi = M_IMAGE_IDENTITY();
    int i;
    
    if ((! load_image(&ubi, "dataset/lena.tga")) || (ubi.comp != 3))
        exit(EXIT_FAILURE);
   
    m_image_ubyte_to_float(&src_image, &ubi);
    
    m_image_copy(&ubi, &src_image);
    m_image_resize(&src_image, &ubi, 128, 128);
    //m_image_gaussian_blur(&src_image, &src_image, 1, 1);
    
    m_image_create(&dest_image, M_FLOAT, src_image.width, src_image.height, 3);
    
    for (i = 0; i < POP_COUNT; i++)
        gen(population[i], &src_image);
    
    m_image_destroy(&ubi);
}

static void clear(void)
{
    m_image_destroy(&src_image);
	m_image_destroy(&tmp_buffer);
	m_image_destroy(&tmpi);
}

static void draw(void)
{
    float error[POP_COUNT];
    float min_error;
    int i, best = 0;
    
    for (i = 0; i < POP_COUNT; i++) {
        render(&dest_image, population[i], POINT_COUNT);
        m_image_gaussian_blur(&dest_image, &dest_image, 1, 1);
        error[i] = get_score(&dest_image, &src_image);
    }
    
    min_error = error[best];
    for (i = 1; i < POP_COUNT; i++) {
        if (error[i] < min_error) {
            min_error = error[i];
            best = i;
        }
    }
    
    render(&test_buffer, population[best], POINT_COUNT);
    m_image_gaussian_blur(&test_buffer, &test_buffer, 2, 2);
    
    /* copy */
    memcpy(population_copy, population, sizeof(population));
    
    /* mate */
    for (i = 0; i < POP_COUNT; i++) {
        int i1 = get_best_tournament(error, POP_COUNT, 3);
		int i2 = get_best_tournament(error, POP_COUNT, 3);
		mate(
			&population[i][0].pos.x,
			&population_copy[i1][0].pos.x,
			&population_copy[i2][0].pos.x,
			POINT_COUNT*5, 5
			);
    }
    
    /* mutate */
    for (i = 0; i < POP_COUNT; i++)
        mutate(population[i], POINT_COUNT, &src_image);
    
    printf("score %f\n", min_error);
}

void main_loop(void)
{	
	draw();
	test_update();
}

int main(int argc, char **argv)
{	
    char dir[256];
    test_get_directory(dir, argv[0]);
    test_set_working_dir(dir);
    
	if (! test_create("M - Vorogen", 256, 256))
		return EXIT_FAILURE;

	init();

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (test_state) {
		main_loop();
		thrd_yield();
	}
	#endif

	clear();
	test_destroy();
	return EXIT_SUCCESS;
}
