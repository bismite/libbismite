BUILD_DIR=build/emscripten

CC=emcc
AR=emar
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)

CFLAGS=-std=gnu11 -Wall -O3 -flto -s USE_SDL=0 -fPIC -msimd128 -msse2 -DWASM_SIMD_ENABLED
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/lib/libSDL2.a
SDL_STATIC_LIBS=$(BUILD_DIR)/lib/libSDL2.a $(BUILD_DIR)/lib/libSDL2_image.a $(BUILD_DIR)/lib/libSDL2_mixer.a
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/20240804-4/emscripten-3.1.64-20240804-4-SDL2-2.30.5-SDL2_image-2.8.2-SDL2_mixer-2.8.0.tgz
SDL_TGZ=build/$(shell basename $(SDL_TGZ_URL))

# ----

SAMPLE_ASSETS = $(wildcard samples/assets/**/*)
SAMPLE_SOURCES = $(wildcard samples/*.c)

SAMPLE_CFLAGS=$(CFLAGS) -s WASM=1 --embed-file samples/assets@assets -s ALLOW_MEMORY_GROWTH=1 -s SINGLE_FILE=1
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite
SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/%.c=$(SAMPLE_DIR)/%.html)

# ----

ARCHIVE=$(BUILD_DIR)/libbismite-emscripten.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-emscripten-samples.tgz

# ----

all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)
lib: $(LIB_DIR) $(OBJ_DIR) $(TARGET)
samples: lib $(SAMPLE_DIR) $(SAMPLE_EXES)

clean:
	rm -rf $(BUILD_DIR)

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@/ext

$(SDL_TGZ):
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))

$(LIBSDL2): $(SDL_TGZ)
	mkdir -p $(BUILD_DIR)
	tar xf $(SDL_TGZ) -C $(BUILD_DIR)

$(OBJ_DIR)/%.o: src/%.c $(LIBSDL2)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.html: samples/%.c $(TARGET)
	$(CC) $< -o $@ $(SDL_STATIC_LIBS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS) -sMAX_WEBGL_VERSION=2

# ----

$(BUILD_DIR)/licenses/libbismite-LICENSE.txt:
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt

$(ARCHIVE): $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	cp -R include $(BUILD_DIR)
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) lib include licenses

$(ARCHIVE_SAMPLES): $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE_SAMPLES) samples licenses
