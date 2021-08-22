
#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 100\n"
#define VERTEX_SHADER_HEADER "precision highp float;\n"
#define FRAGMENT_SHADER_HEADER "precision highp float;\n"
#else
#define SHADER_VERSION "#version 120\n"
#define VERTEX_SHADER_HEADER ""
#define FRAGMENT_SHADER_HEADER ""
#endif

#define D(...) #__VA_ARGS__ "\n";

static const char *DEFAULT_VERTEX_SHADER = SHADER_VERSION VERTEX_SHADER_HEADER D(
uniform mat4 projection;
uniform mat4 view;
attribute vec2 vertex;
attribute vec4 texture_uv;
attribute vec4 transform_a;
attribute vec4 transform_b;
attribute vec4 transform_c;
attribute vec4 transform_d;
attribute float vertex_index;
attribute float texture_z;
attribute vec4 mod_color;
varying vec3 uv;
varying vec4 color;
void main()
{
  gl_Position = projection * view * mat4(transform_a,transform_b,transform_c,transform_d) * vec4(vertex,0.0,1.0);

  // vertex = [ left-top, left-bottom, right-top, right-bottom ]
  // texture_uv = [ x:left, y:top, z:right, w:bottom ]
  int vertexid = int(vertex_index);
  if( vertexid == 0 ){
    uv = vec3(texture_uv.x,texture_uv.y,texture_z); // left-top
  }else if( vertexid == 1 ){
    uv = vec3(texture_uv.x,texture_uv.w,texture_z); // left-bottom
  }else if( vertexid == 2 ){
    uv = vec3(texture_uv.z,texture_uv.y,texture_z); // right-top
  }else if( vertexid == 3 ){
    uv = vec3(texture_uv.z,texture_uv.w,texture_z); // right-bottom
  }

  //
  color = mod_color;
}
);

static const char *DEFAULT_FRAGMENT_SHADER = SHADER_VERSION FRAGMENT_SHADER_HEADER D(
varying vec3 uv;
varying vec4 color;
uniform float time;
uniform vec2 resolution;
uniform vec4 optional_attributes;
uniform sampler2D sampler[16];

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
  int samplerID = int(uv.z);
  if( 0 <= samplerID && samplerID < 16 ) {
    gl_FragColor = getTextureColor(samplerID, uv.xy) * color;
  }else{
    gl_FragColor = color;
  }
}
);

#undef D
