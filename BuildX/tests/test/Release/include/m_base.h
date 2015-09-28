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

#ifndef _M_BASE_H_
#define _M_BASE_H_

/* macros */
#ifndef __OPENCL_VERSION__
	#define M_API
	#define M_SAFE_FREE(p) {if (p) {free(p); (p) = NULL;}}
#else
	#define M_API
#endif

/* types */
enum M_TYPES
{
	M_VOID = 0,
	M_BOOL,
	M_BYTE,
	M_UBYTE,
	M_SHORT,
	M_USHORT,
	M_INT,
	M_UINT,
	M_HALF,
	M_FLOAT,
	M_DOUBLE
};

#endif
