#ifndef __BI_CORE_MATRIX_NEON_H__
#define __BI_CORE_MATRIX_NEON_H__

#include <arm_neon.h>

static inline float32x4_t __comb__(float32x4_t a, float32x4_t b, float32x4_t c, float32x4_t d, float* r)
{
  float32x4_t result;
  result = vmulq_n_f32(         a, r[0] );
  result = vmlaq_n_f32( result, b, r[1] );
  result = vmlaq_n_f32( result, c, r[2] );
  result = vmlaq_n_f32( result, d, r[3] );
  return result;
}
static inline void bi_mat4_multiply(float *left, float* right, float* result)
{
  float32x4_t a = vld1q_f32( right+0 );
  float32x4_t b = vld1q_f32( right+4 );
  float32x4_t c = vld1q_f32( right+8 );
  float32x4_t d = vld1q_f32( right+12 );
  vst1q_f32( result+0, __comb__(a,b,c,d, left+0 ) );
  vst1q_f32( result+4, __comb__(a,b,c,d, left+4 ) );
  vst1q_f32( result+8, __comb__(a,b,c,d, left+8 ) );
  vst1q_f32( result+12, __comb__(a,b,c,d, left+12 ) );
}

#endif
