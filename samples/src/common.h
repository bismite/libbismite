#include <bi/bismite.h>
#include <stdlib.h>
#include <stdio.h>
#include "fps.h"

#ifdef __EMSCRIPTEN__
#define SHADER_HEADER "#version 100\n" "precision highp float;\n"
#else
#define SHADER_HEADER "#version 120\n"
#endif

static char* read_shader(const char* filename)
{
  int header_size = strlen(SHADER_HEADER);
  FILE *file = fopen(filename,"rb");
  fseek(file, 0, SEEK_END);
  int file_size = ftell(file);
  rewind(file);
  char* string = malloc(file_size+header_size+1);
  strcpy(string,SHADER_HEADER);
  fread(string+header_size, sizeof(char), file_size, file);
  string[file_size+header_size] = 0;
  fclose(file);
  return string;
}

static BiShader* __attribute__((unused)) create_shader(const char* vert, const char* frag)
{
  char* vartex_shader = read_shader(vert);
  char* fragment_shader = read_shader(frag);
  BiShader *shader = malloc(sizeof(BiShader));
  bi_shader_init(shader, vartex_shader, fragment_shader);
  free(vartex_shader);
  free(fragment_shader);
  return shader;
}

static BiTextureMapping* make_texture_mapping(const char* name)
{
  // load texture
  BiTextureMapping *mapping = malloc(sizeof(BiTextureMapping));
  BiTexture *texture = malloc(sizeof(BiTexture));
  if( bi_texture_init_with_filename(texture,name,false) ) {
    bi_texture_mapping_init(mapping,texture);
  }else{
    LOG("load error\n");
    return NULL;
  }
  return mapping;
}

static BiNode* make_sprite_from_mapping(BiTextureMapping *mapping)
{
  BiNode* sprite = malloc(sizeof(BiNode));
  bi_node_init(sprite);
  sprite->w = mapping->texture->w;
  sprite->h = mapping->texture->h;
  sprite->anchor_x = 0.5;
  sprite->anchor_y = 0.5;
  sprite->texture_mapping = mapping;
  bi_set_color( sprite->color, 0xFF, 0xFF, 0xFF, 0xFF);
  return sprite;
}

static BiNode* __attribute__((unused)) make_sprite(const char* name)
{
  // load texture
  BiTextureMapping *mapping = make_texture_mapping(name);
  return make_sprite_from_mapping(mapping);
}

static void __attribute__ ((unused)) print_info(BiContext *context)
{
  printf("Window Size: %d,%d\n",context->w,context->h);
  printf("bismite version: %d.%d.%d\n", BISMITE_MAJOR_VERSION, BISMITE_MINOR_VERSION, BISMITE_PATCHLEVEL);

#ifdef EMSCRIPTEN
  printf("emscripten version: %d.%d.%d\n", __EMSCRIPTEN_major__, __EMSCRIPTEN_minor__, __EMSCRIPTEN_tiny__ );
#endif
  printf("compiler: %s\n", __VERSION__);
  SDL_version compiled;
  SDL_version linked;
  SDL_VERSION(&compiled);
  SDL_GetVersion(&linked);
  printf("SDL(compile): %d.%d.%d\n", compiled.major, compiled.minor, compiled.patch);
  printf("SDL(link): %d.%d.%d\n", linked.major, linked.minor, linked.patch);

  printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  GLint max_texture_size;
  glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
  printf("GL_MAX_TEXTURE_SIZE: %d\n",max_texture_size);

  GLint max_texture_image_units;
  glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_texture_image_units);
  printf("GL_MAX_TEXTURE_IMAGE_UNITS: %d\n",max_texture_image_units);
}
