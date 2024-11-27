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
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/20241117/emscripten-3.1.71-20241117-SDL2-2.30.9-SDL2_image-2.8.2-SDL2_mixer-2.8.0.tgz
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

all: $(ARCHIVE) $(ARCHIVE_SAMPLES)
samples: $(TARGET) $(SAMPLE_EXES)

$(BUILD_DIR):
	mkdir -p $@
$(OBJ_DIR)/ext:
	mkdir -p $@/ext
$(SDL_TGZ): $(BUILD_DIR)
	@echo $(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))
$(LIBSDL2): $(SDL_TGZ)
	tar xf $(SDL_TGZ) -C $(BUILD_DIR)
$(OBJ_DIR)/%.o: src/%.c $(LIBSDL2) $(OBJ_DIR)/ext
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)
$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@
$(SAMPLE_DIR)/%.html: samples/%.c $(TARGET) $(SAMPLE_DIR)
	$(CC) $< -o $@ $(SDL_STATIC_LIBS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS) -sMAX_WEBGL_VERSION=2

# ----

$(BUILD_DIR)/licenses/libbismite-LICENSE.txt:
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt

$(ARCHIVE): $(BUILD_DIR)/licenses/libbismite-LICENSE.txt $(TARGET)
	cp -R include $(BUILD_DIR)
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) lib include licenses

$(ARCHIVE_SAMPLES): $(BUILD_DIR)/licenses/libbismite-LICENSE.txt samples
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE_SAMPLES) samples licenses

clean:
	rm -rf $(BUILD_DIR)/objs/*
	rm -rf $(BUILD_DIR)/lib/*
	rm -rf $(BUILD_DIR)/bin/*
	rm -rf $(BUILD_DIR)/include/*
	rm -rf $(BUILD_DIR)/samples/*
	rm -rf $(BUILD_DIR)/licenses/*
	rm -rf $(BUILD_DIR)/*.tgz
	rm -f $(SDL_TGZ)
