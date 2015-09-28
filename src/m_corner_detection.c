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
#include <stdio.h>
#include <memory.h>
#include <assert.h>
#include <m_corner_detection.h>


void m_image_non_max_supp(struct m_image *dest, const struct m_image *src, int radius, float threshold)
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

int m_image_corner_harris(const struct m_image *src, int margin, int radius, float threshold, int *corners, int max_count)
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
   m_image_non_max_supp(&nms, &harris, radius, threshold);

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
