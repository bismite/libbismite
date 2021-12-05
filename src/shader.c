#include <bi/shader.h>
#include <bi/matrix.h>
#include <bi/util.h>
#include <bi/bi_sdl.h>
#include <stdbool.h>
#include <stdio.h>

static void print_shader_log(const char* name, GLuint shader_id)
{
  int loglen=0;
  glGetShaderiv(shader_id,GL_INFO_LOG_LENGTH,&loglen);
  if(loglen>0){
    char *log = malloc(loglen);
    glGetShaderInfoLog(shader_id, loglen, NULL, log);
    printf("%s: %s\n",name,log);
    free(log);
  }
}

static void load_shader(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  // compile vertex shader
  GLuint vShaderId = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vShaderId, 1, &vertex_shader_source, NULL);
  glCompileShader(vShaderId);
  print_shader_log("Vertex Shader Log",vShaderId);

  // compile fragment shader
  GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fShaderId, 1, &fragment_shader_source, NULL);
  glCompileShader(fShaderId);
  print_shader_log("Fragment Shader Log",fShaderId);

  //
  GLuint program_id = glCreateProgram();
  glAttachShader(program_id,vShaderId);
  glDeleteShader(vShaderId);
  glAttachShader(program_id,fShaderId);
  glDeleteShader(fShaderId);
  glLinkProgram(program_id);

  //
  shader->program_id = program_id;

  // Attribute locations
  shader->vertex_location = glGetAttribLocation(program_id, "vertex");
  shader->texture_uv_location = glGetAttribLocation(program_id, "texture_uv");
  shader->vertex_index_location = glGetAttribLocation(program_id, "vertex_index");
  shader->opacity_location = glGetAttribLocation(program_id, "opacity");
  shader->texture_z_location = glGetAttribLocation(program_id, "texture_z");
  shader->tint_color_location = glGetAttribLocation(program_id, "tint_color");
  shader->transform_locations[0] = glGetAttribLocation(program_id, "transform_a");
  shader->transform_locations[1] = glGetAttribLocation(program_id, "transform_b");
  shader->transform_locations[2] = glGetAttribLocation(program_id, "transform_c");
  shader->transform_locations[3] = glGetAttribLocation(program_id, "transform_d");

  // Uniform locations
  shader->camera_location = glGetUniformLocation(program_id, "camera");
  shader->texture_locations[0] = glGetUniformLocation(program_id, "sampler[0]");
  shader->time_location = glGetUniformLocation(program_id, "time");
  shader->resolution_location = glGetUniformLocation(program_id, "resolution");
  shader->scale_location = glGetUniformLocation(program_id, "scale");
  shader->optional_attributes_location = glGetUniformLocation(program_id, "optional_attributes");
}

void bi_shader_init(BiShader* shader,const char* vertex_shader_source, const char* fragment_shader_source)
{
  load_shader(shader,vertex_shader_source,fragment_shader_source);

  // create vbo
  glGenBuffers(1, &shader->uv_buffer);
  glGenBuffers(1, &shader->opacity_buffer);
  glGenBuffers(1, &shader->texture_z_buffer);
  glGenBuffers(1, &shader->tint_color_buffer);
  glGenBuffers(1, &shader->transform_buffer);

  // create vbo : vertex
  glGenBuffers(1, &shader->vertex_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_buffer);
  GLfloat l=0, t=1, r=1, b=0; // vertex left,top,right,bottom
  GLfloat vertex[8] = { l,t, l,b, r,t, r,b };
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 8, vertex, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // create vbo : vertex index
  glGenBuffers(1, &shader->vertex_index_buffer);
  glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_index_buffer);
  GLfloat vertex_index[4] = {0,1,2,3}; // XXX: prior OpenGL3 and ES3, can not send int...
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4, vertex_index, GL_DYNAMIC_DRAW);
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

    // vertex_index (XXX: glVertexAttribIPointer is not available in WebGL 1.0)
    glBindBuffer(GL_ARRAY_BUFFER, shader->vertex_index_buffer);
    glEnableVertexAttribArray ( shader->vertex_index_location );
    glVertexAttribPointer ( shader->vertex_index_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

    // opacity
    glBindBuffer(GL_ARRAY_BUFFER, shader->opacity_buffer);
    glEnableVertexAttribArray ( shader->opacity_location );
    glVertexAttribPointer ( shader->opacity_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

    // texture_z (XXX: glVertexAttribIPointer is not available in WebGL 1.0)
    glBindBuffer(GL_ARRAY_BUFFER, shader->texture_z_buffer);
    glEnableVertexAttribArray ( shader->texture_z_location );
    glVertexAttribPointer ( shader->texture_z_location, 1, GL_FLOAT, GL_FALSE, 0, NULL );

    // tint color
    glBindBuffer(GL_ARRAY_BUFFER, shader->tint_color_buffer);
    glEnableVertexAttribArray ( shader->tint_color_location );
    glVertexAttribPointer ( shader->tint_color_location, 4, GL_FLOAT, GL_FALSE, 0, NULL );

    // transform
    for(int i=0;i<4;i++){
      glBindBuffer(GL_ARRAY_BUFFER, shader->transform_buffer);
      glEnableVertexAttribArray ( shader->transform_locations[i] );
      glVertexAttribPointer ( shader->transform_locations[i], 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*4*4, NULL+sizeof(GLfloat)*4*i );
    }

    // for instancing
    glVertexAttribDivisor(shader->vertex_location, 0);
    glVertexAttribDivisor(shader->texture_uv_location, 1);
    glVertexAttribDivisor(shader->vertex_index_location, 0);
    glVertexAttribDivisor(shader->opacity_location, 1);
    glVertexAttribDivisor(shader->texture_z_location, 1);
    glVertexAttribDivisor(shader->tint_color_location, 1);
    for(int i=0;i<4;i++){
      glVertexAttribDivisor(shader->transform_locations[i], 1);
    }

  glBindVertexArray(0);

  // set sampler2D
  glUseProgram(shader->program_id);
  int texture_location_indexes[BI_LAYER_MAX_TEXTURES];
  for(int i=0; i<BI_LAYER_MAX_TEXTURES; i++) { texture_location_indexes[i] = i; }
  glUniform1iv(shader->texture_locations[0], BI_LAYER_MAX_TEXTURES, texture_location_indexes);
}

void bi_shader_set_camera(BiShader* shader,float w,float h,float x,float y,bool flip_vertical)
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

  bi_mat4_multiply(trans,camera,camera);

  glUniformMatrix4fv(shader->camera_location, 1, GL_FALSE, camera);
}
