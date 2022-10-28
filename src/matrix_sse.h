#ifndef __BI_CORE_MATRIX_SSE_H__
#define __BI_CORE_MATRIX_SSE_H__

#include <emmintrin.h>
#ifdef __EMSCRIPTEN__
#include <wasm_simd128.h>
#endif

static inline __m128 __comb__(float *a, __m128 b0, __m128 b1, __m128 b2, __m128 b3)
{
  __m128 r =        _mm_mul_ps(b0, _mm_set1_ps(a[0]));
  r = _mm_add_ps(r, _mm_mul_ps(b1, _mm_set1_ps(a[1])) );
  r = _mm_add_ps(r, _mm_mul_ps(b2, _mm_set1_ps(a[2])) );
  r = _mm_add_ps(r, _mm_mul_ps(b3, _mm_set1_ps(a[3])) );
  return r;
}
static inline void bi_mat4_multiply(float *left, float* right, float* result)
{
  __m128 b0 = _mm_loadu_ps(right+0);
  __m128 b1 = _mm_loadu_ps(right+4);
  __m128 b2 = _mm_loadu_ps(right+8);
  __m128 b3 = _mm_loadu_ps(right+12);
  __m128 r0 = __comb__(left+0, b0,b1,b2,b3 );
  __m128 r1 = __comb__(left+4, b0,b1,b2,b3 );
  __m128 r2 = __comb__(left+8, b0,b1,b2,b3 );
  __m128 r3 = __comb__(left+12,b0,b1,b2,b3 );
  _mm_storeu_ps(&result[0*4], r0);
  _mm_storeu_ps(&result[1*4], r1);
  _mm_storeu_ps(&result[2*4], r2);
  _mm_storeu_ps(&result[3*4], r3);
}

#endif
