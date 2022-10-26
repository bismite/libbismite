CC=emcc
AR=emar
CFLAGS=-std=gnu11 -Wall -O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png] -fPIC
INCLUDE_PATHS=-Iinclude

LIB_DIR=build/emscripten-nosimd/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=build/emscripten-nosimd/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SAMPLE_DIR=build/emscripten-nosimd/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.html)
SAMPLE_CFLAGS=-s WASM=1 --embed-file samples/assets@assets -s ALLOW_MEMORY_GROWTH=1 -s SINGLE_FILE=1
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=build/emscripten-nosimd/libbismite-emscripten-nosimd.tgz

# ----

all: $(OBJ_DIR) $(LIB_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES)
release: all $(ARCHIVE)
clean:
	rm -rf build/emscripten-nosimd

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@/ext

$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.html: samples/src/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)

# ----

$(ARCHIVE):
	cp -R include build/emscripten-nosimd
	cp LICENSE.txt build/emscripten-nosimd/LICENSE.txt
	tar -cz -C build/emscripten-nosimd -f $(ARCHIVE) lib LICENSE.txt include
