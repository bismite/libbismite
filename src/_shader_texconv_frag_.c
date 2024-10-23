#include "_shader_header_.h"

#define D(...) #__VA_ARGS__ "\n";

const char *SHADER_TEXCONV_FRAG =
SHADER_VERSION
FRAGMENT_SHADER_HEADER
D(
uniform sampler2D sampler;
in vec2 v_tex_coord;
out vec4 color;
void main(void){
  color = texture(sampler, v_tex_coord);
}
);

#undef D
