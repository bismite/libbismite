
in vec3 uv;
in vec4 _tint_color;
in float _opacity;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
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

void main()
{
  vec4 c = getTextureColor(int(uv.z), uv.xy);
  c = vec4(_tint_color.rgb + c.rgb*(1.0-_tint_color.a), c.a * _opacity );
  output_color = vec4(c.r, c.g, c.b*0.2, c.a );
}
