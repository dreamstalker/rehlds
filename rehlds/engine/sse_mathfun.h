/* SIMD (SSE1+MMX or SSE2) implementation of sin, cos, exp and log

Inspired by Intel Approximate Math library, and based on the
corresponding algorithms of the cephes math library

The default is to use the SSE1 version. If you define USE_SSE2 the
the SSE2 intrinsics will be used in place of the MMX intrinsics. Do
not expect any significant performance improvement with SSE2.
*/

/* Copyright (C) 2007  Julien Pommier

This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
claim that you wrote the original software. If you use this software
in a product, an acknowledgment in the product documentation would be
appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.

(this is the zlib license)
*/
#pragma once

#include <xmmintrin.h>

/* yes I know, the top of this file is quite ugly */

#ifdef _MSC_VER /* visual c++ */
# define ALIGN16_BEG __declspec(align(16))
# define ALIGN16_END 
#else /* gcc or icc */
# define ALIGN16_BEG
# define ALIGN16_END __attribute__((aligned(16)))
#endif

/* __m128 is ugly to write */
typedef __m128 v4sf;  // vector of 4 float (sse1)

#include <emmintrin.h>
typedef __m128i v4si; // vector of 4 int (sse2)


/* declare some SSE constants -- why can't I figure a better way to do that? */
#define _PS_CONST(Name, Val)                                            \
  static const ALIGN16_BEG float _ps_##Name[4] ALIGN16_END = { Val, Val, Val, Val }
#define _PI32_CONST(Name, Val)                                            \
  static const ALIGN16_BEG int _pi32_##Name[4] ALIGN16_END = { Val, Val, Val, Val }
#define _PS_CONST_TYPE(Name, Type, Val)                                 \
  static const ALIGN16_BEG Type _ps_##Name[4] ALIGN16_END = { Val, Val, Val, Val }

_PS_CONST(1, 1.0f);
_PS_CONST(0p5, 0.5f);
/* the smallest non denormalized float number */
_PS_CONST_TYPE(min_norm_pos, int, 0x00800000);
_PS_CONST_TYPE(mant_mask, int, 0x7f800000);
_PS_CONST_TYPE(inv_mant_mask, int, ~0x7f800000);

_PS_CONST_TYPE(sign_mask, int, (int)0x80000000);
_PS_CONST_TYPE(inv_sign_mask, int, ~0x80000000);

_PI32_CONST(1, 1);
_PI32_CONST(inv1, ~1);
_PI32_CONST(2, 2);
_PI32_CONST(4, 4);
_PI32_CONST(0x7f, 0x7f);

_PS_CONST(cephes_SQRTHF, 0.707106781186547524f);
_PS_CONST(cephes_log_p0, 7.0376836292E-2f);
_PS_CONST(cephes_log_p1, -1.1514610310E-1f);
_PS_CONST(cephes_log_p2, 1.1676998740E-1f);
_PS_CONST(cephes_log_p3, -1.2420140846E-1f);
_PS_CONST(cephes_log_p4, +1.4249322787E-1f);
_PS_CONST(cephes_log_p5, -1.6668057665E-1f);
_PS_CONST(cephes_log_p6, +2.0000714765E-1f);
_PS_CONST(cephes_log_p7, -2.4999993993E-1f);
_PS_CONST(cephes_log_p8, +3.3333331174E-1f);
_PS_CONST(cephes_log_q1, -2.12194440e-4f);
_PS_CONST(cephes_log_q2, 0.693359375f);



_PS_CONST(exp_hi, 88.3762626647949f);
_PS_CONST(exp_lo, -88.3762626647949f);

_PS_CONST(cephes_LOG2EF, 1.44269504088896341f);
_PS_CONST(cephes_exp_C1, 0.693359375f);
_PS_CONST(cephes_exp_C2, -2.12194440e-4f);

_PS_CONST(cephes_exp_p0, 1.9875691500E-4f);
_PS_CONST(cephes_exp_p1, 1.3981999507E-3f);
_PS_CONST(cephes_exp_p2, 8.3334519073E-3f);
_PS_CONST(cephes_exp_p3, 4.1665795894E-2f);
_PS_CONST(cephes_exp_p4, 1.6666665459E-1f);
_PS_CONST(cephes_exp_p5, 5.0000001201E-1f);

_PS_CONST(minus_cephes_DP1, -0.78515625f);
_PS_CONST(minus_cephes_DP2, -2.4187564849853515625e-4f);
_PS_CONST(minus_cephes_DP3, -3.77489497744594108e-8f);
_PS_CONST(sincof_p0, -1.9515295891E-4f);
_PS_CONST(sincof_p1, 8.3321608736E-3f);
_PS_CONST(sincof_p2, -1.6666654611E-1f);
_PS_CONST(coscof_p0, 2.443315711809948E-005f);
_PS_CONST(coscof_p1, -1.388731625493765E-003f);
_PS_CONST(coscof_p2, 4.166664568298827E-002f);
_PS_CONST(cephes_FOPI, 1.27323954473516f); // 4 / M_PI

extern v4sf log_ps(v4sf x);
extern v4sf exp_ps(v4sf x);
extern v4sf sin_ps(v4sf x);
extern v4sf cos_ps(v4sf x);
extern void sincos_ps(v4sf x, v4sf *s, v4sf *c);
