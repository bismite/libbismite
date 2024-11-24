# libbismite

# Change Log
## 12.2.0 - 2024/11/25
- BiFramebuffer has BiTextures (max 8).
- remove bi_texture_init_with_framebuffer().
- remove bi_framebuffer_save_png_image().
## 12.1.1 - 2024/11/24
- add : bi_texture_save_png_image(), bi_image_rgba32_flip_vertical(), bi_image_rgba32_save()
- change: bi_texture_save_png_image() and bi_framebuffer_save_png_image() flip vertical.
## 12.0.1 - 2024/11/22
- removed BiLayerGroup and BiCanvas
- BiLayer is renamed to BiShaderNode
- BiNode has BiFramebuffer
- Multiple Render Targets are now supported.
  - BiFramebuffer has multiple textures (MAX 8).
- samples updated.
## 11.0.0 - 2024/11/03
- Update: `node.c` and `shader.c` have been updated. Cropped textures are handled in shaders.
  - new function: `bi_node_set_texture_mapping` in `node.c`.
  - samples update: `shader-collapse`, `texture-mapping`
  - updated sample shaders.
## 10.2.0 - 2024/10/28
- vertex shader send local_xy (normalized, 0.0 to 1.0) to fragment shader.
- send `node_size` for shader
- update sample `shader-collapse.c`
## 10.1.0 - 2024/09/16
- add on_mainloop_end_callback
- add bi_stop_run_loop()
## 10.0.3 - 2024/09/14
- fix Makefile.linux.mk(add -fPIC)
## 10.0.2 - 2024/07/30
- update emscripten 3.1.64
## 10.0.0 - 2024/05/29
- Framework
  - update SDL2 2.30.3, SDL2_image 2.8.2, SDL2_mixer 2.8.0.
  - update emscripten 3.1.59.
  - remove emscripten-nosimd and macos-x86_64.
- Node
  - node: color system update, node#color and node#tint available.
  - ext/TransitionLayer removed.
  - BiNodeBase has children array.
  - layer class inherit BiNodeBase, and no longer has root node.
- Label
  - BiFontAtlas renamed to BiFont.
  - remake BiLabel.
  - add bi_label_deinit().
- remake Postprocesssing feature.
  - add bi_layer_init_as_postprocess().
  - src/render.c : postprocess use glBlitFramebuffer
- misc
  - canvas: add bi_canvas_init_with_framebuffer().
  - add BiColor.
  - timer callback receives context
  - canvas: 4x4 matrix extra data.

## 8.0.2 - 2023/04/10
- BiActionRepeat is no longer available. add repeat flag to actions.
- Fix ScaleBy action and ScaleTo aciton
## 7.0.2 - 2023/04/07
- update SDL 2.26.5
## 7.0.0 - 2023/03/22
- license change : zlib license
## 6.0.5 - 2023/03/20
- update SDL 2.26.4
- split macOS binaries into arm64/x86_64.
## 6.0.3 - 2023/3/3
- update SDL 2.26.3, SDL_image 2.6.3, SDL_mixer 2.6.3.
- use system-independent SDL even on emscripten and linux.
- LTO enabled.
- update canvas samples
## 6.0.2 - 2022/11/19
- update workflows and makefiles
## 6.0.0 - 2022/11/7
- timer system overhauled.
- src/texture.c: SDL_RWFromFile replaced by SDL_RWFromFP
## 5.0.0 - 2022/11/1
- Update to WebGL 2.0
## 4.1.2 - 2022/10/29
- fix sse alignment
## 4.1.1 - 2022/10/26
- SIMD support : WASM, MacOS(arm), x86_64
- src/shader.c : dynamic allocate temporary buffers
## 4.0.0 - 2022/5/10
- TextureMapping removed.
- Cropped Texture supported.
## 3.0.0 - 2022/4/14
- update ext/font
  - organize function names
  - add character color setter
  - add line_x_to_index function for width limit
## 2.0.2 - 2022/1/13
- update bi_get_now()
## 2.0.1 - 2022/1/4
- fix font table range
## 2.0.0 - 2022/1/1
- update font data format, support 32bit unicode.
  - update font and label sample
## 1.2.3 - 2021/12/31
- src/ext/font.c : fix empty white space
- Makefile.macos.mk : update SDL-macOS-UniversalBinaries 1.2
## 1.2.1 - 2021/12/22
- BiTexture: add straight alpha loading. add antialias setter function.
## 1.1.9 - 2021/12/18
- all Node type structures(Node,Layer,LayerGroup) has `void* userdata`
- include/bi/node_base.h : add stdbool.h
- reorganize timer handler.
## 1.1.1 - 2021/12/15
- small updates for actions.
## 1.1.0 - 2021/12/15
- textures convert to premultiplied alpha in loading.
- add canvas system
- timer system overhaul (delta time,saturation,etc)
  - on_update removed from node
  - global timer removed from context
  - add timers to layer and layer group
- update and add some actions
- sprite tinting method changed
## 1.0.8 - 2021/09/23
- ext and samples have been merged.
- renamed to libbismite
## 0.13.0 - 2021/09/07
- GLEW no longer required.
- callback functions for BiNode has been changed.
- Added frame buffer to BiLayerGroup.
- max layer textures increase to 16
- BiLayer: max number of textures has been increased to 16.
- BiLayer can now act as a post-process.
- BiLayerGroup has interaction_enabled flag
- Shader receives display scale as uniform attribute.

# License
Copyright 2018-2024 kbys <work4kbys@gmail.com>

zlib license
