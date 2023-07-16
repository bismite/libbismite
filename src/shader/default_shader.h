
#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 300 es\n"
#define VERTEX_SHADER_HEADER "precision highp float;\n"
#define FRAGMENT_SHADER_HEADER "precision highp float;\n"
#else
#define SHADER_VERSION "#version 410\n"
#define VERTEX_SHADER_HEADER ""
#define FRAGMENT_SHADER_HEADER ""
#endif

#define D(...) #__VA_ARGS__ "\n";

static const char *DEFAULT_VERTEX_SHADER = SHADER_VERSION VERTEX_SHADER_HEADER D(
uniform mat4 camera;
in vec2 vertex;
in vec4 texture_uv;
in mat4 transform;
in int texture_index;
in vec4 tint;
in vec4 modulate;
in mat4 node_extra_data;
out vec2 uv;
flat out int _texture_index;
out vec4 _tint;
out vec4 _modulate;
out mat4 _node_extra_data;
void main()
{
  gl_Position = camera * transform * vec4(vertex,0.0,1.0);
  // vertex = [ left-top, left-bottom, right-top, right-bottom ]
  // texture_uv = [ x:left, y:top, z:right, w:bottom ]
  if( gl_VertexID == 0 ){
    uv = vec2(texture_uv.x,texture_uv.y); // left-top
  }else if( gl_VertexID == 1 ){
    uv = vec2(texture_uv.x,texture_uv.w); // left-bottom
  }else if( gl_VertexID == 2 ){
    uv = vec2(texture_uv.z,texture_uv.y); // right-top
  }else if( gl_VertexID == 3 ){
    uv = vec2(texture_uv.z,texture_uv.w); // right-bottom
  }
  _texture_index = texture_index;
  _tint = tint / 255.0;
  _modulate = modulate / 255.0;
  _node_extra_data = node_extra_data;
}
);

static const char *DEFAULT_FRAGMENT_SHADER = SHADER_VERSION FRAGMENT_SHADER_HEADER D(
in vec2 uv;
flat in int _texture_index;
in vec4 _tint;
in vec4 _modulate;
in mat4 _node_extra_data;
uniform float time;
uniform vec2 resolution;
uniform mat4 layer_extra_data;
uniform sampler2D sampler[16];
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

void main()
{
  vec4 c = vec4(1.0);
  if( 0 <= _texture_index && _texture_index < 16 ) {
    c = getTextureColor( _texture_index, uv);
  }
  c = c * _modulate;
  color = vec4( _tint.rgb*_tint.a*c.a + c.rgb*(1.0-_tint.a), c.a );
}
);

#undef D
