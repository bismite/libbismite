#ifndef __BI_CORE_MATRIX_H__
#define __BI_CORE_MATRIX_H__

// https://github.com/datenwolf/linmath.h
#include "linmath.h"

#if defined(__ARM_NEON)
#include "matrix_neon.h"
#elif (defined(__EMSCRIPTEN__) && defined(WASM_SIMD_ENABLED))
#include "matrix_sse.h"
#elif (! defined(__EMSCRIPTEN__))
#include "matrix_sse.h"
#else
static inline void bi_mat4_multiply(float *left, float* right, float* result) {
  mat4x4_mul((vec4*)result, (vec4*)right, (vec4*)left);
}
#endif

static inline void bi_mat4_identity(float *matrix) {
  mat4x4_identity((vec4*)matrix);
}

static inline void bi_mat4_copy(float *to, float* from) {
  mat4x4_dup((vec4*)to, (vec4*)from);
}

static inline void bi_mat4_invert(float *result, float* src) {
  mat4x4_invert((vec4*)result, (vec4*)src);
}

#endif // #ifndef __BI_CORE_MATRIX_H__
