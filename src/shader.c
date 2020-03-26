#include <bi/shader.h>
#include <bi/shader_text.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <stdbool.h>
#include <stdio.h>

static GLuint load_shader()
{
    // compile vertex shader
    GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShaderId, 1, &VERTEX_SHADER, NULL);
    glCompileShader(vShaderId);

    // compile fragment shader
    GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fShaderId, 1, &FRAGMENT_SHADER, NULL);
    glCompileShader(fShaderId);

    //
    GLuint programId = glCreateProgram();
    glAttachShader(programId,vShaderId);
    glAttachShader(programId,fShaderId);

    //
    glLinkProgram(programId);

    glUseProgram(programId);

    return programId;
}

static void load_locations(BiShader* shader)
{
  // uniform locations
  shader->projection_location = glGetUniformLocation ( shader->programId, "projection" );
  shader->view_location = glGetUniformLocation ( shader->programId, "view" );
  shader->texture_locations[0]  = glGetUniformLocation(shader->programId, "sampler0");
  shader->texture_locations[1]  = glGetUniformLocation(shader->programId, "sampler1");
  shader->texture_locations[2]  = glGetUniformLocation(shader->programId, "sampler2");
  shader->texture_locations[3]  = glGetUniformLocation(shader->programId, "sampler3");
  shader->texture_locations[4]  = glGetUniformLocation(shader->programId, "sampler4");
  shader->texture_locations[5]  = glGetUniformLocation(shader->programId, "sampler5");
  shader->texture_locations[6]  = glGetUniformLocation(shader->programId, "sampler6");
  shader->texture_locations[7]  = glGetUniformLocation(shader->programId, "sampler7");

  // attributes locations
  shader->vertex_location = glGetAttribLocation(shader->programId, "vertex");
  shader->texture_uv_location = glGetAttribLocation(shader->programId, "texture_uv");
  shader->vertex_index_location = glGetAttribLocation(shader->programId, "vertex_index");
  shader->texture_z_location = glGetAttribLocation(shader->programId, "texture_z");
  shader->mod_color_location = glGetAttribLocation(shader->programId, "mod_color");
  shader->transform_locations[0] = glGetAttribLocation(shader->programId, "transform_a");
  shader->transform_locations[1] = glGetAttribLocation(shader->programId, "transform_b");
  shader->transform_locations[2] = glGetAttribLocation(shader->programId, "transform_c");
  shader->transform_locations[3] = glGetAttribLocation(shader->programId, "transform_d");
}

void bi_set_projection(BiShader* shader,int w, int h, bool centering)
{
    if(centering) {
      int l = -w/2;
      int r =  w/2;
      int t =  h/2;
      int b = -h/2;

      GLfloat projection[16] = {
        2.0/(r-l),           0.0, 0.0, 0.0,
               0.0,    2.0/(t-b), 0.0, 0.0,
               0.0,          0.0, 1.0, 0.0,
      -(r+l)/(r-l), -(t+b)/(t-b), 0.0, 1.0
      };

      glUniformMatrix4fv(shader->projection_location, 1, GL_FALSE, projection);

    }else{
      GLfloat projection[16] = {
        2.0/w,   0.0, 0.0, 0.0,
          0.0, 2.0/h, 0.0, 0.0,
          0.0,   0.0, 1.0, 0.0,
         -1.0,  -1.0, 0.0, 1.0
      };

      glUniformMatrix4fv(shader->projection_location, 1, GL_FALSE, projection);
    }
}


void bi_set_camera(BiShader* shader,int x, int y)
{
    GLfloat view[16] = {
      1.0, 0.0, 0.0, 0.0,
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
       -x,  -y, 0.0, 1.0
    };

    glUniformMatrix4fv(shader->view_location, 1, GL_FALSE, view);
}

void bi_init_shader(BiShader* shader,int w,int h)
{
    glEnable(GL_BLEND);

    shader->programId = load_shader();

    load_locations(shader);

    // view
    bi_set_camera(shader,0,0);

    // projection
    bi_set_projection(shader,w,h,false);

    // vertex
    GLfloat l=0, t=1, r=1, b=0;
    GLfloat vertex[8] = {
      l, t,
      l, b,
      r, t,
      r, b
    };

    // XXX: prior OpenGL3 and ES3, can not send int...
    GLfloat vertex_index[4] = {0,1,2,3};

    // create vbo
    glGenBuffers(1, &shader->uv_buffer);
    glGenBuffers(1, &shader->texture_z_buffer);
    glGenBuffers(1, &shader->mod_color_buffer);
    glGenBuffers(1, &shader->transform_buffer);

    // create vbo : vertex
    glGenBuffers(1, &shader->vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertex, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vbo : vertex index
    glGenBuffers(1, &shader->vertex_index_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_index_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, vertex_index, GL_DYNAMIC_DRAW); // XXX: float. not int.
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // create vao
    glGenVertexArrays_wrapper(1, &shader->vao);
    glBindVertexArray_wrapper(shader->vao);

      // vertex
      glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_buffer);
      glEnableVertexAttribArray ( shader->vertex_location );
      glVertexAttribPointer ( shader->vertex_location, 2, GL_FLOAT, GL_FALSE, 0, NULL );

      // texture_uv
      glBindBuffer(GL_ARRAY_BUFFER, shader->uv_buffer);
      glEnableVertexAttribArray ( shader->texture_uv_location );
      glVertexAttribPointer ( shader->texture_uv_location, 4, GL_FLOAT, GL_FALSE, 0, NULL );

      // vertex_index
      glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_index_buffer);
      glEnableVertexAttribArray ( shader->vertex_index_location );
      // XXX: no glVertexAttribIPointer : since OpenGL3.0 and ES3.0
      glVertexAttribPointer ( shader->vertex_index_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

      // texture_z
      glBindBuffer(GL_ARRAY_BUFFER, shader->texture_z_buffer);
      glEnableVertexAttribArray ( shader->texture_z_location );
      // XXX: no glVertexAttribIPointer : since OpenGL3.0 and ES3.0
      glVertexAttribPointer ( shader->texture_z_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

      // mod_color
      glBindBuffer(GL_ARRAY_BUFFER, shader->mod_color_buffer);
      glEnableVertexAttribArray ( shader->mod_color_location );
      glVertexAttribPointer ( shader->mod_color_location, 4, GL_FLOAT, GL_FALSE, 0, NULL );

      // transform
      for(int i=0;i<4;i++){
        glBindBuffer(GL_ARRAY_BUFFER, shader->transform_buffer);
        glEnableVertexAttribArray ( shader->transform_locations[i] );
        glVertexAttribPointer ( shader->transform_locations[i], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4*4, NULL+sizeof(GLfloat)*4*i );
      }

      // for instancing
      glVertexAttribDivisor_wrapper(shader->vertex_location, 0); // vertex
      glVertexAttribDivisor_wrapper(shader->texture_uv_location, 1); // uv
      glVertexAttribDivisor_wrapper(shader->vertex_index_location, 0); // vertex_index
      glVertexAttribDivisor_wrapper(shader->texture_z_location, 1); // texture_z
      glVertexAttribDivisor_wrapper(shader->mod_color_location, 1); // mod_color
      for(int i=0;i<4;i++){
        glVertexAttribDivisor_wrapper(shader->transform_locations[i], 1); // transform
      }

    glBindVertexArray_wrapper(0);
}
