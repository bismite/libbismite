#include <bi/bismite.h>
#include <bi/ext/label.h>
#include <stdlib.h>
#include <stdio.h>

#define W 480
#define H 320

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#include <emscripten/version.h>
#define SHADER_HEADER "#version 300 es\n" "precision highp float;\n"
EM_JS_DEPS(sdl_deps, "$autoResumeAudioContext,$dynCall");
#else
#define SHADER_HEADER "#version 410\n"
#endif

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define ALLOC(x) malloc(sizeof(x))

//
// Font
//
__attribute__((unused)) static BiFont* load_font(const char* name, BiTexture *font_texture)
{
  BiFont *font = bi_font_init_with_file(ALLOC(BiFont),name);
  font->texture = font_texture;
  return font;
}

//
// FPS Layer
//

static void label_fps_indicate(BiContext* context,BiTimer* timer,double dt)
{
  BiLabel *label = timer->userdata;
  char text[1024];
  snprintf(text,1024,"FPS:%.2f", context->profile.stats.fps );
  bi_label_set_text(label, text);
}

static BiLabel* create_fps_label(BiContext* context)
{
  // Font
  BiTexture *font_texture = bi_texture_init_with_filename(malloc(sizeof(BiTexture)),"assets/font.png",false);
  BiFont *font = load_font("assets/font12.dat",font_texture);
  // label
  BiLabel* label = bi_label_init(malloc(sizeof(BiLabel)),font);
  bi_label_set_background_color(label, RGBA(0,0,0,128));
  bi_label_set_text(label, "FPS:00.00");
  // timer
  BiTimer *timer = bi_timer_init(malloc(sizeof(BiTimer)), label_fps_indicate, 100, -1, label);
  bi_node_add_timer((BiNode*)label,timer);
  return label;
}

__attribute__((unused)) static void add_fps_layer(BiContext* context)
{
  // label
  BiLabel* label = create_fps_label(context);
  bi_node_set_anchor((BiNode*)label,0,1);
  bi_node_set_position((BiNode*)label,0,context->h);
  // shader
  BiShaderNode *shader_node = bi_shader_node_init(malloc(sizeof(BiShaderNode)));
  bi_node_set_z(shader_node,0xff);
  bi_node_add_node( shader_node, (BiNode*)label );
  shader_node->textures[0] = label->font->texture;
  // add shader
  bi_node_add_node(&context->default_framebuffer_node, shader_node);
}

//
// Utilities
//

__attribute__((unused)) static BiTimer* onupdate(BiNode* n,timer_callback func)
{
  BiTimer *t = bi_timer_init(malloc(sizeof(BiTimer)),func,0,-1,n);
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

__attribute__((unused)) static BiNode* set_texture(BiNode* n, const char* name)
{
  BiTexture *tex = bi_texture_init_with_filename(ALLOC(BiTexture),name,false);
  bi_node_set_size(n, tex->w, tex->h);
  bi_node_set_texture(n,tex,0,0,tex->w,tex->h);
  return n;
}

__attribute__((unused)) static BiNode* make_sprite_from_texture(BiTexture* t)
{
  BiNode* sprite = bi_node_init(malloc(sizeof(BiNode)));
  bi_node_set_size(sprite, t->w, t->h);
  bi_node_set_texture(sprite,t,0,0,t->w,t->h);
  bi_node_set_anchor(sprite,0.5,0.5);
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

__attribute__((unused)) static BiNode* make_bg(const char* name)
{
  BiNode *n = make_sprite(name);
  bi_node_set_anchor(n,0,0);
  return n;
}

static void print_version(const char* name,const SDL_version *v)
{
  printf("%s : %u.%u.%u\n", name, v->major, v->minor, v->patch);
}

#define print_gl_parameter(name) { GLint v; glGetIntegerv(name, &v); printf( #name "=%d\n",v ); }

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

  SDL_version v;
  SDL_VERSION(&v);
  print_version("SDL(compile)",&v);
  SDL_GetVersion(&v);
  print_version("SDL(link)",&v);
  SDL_IMAGE_VERSION(&v)
  print_version("Image(compile)", &v);
  print_version("Image(link)", IMG_Linked_Version() );
  SDL_MIXER_VERSION(&v)
  print_version("Mixer(compile)", &v);
  print_version("Mixer(link)", Mix_Linked_Version() );

  printf("OpenGL Vendor: %s\n", glGetString(GL_VENDOR));
  printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
  printf("OpenGL Renderer: %s\n", glGetString(GL_RENDERER));
  printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  print_gl_parameter(GL_MAX_TEXTURE_SIZE);
  print_gl_parameter(GL_MAX_TEXTURE_IMAGE_UNITS);
  print_gl_parameter(GL_MAX_COLOR_ATTACHMENTS);
  print_gl_parameter(GL_MAX_DRAW_BUFFERS);

  // Determine Default Framebuffer Size
  printf("HIGH DPI: %s\n", is_high_dpi(context) ? "YES" : "NO");
  int w,h;
  SDL_GetWindowSize(context->_window,&w,&h);
  printf("SDL_GetWindowSize: %d,%d\n",w,h);
  SDL_GetWindowSizeInPixels(context->_window,&w,&h);
  printf("SDL_GetWindowSizeInPixels: %d,%d\n",w,h);
  // SDL_GL_GetDrawableSize(context->_window,&w,&h);
  // printf("SDL_GL_GetDrawableSize: %d,%d\n",w,h);
}

static void on_window_size_changed(BiContext* context)
{
  int pw,ph;
  SDL_GetWindowSizeInPixels(context->_window,&pw,&ph);
  int w,h;
  SDL_GetWindowSize(context->_window,&w,&h);
  printf("on_window_size_changed: %d,%d %d,%d\n",w,h, pw,ph);
  context->w = w;
  context->h = h;
  context->default_framebuffer.w = w;
  context->default_framebuffer.h = h;
}

__attribute__((unused)) static BiContext* make_context(const char* name)
{
  srand( bi_get_now() );
  uint32_t flags = BI_WINDOW_ALLOW_HIGHDPI | BI_WINDOW_RESIZABLE;
  BiContext* context = bi_init_context(ALLOC(BiContext),W,H,0,flags,name);
  print_info(context);
  context->window_event_callback[SDL_WINDOWEVENT_SIZE_CHANGED] = on_window_size_changed;
  add_fps_layer(context);
  return context;
}
