CC=emcc
AR=emar
CFLAGS=-std=gnu11 -Wall -Oz -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png]
INCLUDE_PATHS=

TARGET=build/emscripten/libbismite.a
OBJ_DIR=build/emscripten/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SAMPLE_DIR=build/emscripten/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.html)
SAMPLE_CFLAGS=-s WASM=1 --embed-file samples/assets@assets -s ALLOW_MEMORY_GROWTH=1 -s SINGLE_FILE=1
SAMPLE_LIBS = -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

# ----

all: $(OBJ_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES)
clean:
	rm -rf build/emscripten

$(OBJ_DIR):
	mkdir -p $@/ext

$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.html: samples/src/%.c
	$(CC) $^ -o $@ -I include $(CFLAGS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) -Lbuild/emscripten
