
in vec2 uv;
flat in int _texture_index;
in vec4 color;
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

#define ORBIT_R 100.0
#define BALL_SIZE 110.0

void main()
{
  vec2 xy = gl_FragCoord.xy / scale;

  vec4 c = getTextureColor(_texture_index, uv);
  vec2 orbit = vec2(
    (ORBIT_R * sin(time) + resolution.x * 0.5),
    (ORBIT_R * cos(time) + resolution.y * 0.5)
  );
  if( distance(xy,orbit) < BALL_SIZE ) {
    output_color = vec4( 1.0-c.r, 1.0-c.g, 1.0-c.b, 1.0 );
  }else{
    output_color = c;
  }
}
