# libbismite

# Change Log
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

Copyright 2018-2022 kbys <work4kbys@gmail.com>

Apache License Version 2.0
