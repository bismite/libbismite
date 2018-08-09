#ifndef __BI_CORE_MATRIX_H__
#define __BI_CORE_MATRIX_H__

// https://github.com/datenwolf/linmath.h
#include <bi/linmath.h>

static inline void bi_mat4_identity(float *matrix) {
  mat4x4_identity((vec4*)matrix);
}

static inline void bi_mat4_copy(float *to, float* from) {
  mat4x4_dup((vec4*)to, (vec4*)from);
}

static inline void bi_mat4_multiply(float *left, float* right, float* result) {
  mat4x4_mul((vec4*)result, (vec4*)right, (vec4*)left);
}

static inline void bi_mat4_invert(float *result, float* src) {
  mat4x4_invert((vec4*)result, (vec4*)src);
}

#endif
