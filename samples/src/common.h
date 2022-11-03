#include <bi/bismite.h>
#include <stdlib.h>
#include <stdio.h>
#include "fps.h"

#ifdef __EMSCRIPTEN__
#include <emscripten/version.h>
#define SHADER_HEADER "#version 300 es\n" "precision highp float;\n"
#else
#define SHADER_HEADER "#version 410\n"
#endif

#define ALLOC(x) malloc(sizeof(x))

__attribute__((unused)) static BiFontAtlas* load_font_atlas(const char* name, BiTexture *font_texture)
{
  BiFontAtlas *font = bi_font_init_with_file(ALLOC(BiFontAtlas),name);
  font->texture = font_texture;
  return font;
}

__attribute__((unused)) static BiTimer* onupdate(BiNode* n,timer_callback func)
{
  BiTimer *t = bi_timer_init(malloc(sizeof(BiTimer)),func,0,-1,NULL);
  bi_node_add_timer(n,t);
  return t;
}

static char* read_shader(const char* filename)
{
  int header_size = strlen(SHADER_HEADER);
  FILE *file = fopen(filename,"rb");
  if(file==NULL){
    printf("shader load error %s\n",filename);
    exit(1);
  }
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

__attribute__((unused)) static BiShader* create_shader(const char* vert, const char* frag)
{
  char* vartex_shader = read_shader(vert);
  char* fragment_shader = read_shader(frag);
  BiShader *shader = malloc(sizeof(BiShader));
  bi_shader_init(shader, vartex_shader, fragment_shader);
  free(vartex_shader);
  free(fragment_shader);
  return shader;
}

__attribute__((unused)) static BiShader* create_shader_with_default_vertex_shader(const char* frag)
{
  const char* vartex_shader = bi_default_vertex_shader();
  char* fragment_shader = read_shader(frag);
  BiShader *shader = malloc(sizeof(BiShader));
  bi_shader_init(shader, vartex_shader, fragment_shader);
  free(fragment_shader);
  return shader;
}

__attribute__((unused)) static BiNode* make_sprite_from_texture(BiTexture* t)
{
  BiNode* sprite = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(sprite, t->w, t->h);
  bi_node_set_anchor(sprite,0.5,0.5);
  bi_node_set_texture(sprite,t,0,0,t->w,t->h);
  return sprite;
}

#define MAKE_TEXTURE(name) bi_texture_init_with_filename(ALLOC(BiTexture),name,false)

__attribute__((unused)) static BiNode* make_sprite(const char* name)
{
  BiTexture *texture = MAKE_TEXTURE(name);
  if(texture==NULL){
    printf("load failed %s\n",name);
    exit(1);
  }
  return make_sprite_from_texture(texture);
}

__attribute__((unused)) static BiNode* make_sprite_with_anchor(const char* name,float x, float y)
{
  BiNode *n = make_sprite(name);
  bi_node_set_anchor(n,x,y);
  return n;
}

__attribute__((unused)) static void print_info(BiContext *context)
{
  printf("bismite version: %d.%d.%d\n", BISMITE_MAJOR_VERSION, BISMITE_MINOR_VERSION, BISMITE_PATCHLEVEL);
  printf("Window Size: %d,%d\n",context->w,context->h);
  //
  GLint viewport[4] = {0};
  glGetIntegerv(GL_VIEWPORT, viewport);
  printf("OpenGL ViewPort Size: %d,%d\n",viewport[2],viewport[3]);

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
