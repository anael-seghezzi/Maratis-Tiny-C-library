Maratis Tiny C library
======================

is a collection of small and efficient math and image processing routines written in ANSI C with no dependencies.

The library is divided in independent single files (stb style):
- [m_math.h] (https://github.com/anael-seghezzi/Maratis-Tiny-C-library/blob/master/include/m_math.h)
- [m_image.h] (https://github.com/anael-seghezzi/Maratis-Tiny-C-library/blob/master/include/m_image.h)
- [m_raster.h] (https://github.com/anael-seghezzi/Maratis-Tiny-C-library/blob/master/include/m_raster.h)
- [m_dist.h] (https://github.com/anael-seghezzi/Maratis-Tiny-C-library/blob/master/include/m_dist.h)
- [m_color.h] (https://github.com/anael-seghezzi/Maratis-Tiny-C-library/blob/master/include/m_color.h)
- [m_path_finding.h] (https://github.com/anael-seghezzi/Maratis-Tiny-C-library/blob/master/include/m_path_finding.h)

Math
----

* vector manipulation
* interpolation (cubic, catmullrom)
* quaternion (basics, slerp...)
* matrix (projection, transformation...)
* random number generator
* 2d routines
* 3d routines
  * voxeliser (tri-box overlap)
  * raytracing (sphere, plane, box, triangle)

Image manipulation
------------------

* ubyte, ushort, int, half, float...
* copy, conversions, mirror, reframe, rotate...
* filters (convolution, gaussian blur, sobel, harris)
* resizing, pyrdown
* morphology (floodfill, dilate, erode, thinning...)
* corner detection (harris, non-maxima suppression)

Rasterization
-------------

* triangle with interpolation (perspective correct)
* basic line, circle and polygon

Distance map
------------
* distance transform and voronoi

Color conversion
----------------

* RGB, HSV, HSL
* sRGB / linear

Path finding
------------

* path finding on regular grid (floodfill-based)

Asm.js demos
------------

* [m_raster] (http://maratis3d.org/js/m_raster/m_raster.html)
* [m_voronoi] (http://maratis3d.org/js/m_voronoi/m_voronoi.html)
* [m_raytracing] (http://maratis3d.org/js/m_raytracing/m_raytracing.html)

Games / Demos
-------------

* Back on Earth - Ludum Dare 34 Compo (with source)

 [![BackOnEarth](http://maratis3d.com/download/ludum34/scm.png)](http://ludumdare.com/compo/ludum-dare-34/?action=preview&uid=45582)


Building the demos (CMake)
--------------------------

**Unix:**

run:

    build_tests_unix.sh
    
or type:

    mkdir Build
    cd Build
    cmake -G "Unix Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin
    make
    make install

**Windows:**

    mkdir Build
    cd Build
    cmake -G "Visual Studio 11" ../ -DCMAKE_INSTALL_PREFIX=../bin

or:

    mkdir Build
    cd Build
    cmake -G "MinGW Makefiles" ../ -DCMAKE_INSTALL_PREFIX=../bin
    make
    make install

**Emscripten:**

    ./emcc -s LEGACY_GL_EMULATION=1 -s USE_GLFW=3

License (zlib)
--------------

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

You may also like
-----------------

* [stb](https://github.com/nothings/stb)
* [PAL](https://github.com/parallella/pal)
* [Maratis](http://www.maratis3d.org)
* [BGFX](https://github.com/bkaradzic/bgfx)
* [GLFW](https://github.com/glfw/glfw)
