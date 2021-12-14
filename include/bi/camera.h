#ifndef __BISMITE_CAMERA_H__
#define __BISMITE_CAMERA_H__

#include <stdbool.h>
#include <bi/bi_gl.h>

extern GLfloat* bi_camera_matrix(GLfloat* matrix,float x,float y,float w,float h,bool flip_vertical);

#endif
