Maratis Tiny C library
======================

Maratis Tiny C library is a collection of small and efficient math and image processing routines written in C. Some of the code should be compatible with both standard C, OpenCL and CUDA, though GPGPU compatibility is still in progress. In other cases, the code is dependent on memory allocation (malloc/free) and multithreading is handled by OpenMP.

* free and open-source
* no dependencies

Math
----

* vector manipulation
* float array routines (normalization, convolution...)
* structure tensor field, harris response
* interpolation (cubic, catmullrom)
* 2d geometry routines
* 3d routines (geometry, primitives raytracing)

Color conversion
----------------

* RGB, HSV, HSL

Rasterization
-------------

* float4-attribute triangle
* basic line, circle and polygon

Image (CPU only)
----------------

* simple image manipulation (copy, conversion, mirror, reframe...)
* filters (convolution, gaussian blur, sobel, harris)
* resizing (pyrdown, resize)
* morphology operations (floodfill, dilate, erode, thinning...)
* corner detection (harris, non-maxima suppression)
* distance and voronoi functions

Building the tests (CMake)
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

Asm.js demos
------------

* [m_voronoi] (http://maratis3d.org/js/m_voronoi/m_voronoi.html)
* [m_raytracing] (http://maratis3d.org/js/m_raytracing/m_raytracing.html)

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
