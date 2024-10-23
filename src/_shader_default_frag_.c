#include "_shader_header_.h"

#define D(...) #__VA_ARGS__ "\n";

const char *SHADER_DEFAULT_FRAG =
SHADER_VERSION
FRAGMENT_SHADER_HEADER
D(
in vec2 uv;
in vec2 local_xy;
flat in int _texture_index;
in vec4 _tint;
in vec4 _modulate;
in mat4 _node_extra_data;
uniform float time;
uniform vec2 resolution;
uniform mat4 layer_extra_data;
uniform sampler2D sampler[16];
out vec4 color;
vec4 getTextureColor(int samplerID,vec2 xy) {
  if(samplerID==0){ return texture(sampler[0], xy); }
  if(samplerID==1){ return texture(sampler[1], xy); }
  if(samplerID==2){ return texture(sampler[2], xy); }
  if(samplerID==3){ return texture(sampler[3], xy); }
  if(samplerID==4){ return texture(sampler[4], xy); }
  if(samplerID==5){ return texture(sampler[5], xy); }
  if(samplerID==6){ return texture(sampler[6], xy); }
  if(samplerID==7){ return texture(sampler[7], xy); }
  if(samplerID==8){ return texture(sampler[8], xy); }
  if(samplerID==9){ return texture(sampler[9], xy); }
  if(samplerID==10){ return texture(sampler[10], xy); }
  if(samplerID==11){ return texture(sampler[11], xy); }
  if(samplerID==12){ return texture(sampler[12], xy); }
  if(samplerID==13){ return texture(sampler[13], xy); }
  if(samplerID==14){ return texture(sampler[14], xy); }
  if(samplerID==15){ return texture(sampler[15], xy); }
  return vec4(0);
}

void main()
{
  vec4 c = vec4(1.0);
  if( 0 <= _texture_index && _texture_index < 16 ) {
    c = getTextureColor( _texture_index, uv);
  }
  c = vec4(c.rgb*_modulate.rgb, c.a) * _modulate.a;
  color = vec4( _tint.rgb*_tint.a*c.a + c.rgb*(1.0-_tint.a), c.a );
}
);

#undef D
