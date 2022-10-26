#ifndef __BI_CORE_MATRIX_SSE_H__
#define __BI_CORE_MATRIX_SSE_H__

#include <emmintrin.h>
#ifdef __EMSCRIPTEN__
#include <wasm_simd128.h>
#endif

static inline __m128 __comb__(float *a, float *b)
{
  __m128 r =        _mm_mul_ps(*(__m128*)&b[0*4], _mm_set1_ps(a[0]));
  r = _mm_add_ps(r, _mm_mul_ps(*(__m128*)&b[1*4], _mm_set1_ps(a[1])) );
  r = _mm_add_ps(r, _mm_mul_ps(*(__m128*)&b[2*4], _mm_set1_ps(a[2])) );
  r = _mm_add_ps(r, _mm_mul_ps(*(__m128*)&b[3*4], _mm_set1_ps(a[3])) );
  return r;
}
static inline void bi_mat4_multiply(float *left, float* right, float* result)
{
  __m128 r0 = __comb__(&left[0*4], right );
  __m128 r1 = __comb__(&left[1*4], right );
  __m128 r2 = __comb__(&left[2*4], right );
  __m128 r3 = __comb__(&left[3*4], right );
  _mm_storeu_ps(&result[0*4], r0);
  _mm_storeu_ps(&result[1*4], r1);
  _mm_storeu_ps(&result[2*4], r2);
  _mm_storeu_ps(&result[3*4], r3);
}

#endif
