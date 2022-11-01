
CC=emcc
AR=emar
INCLUDE_PATHS=-Iinclude

LIB_DIR=build/emscripten/lib
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)

CFLAGS_NOSIMD=-std=gnu11 -Wall -O3 -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png] -fPIC -sMAX_WEBGL_VERSION=2
TARGET_NOSIMD=$(LIB_DIR)/libbismite-nosimd.a
OBJ_DIR_NOSIMD=build/emscripten/objs-nosimd
OBJECTS_NOSIMD = $(SOURCES:src/%.c=$(OBJ_DIR_NOSIMD)/%.o)

CFLAGS=$(CFLAGS_NOSIMD) -msimd128 -msse2 -DWASM_SIMD_ENABLED
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=build/emscripten/objs
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

# ----

SAMPLE_ASSETS = $(wildcard samples/assets/**/*)
SAMPLE_SOURCES = $(wildcard samples/src/*.c)

SAMPLE_CFLAGS_NOSIMD=$(CFLAGS_NOSIMD) -s WASM=1 --embed-file samples/assets@assets -s ALLOW_MEMORY_GROWTH=1 -s SINGLE_FILE=1
SAMPLE_LDFLAGS_NOSIMD =-L$(LIB_DIR) -lbismite-nosimd
SAMPLE_DIR_NOSIMD=build/emscripten/samples-nosimd
SAMPLE_EXES_NOSIMD = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR_NOSIMD)/%.html)

SAMPLE_CFLAGS=$(CFLAGS) -s WASM=1 --embed-file samples/assets@assets -s ALLOW_MEMORY_GROWTH=1 -s SINGLE_FILE=1
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite
SAMPLE_DIR=build/emscripten/samples
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.html)

# ----

ARCHIVE=build/emscripten/libbismite-emscripten.tgz
ARCHIVE_SAMPLES=build/emscripten/libbismite-emscripten-samples.tgz

# ----

all: samples samples-nosimd $(ARCHIVE) $(ARCHIVE_SAMPLES)
lib: $(LIB_DIR) $(OBJ_DIR) $(TARGET)
lib-nosimd: $(LIB_DIR) $(OBJ_DIR_NOSIMD) $(TARGET_NOSIMD)
samples: lib $(SAMPLE_DIR) $(SAMPLE_EXES)
samples-nosimd: lib-nosimd $(SAMPLE_DIR_NOSIMD) $(SAMPLE_EXES_NOSIMD)

clean:
	rm -rf build/emscripten

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@/ext

$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(OBJ_DIR_NOSIMD):
	mkdir -p $@/ext

$(OBJ_DIR_NOSIMD)/%.o: src/%.c
	$(CC) -c $^ -o $@ $(CFLAGS_NOSIMD) $(INCLUDE_PATHS)

$(TARGET_NOSIMD): $(OBJECTS_NOSIMD)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.html: samples/src/%.c
	$(CC) $^ -o $@ $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)

# ----

$(SAMPLE_DIR_NOSIMD):
	mkdir -p $@

$(SAMPLE_DIR_NOSIMD)/%.html: samples/src/%.c
	$(CC) $^ -o $@ $(SAMPLE_CFLAGS_NOSIMD) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS_NOSIMD)

# ----

build/emscripten/licenses/libbismite-LICENSE.txt:
	mkdir -p build/emscripten/licenses
	cp LICENSE.txt build/emscripten/licenses/libbismite-LICENSE.txt

$(ARCHIVE): build/emscripten/licenses/libbismite-LICENSE.txt
	cp -R include build/emscripten
	tar -cz -C build/emscripten -f $(ARCHIVE) lib include licenses

$(ARCHIVE_SAMPLES): build/emscripten/licenses/libbismite-LICENSE.txt
	tar -cz -C build/emscripten -f $(ARCHIVE_SAMPLES) samples samples-nosimd licenses
