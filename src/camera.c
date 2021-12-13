#include <bi/camera.h>
#include <bi/matrix.h>

GLfloat* bi_camera_matrix(GLfloat* matrix,float x,float y,float w,float h,bool flip_vertical)
{
  // Projection
  GLfloat camera[16] = {
    2.0/w,   0.0, 0.0, 0.0,
      0.0, 2.0/h, 0.0, 0.0,
      0.0,   0.0, 1.0, 0.0,
     -1.0,  -1.0, 0.0, 1.0
  };

  // upside-down for framebuffer
  if(flip_vertical){
    GLfloat flip_scale[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0,-1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0
    };
    GLfloat flip_trans[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0,  -h, 0.0, 1.0
    };
    bi_mat4_multiply(flip_scale,camera,camera);
    bi_mat4_multiply(flip_trans,camera,camera);
  }

  // Trans
  GLfloat trans[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
     -x,  -y, 0.0, 1.0
  };

  bi_mat4_multiply(trans,camera,matrix);
  return matrix;
}
