
in vec2 uv;
flat in int _texture_index;
in vec4 _tint;
in vec4 _modulate;
in mat4 _node_extra_data;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
uniform vec2 viewport_size;
uniform mat4 shader_extra_data;
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

vec4 get_color(int i, vec2 d)
{
  // Support High-DPI
  float scale = viewport_size.x / resolution.x;
  vec2 xy = gl_FragCoord.xy / scale + d;
  vec2 uv = vec2(xy.x / 480.0, xy.y / 320.0 );
  return getTextureColor(i, uv);
}

vec4 blur(int i, float r)
{
  vec4 c = vec4(0.0);
  for(float x=-r; x<=r; x+=1.0){
    for(float y=-r; y<=r; y+=1.0){
      c += get_color(i, vec2(x,y) );
    }
  }
  c = c / pow(r*2.0+1.0, 2.0);
  return vec4(c.r, c.g, c.b, 1.0);
}

void main()
{
  vec4 mask_color = getTextureColor(1, uv);
  float power = mask_color[0];
  if(power>0.0){
    color = blur(0, power * 8.0);
  }else{
    color = getTextureColor(0, uv);
  }
}
