# libbismite

# Change Log
## 1.2.0 - 2021/12/22
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

Copyright 2018-2021 kbys <work4kbys@gmail.com>

Apache License Version 2.0
