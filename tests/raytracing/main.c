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

/* raytracing test */

#include "../test.h"
#include <m_math.h>


#define GET_RAY(ray, px, py, pz, hw, hh, ratio)\
{\
	float3 pt = {((float)px - hw) / hw, (-((float)py - hh) / hh) * ratio, pz};\
	float ptl = 1.0f / M_LENGHT3(pt);\
	ray.x = pt.x * ptl;\
	ray.y = pt.y * ptl;\
	ray.z = pt.z * ptl;\
}

static void draw(void)
{
	float *data = (float *)test_buffer.data;
	int w = test_buffer.width;
	int h = test_buffer.height;
	int y;
	
	float3 sphere_pos;
	float3 light_dir;
	float z_near = 1e-4;
	float ambient = 0.15f;
	float sphere_radius2;
	float hw = w * 0.5f;
	float hh = h * 0.5f;
	float ratio = (float)test_height / (float)test_width;
	
	light_dir.x = 0.5f;
	light_dir.y = 0.25f;
	light_dir.z = -0.5f;
	
	sphere_radius2 = 100;
	sphere_pos.x = cosf(test_t * 0.025f) * 20.0f;
	sphere_pos.y = 0.0f;
	sphere_pos.z = 50.0f + (sinf(test_t * 0.025f) + 1.0f) * 50.0f;
	
	/* clear */
	memset(test_buffer.data, 0, test_buffer.size * sizeof(float));
	
	/* raytrace */
	#pragma omp parallel for schedule(dynamic, 8)
	for (y = 0; y < h; y++) {
		
		float *pixel = data + y * w * 3;
		int x;
		
		for (x = 0; x < w; x++) {
			
			float3 origin = {0, 0, 0};
			float3 ray;
			float dist, Z = 1e20;
			
			/* get ray from pixel position */
			GET_RAY(ray, x, y, 1.35f, hw, hh, ratio);
			
			/* sphere */
			dist = m_3d_ray_sphere_intersection(&origin, &ray, &sphere_pos, sphere_radius2);
			if (dist > z_near) {
				
				if (dist < Z) {
					
					float3 rd = {ray.x * dist, ray.y * dist, ray.z * dist};
					float3 pos = {origin.x + rd.x, origin.y + rd.y, origin.z + rd.z};
					float3 normal = {pos.x - sphere_pos.x, pos.y - sphere_pos.y, pos.z - sphere_pos.z};
					float diffuse;
					
					M_NORMALIZE3(normal, normal);
					diffuse = M_DOT3(normal, light_dir);
					diffuse = M_MAX(0, diffuse);

					Z = dist;
					pixel[0] = ambient + diffuse;
					pixel[1] = ambient + diffuse;
					pixel[2] = ambient + diffuse;
				}
			}
			
			pixel += 3;
		}
	}
}

int main(int argc, char **argv)
{	
	if (! test_create("M - RaytracingTest", 320, 180))
		return EXIT_FAILURE;
	
	while (test_state) {
		draw();
		test_update();
		thrd_yield();
	}
	
	test_destroy();
	return EXIT_SUCCESS;
}
