
varying vec3 uv;
varying vec4 color;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
uniform vec4 optional_attributes;

vec4 getTextureColor(int samplerID,vec2 xy) {
  if(samplerID==0){ return texture2D(sampler[0], xy); }
  if(samplerID==1){ return texture2D(sampler[1], xy); }
  if(samplerID==2){ return texture2D(sampler[2], xy); }
  if(samplerID==3){ return texture2D(sampler[3], xy); }
  if(samplerID==4){ return texture2D(sampler[4], xy); }
  if(samplerID==5){ return texture2D(sampler[5], xy); }
  if(samplerID==6){ return texture2D(sampler[6], xy); }
  if(samplerID==7){ return texture2D(sampler[7], xy); }
  if(samplerID==8){ return texture2D(sampler[8], xy); }
  if(samplerID==9){ return texture2D(sampler[9], xy); }
  if(samplerID==10){ return texture2D(sampler[10], xy); }
  if(samplerID==11){ return texture2D(sampler[11], xy); }
  if(samplerID==12){ return texture2D(sampler[12], xy); }
  if(samplerID==13){ return texture2D(sampler[13], xy); }
  if(samplerID==14){ return texture2D(sampler[14], xy); }
  if(samplerID==15){ return texture2D(sampler[15], xy); }
  return vec4(0);
}

vec4 blur(int i, vec2 direction,float power)
{
  vec4 c = vec4(0.0);
  vec2 s = direction/resolution;
  float d = power / 110.0;
  for(float q=1.0; q<=10.0; q+=1.0) {
    c += getTextureColor(i, uv.xy + s*q ) * d*(11.0-q);
    c += getTextureColor(i, uv.xy - s*q ) * d*(11.0-q);
  }
  return c;
}

void main()
{
  int i = int(uv.z);
  float power = cos(time*3.0)*0.5 + 0.5;
  vec4 c = getTextureColor(i, uv.xy) * (1.0-power);
  gl_FragColor = c + blur( i, vec2(1.0,0.0), power );;
}
