
#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 100\n"
#define VERTEX_SHADER_HEADER "precision highp float;\n"
#define FRAGMENT_SHADER_HEADER "precision highp float;\n"
#else
#define SHADER_VERSION "#version 120\n"
#define VERTEX_SHADER_HEADER ""
#define FRAGMENT_SHADER_HEADER ""
#endif

#define D(...) #__VA_ARGS__ "\n";

static const char *TEXCONV_VERTEX_SHADER = SHADER_VERSION VERTEX_SHADER_HEADER D(
attribute vec2 vertex;
attribute vec2 tex_coord;
varying   vec2 v_tex_coord;
void main(void){
  v_tex_coord = tex_coord;
  gl_Position = vec4(vertex,0.0,1.0);
}
);

static const char *TEXCONV_FRAGMENT_SHADER = SHADER_VERSION FRAGMENT_SHADER_HEADER D(
uniform sampler2D texture;
varying vec2      v_tex_coord;
void main(void){
  gl_FragColor = texture2D(texture, v_tex_coord);
}
);

#undef D
