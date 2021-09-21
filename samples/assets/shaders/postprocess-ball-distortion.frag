
varying vec3 uv;
varying vec4 color;
uniform sampler2D sampler[16];
uniform float time;
uniform vec2 resolution;
uniform float scale;
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

#define AMPLITUDE 40.0
#define ORBIT_R 100.0
#define BALL_SIZE 110.0

void main()
{
  vec2 xy = gl_FragCoord.xy / scale;

  vec2 orbit = vec2(
    ORBIT_R * cos(time) + resolution.x * 0.5,
    ORBIT_R * sin(time) + resolution.y * 0.5
  );
  if( distance(xy,orbit) < BALL_SIZE ) {
    float offset = cos(uv.x * 10 + time*5.0) * AMPLITUDE / resolution.y;
    gl_FragColor = getTextureColor(int(uv.z), vec2(uv.x,uv.y+offset) );
  } else {
    gl_FragColor = getTextureColor(int(uv.z), uv.xy);
  }
}
