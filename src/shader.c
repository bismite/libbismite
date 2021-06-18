#include <bi/shader.h>

#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <stdbool.h>
#include <stdio.h>

static GLuint load_shader(const char* vertex_shader_source, const char* fragment_shader_source)
{
    // compile vertex shader
    GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShaderId, 1, &vertex_shader_source, NULL);
    glCompileShader(vShaderId);

    // compile fragment shader
    GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShaderId, 1, &fragment_shader_source, NULL);
    glCompileShader(fShaderId);

    //
    GLuint program_id = glCreateProgram();
    glAttachShader(program_id,vShaderId);
    glDeleteShader(vShaderId);
    glAttachShader(program_id,fShaderId);
    glDeleteShader(fShaderId);
    glLinkProgram(program_id);

    return program_id;
}

static void load_locations(BiShader* shader)
{
  // uniform locations
  shader->projection_location = glGetUniformLocation ( shader->program_id, "projection" );
  shader->view_location = glGetUniformLocation ( shader->program_id, "view" );

  // attributes locations
  shader->vertex_location = glGetAttribLocation(shader->program_id, "vertex");
  shader->texture_uv_location = glGetAttribLocation(shader->program_id, "texture_uv");
  shader->vertex_index_location = glGetAttribLocation(shader->program_id, "vertex_index");
  shader->texture_z_location = glGetAttribLocation(shader->program_id, "texture_z");
  shader->mod_color_location = glGetAttribLocation(shader->program_id, "mod_color");
  shader->transform_locations[0] = glGetAttribLocation(shader->program_id, "transform_a");
  shader->transform_locations[1] = glGetAttribLocation(shader->program_id, "transform_b");
  shader->transform_locations[2] = glGetAttribLocation(shader->program_id, "transform_c");
  shader->transform_locations[3] = glGetAttribLocation(shader->program_id, "transform_d");

  shader->texture_locations[0] = glGetUniformLocation(shader->program_id, "sampler[0]");
  const int texture_location_indexes[8] = {0, 1, 2, 3, 4, 5, 6, 7};
  glUniform1iv(shader->texture_locations[0], 8, texture_location_indexes);

  shader->time_location = glGetUniformLocation(shader->program_id, "time");
  shader->resolution_location = glGetUniformLocation(shader->program_id, "resolution");
  shader->optional_attributes_location = glGetUniformLocation(shader->program_id, "optional_attributes");
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

void bi_shader_init(BiShader* shader,int w,int h,const char* vertex_shader_source, const char* fragment_shader_source)
{
    shader->program_id = load_shader(vertex_shader_source,fragment_shader_source);

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
    glGenVertexArrays(1, &shader->vao);
    glBindVertexArray(shader->vao);

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
      glVertexAttribDivisor(shader->vertex_location, 0); // vertex
      glVertexAttribDivisor(shader->texture_uv_location, 1); // uv
      glVertexAttribDivisor(shader->vertex_index_location, 0); // vertex_index
      glVertexAttribDivisor(shader->texture_z_location, 1); // texture_z
      glVertexAttribDivisor(shader->mod_color_location, 1); // mod_color
      for(int i=0;i<4;i++){
        glVertexAttribDivisor(shader->transform_locations[i], 1); // transform
      }

    glBindVertexArray(0);
}
