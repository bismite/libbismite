
#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 300 es\n"
#define VERTEX_SHADER_HEADER "precision highp float;\n"
#define FRAGMENT_SHADER_HEADER "precision highp float;\n"
#else
#define SHADER_VERSION "#version 410\n"
#define VERTEX_SHADER_HEADER ""
#define FRAGMENT_SHADER_HEADER ""
#endif

#define D(...) #__VA_ARGS__ "\n";

static const char *TEXCONV_VERTEX_SHADER = SHADER_VERSION VERTEX_SHADER_HEADER D(
in vec2 vertex;
in vec2 tex_coord;
out vec2 v_tex_coord;
void main(void){
  v_tex_coord = tex_coord;
  gl_Position = vec4(vertex,0.0,1.0);
}
);

static const char *TEXCONV_FRAGMENT_SHADER = SHADER_VERSION FRAGMENT_SHADER_HEADER D(
uniform sampler2D sampler;
in vec2 v_tex_coord;
out vec4 color;
void main(void){
  color = texture(sampler, v_tex_coord);
}
);

#undef D
