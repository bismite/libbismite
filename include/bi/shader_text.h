
#ifdef __EMSCRIPTEN__
// #define VERTEX_SHADER_HEADER "#version 100\n#extension GL_EXT_draw_instanced : enable\n"
#define VERTEX_SHADER_HEADER "#version 100\nprecision mediump float;"
#define FRAGMENT_SHADER_HEADER "#version 100\nprecision mediump float;"
#else
#define VERTEX_SHADER_HEADER "#version 120\n"
#define FRAGMENT_SHADER_HEADER "#version 120\n"
#endif

#define D(...) #__VA_ARGS__ "\n";

const char *VERTEX_SHADER = VERTEX_SHADER_HEADER D(
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

const char *FRAGMENT_SHADER = FRAGMENT_SHADER_HEADER D(
varying vec3 uv;
varying vec4 color;
uniform sampler2D sampler0;
uniform sampler2D sampler1;
uniform sampler2D sampler2;
uniform sampler2D sampler3;
uniform sampler2D sampler4;
uniform sampler2D sampler5;
uniform sampler2D sampler6;
uniform sampler2D sampler7;
void main()
{
  int samplerID = int(uv.z);
  if(samplerID==0){
    gl_FragColor = texture2D( sampler0, uv.xy ) * color;
  }else if(samplerID==1){
    gl_FragColor = texture2D( sampler1, uv.xy ) * color;
  }else if(samplerID==2){
    gl_FragColor = texture2D( sampler2, uv.xy ) * color;
  }else if(samplerID==3){
    gl_FragColor = texture2D( sampler3, uv.xy ) * color;
  }else if(samplerID==4){
    gl_FragColor = texture2D( sampler4, uv.xy ) * color;
  }else if(samplerID==5){
    gl_FragColor = texture2D( sampler5, uv.xy ) * color;
  }else if(samplerID==6){
    gl_FragColor = texture2D( sampler6, uv.xy ) * color;
  }else if(samplerID==7){
    gl_FragColor = texture2D( sampler7, uv.xy ) * color;
  }else{
    gl_FragColor = color;
  }
}
);

#undef D
