
uniform mat4 camera;
in vec2 resolution;
in vec2 node_size;
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
out vec2 _node_size;
out vec2 local_xy;
void main()
{
  gl_Position = camera * transform * vec4(vertex,0.0,1.0);
  // vertex = [ left-top, left-bottom, right-top, right-bottom ]
  // texture_uv = [ x:left, y:top, z:right, w:bottom ]
  if( gl_VertexID == 0 ){
    uv = vec2(texture_uv.x,texture_uv.y); // left-top
    local_xy = vec2(0.0,1.0);
  }else if( gl_VertexID == 1 ){
    uv = vec2(texture_uv.x,texture_uv.w); // left-bottom
    local_xy = vec2(0.0,0.0);
  }else if( gl_VertexID == 2 ){
    uv = vec2(texture_uv.z,texture_uv.y); // right-top
    local_xy = vec2(1.0,1.0);
  }else if( gl_VertexID == 3 ){
    uv = vec2(texture_uv.z,texture_uv.w); // right-bottom
    local_xy = vec2(1.0,0.0);
  }
  _texture_index = texture_index;
  _tint = tint / 255.0;
  _modulate = modulate / 255.0;
  _node_extra_data = node_extra_data;
  _node_size = node_size;
}
