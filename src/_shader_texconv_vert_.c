#include "_shader_header_.h"

#define D(...) #__VA_ARGS__ "\n";

const char *SHADER_TEXCONV_VERT =
SHADER_VERSION
VERTEX_SHADER_HEADER
D(
in vec2 vertex;
in vec2 tex_coord;
out vec2 v_tex_coord;
void main(void){
  v_tex_coord = tex_coord;
  gl_Position = vec4(vertex,0.0,1.0);
}
);

#undef D
