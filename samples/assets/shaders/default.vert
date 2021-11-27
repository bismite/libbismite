
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
attribute float opacity;
attribute vec4 tint_color;
varying vec3 uv;
varying vec4 _tint_color;
varying float _opacity;

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

  _tint_color = tint_color;
  _opacity = opacity;
}
