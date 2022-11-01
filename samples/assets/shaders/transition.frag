
in vec2 uv;
flat in int _texture_index;
in vec4 _tint_color;
in float _opacity;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
uniform float scale;
uniform vec4 optional_attributes;
out vec4 output_color;

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

const float GRID_SIZE = 50.0;
const float S_GRID_SIZE = 5.0;

void main()
{
  float progress = optional_attributes[0];

  vec2 xy = gl_FragCoord.xy / scale;
  xy = floor(xy / S_GRID_SIZE) * S_GRID_SIZE;
  vec2 tmp = fract(xy / GRID_SIZE);
  if ( tmp.x + tmp.y > progress * 2.0 ) {
    vec4 c = getTextureColor(_texture_index, uv);
    output_color = vec4(_tint_color.rgb + c.rgb*(1.0-_tint_color.a), c.a * _opacity );
  }else{
    discard;
  }
}
