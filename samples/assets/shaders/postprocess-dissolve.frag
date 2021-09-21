
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

void main()
{
  vec4 noise = getTextureColor(1, uv.xy);
  vec4 c = getTextureColor(0, uv.xy);
  float level = sin(time*2.0) + 0.1;
  float height = noise.r;

  if( height > level ){
    if( abs(height-level) < 0.01 && c.a > 0.0 ){
      gl_FragColor = vec4(1.0,0.0,0.0,c.a);
    }else{
      gl_FragColor = vec4(c.r, c.g, c.b, c.a);
    }
  }else{
    discard;
  }
}
