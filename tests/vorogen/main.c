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

#define M_MATH_IMPLEMENTATION
#define M_IMAGE_IMPLEMENTATION
#define M_DIST_IMPLEMENTATION
#include <m_math.h>
#include <m_image.h>
#include <m_dist.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "../test.h"

#define POP_COUNT 128
#define POINT_COUNT 64
#define RANDF ((float)rand() / (float)RAND_MAX)

struct test_point
{
	float2 pos;
	float3 col;
};

static struct m_image test_buffer = M_IMAGE_IDENTITY();

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

static int get_best_dual(float *error, float *energy, int POP_SIZE, float p)
{
    float r = RANDF;
    int id1 = rand() % POP_SIZE;
    int id2 = rand() % POP_SIZE;
	int best = id1;
	
	if ((r > p) || (energy == NULL)) {
		if (error[id1] < error[id2])
			best = id1;
		else if(error[id1] > error[id2])
			best = id2;
	}
	else {
		if (! (error[id1] > error[id2] && energy[id1] > energy[id2])) {
			if(error[id1] < error[id2] || energy[id1] < energy[id2]) {
				best = id1;
			}
		}
		if (! (error[id2] > error[id1] && energy[id2] > energy[id1])) {
			if(error[id2] < error[id1] || energy[id2] < energy[id1]) {
				best = id2;
			}
		}
	}

	return best;
}

static int get_best_dual4(float *error, float *energy, int POP_SIZE, float p)
{
	float er4[4];
	float en4[4];
	float er2[2];
	float en2[2];
	int id4[4];
	int id2[2];
	int i;

	for (i = 0; i < 4; i++) {
		int best = get_best_dual(error, energy, POP_SIZE, p);
		id4[i] = best;
		er4[i] = error[best];
		en4[i] = energy[best];
	}

	for (i = 0; i < 2; i++) {
		int best = get_best_dual(er4, en4, 4, p);
		id2[i] = id4[best];
		er2[i] = er4[best];
		en2[i] = en4[best];
	}

	return id2[get_best_dual(er2, en2, 2, p)];
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
    		m_RGB_to_HSL(&pts[i].col.x, &pts[i].col.x);
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
            col.x += (RANDF - 0.5f) * s1*45;
            col.y += (RANDF - 0.5f) * s1;
            col.z += (RANDF - 0.5f) * s1;
            if (col.x >= 0 && col.x <= 360 && col.y >= 0 && col.y <= 1 && col.y >= 0 && col.y <= 1)
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
		m_RGB_to_HSL(&pts[i].col.x, &pts[i].col.x);
	} 
}

static float get_originality(int id)
{
    struct test_point *ip = population[id];
    float diff = 0;
    int i;
    
    for (i = 0; i < POP_COUNT; i++) {
        if (i != id) {
            int j;
            
            for (j = 0; j <POINT_COUNT; j++) {
             
                diff += fabs(ip[j].pos.x - population[i][j].pos.x) * 16.0f;
                diff += fabs(ip[j].pos.y - population[i][j].pos.y) * 16.0f;
                
                diff += fabs(ip[j].col.x - population[i][j].col.x) / 360.0f;
                diff += fabs(ip[j].col.y - population[i][j].col.y);
                diff += fabs(ip[j].col.z - population[i][j].col.z) * 2;
            }
        }
    }
    
    return diff;
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
		m_HSL_to_RGB(dest_data, dest_data);
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
    m_sRGB_to_linear((float*)src_image.data, (float*)src_image.data, src_image.size);
    
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
    float energy[POP_COUNT];
    float min_error;
    int i, best = 0;
    
    for (i = 0; i < POP_COUNT; i++) {
        render(&dest_image, population[i], POINT_COUNT);
        m_image_gaussian_blur(&dest_image, &dest_image, 1, 1);
        error[i] = get_score(&dest_image, &src_image);
        #ifdef SELECT_DUAL
        energy[i] = 1.0f / get_originality(i);
        #endif
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
        
        #ifdef SELECT_DUAL
        int i1 = get_best_dual4(error, energy, POP_COUNT, 0.2f);
		int i2 = get_best_dual4(error, energy, POP_COUNT, 0.2f);
		#else
        int i1 = get_best_tournament(error, POP_COUNT, 3);
		int i2 = get_best_tournament(error, POP_COUNT, 3);
		#endif
		
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
    test_swap_buffer(&test_buffer);
	test_update();
}

int main(int argc, char **argv)
{
    char dir[256];
    test_get_directory(dir, argv[0]);
    test_set_working_dir(dir);
    
	if (! test_create("M - Vorogen", 256, 256))
		return EXIT_FAILURE;

    m_image_create(&test_buffer, M_FLOAT, 256, 256, 3);
	init();

	#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
	#else
	while (test_state) {
		main_loop();
		thrd_yield();
	}
	#endif

    m_image_destroy(&test_buffer);
	clear();
	test_destroy();
	return EXIT_SUCCESS;
}
