
in vec2 uv;
flat in int _texture_index;
in vec4 _tint;
in vec4 _modulate;
in mat4 _node_extra_data;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
uniform float scale;
uniform mat4 layer_extra_data;
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

const float GRID_SIZE = 50.0;
const float S_GRID_SIZE = 5.0;

void main()
{
  float progress = layer_extra_data[0][0];

  vec2 xy = gl_FragCoord.xy / scale;
  xy = floor(xy / S_GRID_SIZE) * S_GRID_SIZE;
  vec2 tmp = fract(xy / GRID_SIZE);

  if ( tmp.x + tmp.y > progress * 2.0 ) {
    vec4 c = getTextureColor( _texture_index, uv);
    c = c * _modulate;
    color = vec4( _tint.rgb*_tint.a*c.a + c.rgb*(1.0-_tint.a), c.a );
  }else{
    discard;
  }
}
