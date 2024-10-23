
#ifdef __EMSCRIPTEN__
#define SHADER_VERSION "#version 300 es\n"
#define VERTEX_SHADER_HEADER "precision highp float;\n"
#define FRAGMENT_SHADER_HEADER "precision highp float;\n"
#else
#define SHADER_VERSION "#version 410\n"
#define VERTEX_SHADER_HEADER ""
#define FRAGMENT_SHADER_HEADER ""
#endif

#undef D
