
in vec2 uv;
in vec4 crop;
in vec2 local_xy;
flat in int _texture_index;
in vec4 _tint;
in vec4 _modulate;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
uniform mat4 shader_extra_data;

layout (location = 0) out vec4 color0;

vec4 getTextureColor(int index,vec2 xy,vec4 crop) {
  if( index < 0 || 16 <= index ) { return vec4(1.0); }
  float upper = crop[1] > crop[3] ? crop[1] : crop[3];
  float lower = crop[1] > crop[3] ? crop[3] : crop[1];
  if( xy.x < 0.0 || 1.0 < xy.x  ){ return vec4(0.0); }
  if( xy.y < 0.0 || 1.0 < xy.y  ){ return vec4(0.0); }
  if( xy.x < crop[0] ){ return vec4(0.0); }
  if( xy.y < lower ){ return vec4(0.0); }
  if( xy.x > crop[2] ){ return vec4(0.0); }
  if( xy.y > upper ){ return vec4(0.0); }
  if(index==0){ return texture(sampler[0], xy); }
  if(index==1){ return texture(sampler[1], xy); }
  if(index==2){ return texture(sampler[2], xy); }
  if(index==3){ return texture(sampler[3], xy); }
  if(index==4){ return texture(sampler[4], xy); }
  if(index==5){ return texture(sampler[5], xy); }
  if(index==6){ return texture(sampler[6], xy); }
  if(index==7){ return texture(sampler[7], xy); }
  if(index==8){ return texture(sampler[8], xy); }
  if(index==9){ return texture(sampler[9], xy); }
  if(index==10){ return texture(sampler[10], xy); }
  if(index==11){ return texture(sampler[11], xy); }
  if(index==12){ return texture(sampler[12], xy); }
  if(index==13){ return texture(sampler[13], xy); }
  if(index==14){ return texture(sampler[14], xy); }
  if(index==15){ return texture(sampler[15], xy); }
  return vec4(0.0);
}

void main()
{
  vec4 c = getTextureColor(_texture_index, uv, crop);
  c = vec4(c.rgb*_modulate.rgb, c.a) * _modulate.a;
  color0 = vec4( _tint.rgb*_tint.a*c.a + c.rgb*(1.0-_tint.a), c.a );
}
