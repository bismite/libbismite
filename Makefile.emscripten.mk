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

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/%.c=$(SAMPLE_DIR)/%.html)
SAMPLE_CFLAGS=$(CFLAGS) -sWASM=1 -sALLOW_MEMORY_GROWTH=1 -sSINGLE_FILE=1
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite -sMAX_WEBGL_VERSION=2
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

TEST_DIR=$(BUILD_DIR)/test
TEST_SOURCES = $(wildcard test/*.c)
TEST_EXES = $(TEST_SOURCES:test/%.c=$(TEST_DIR)/%.html)
TEST_CFLAGS=$(SAMPLE_CFLAGS)
TEST_LDFLAGS= $(SAMPLE_LDFLAGS)
TEST_ASSETS = $(SAMPLE_ASSETS)

# ----

ARCHIVE=$(BUILD_DIR)/libbismite-emscripten.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-emscripten-samples.tgz

# ----

all: $(ARCHIVE) $(ARCHIVE_SAMPLES) test
samples: $(SAMPLE_EXES)
test: $(TEST_EXES)

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

# ---- Samples

$(SAMPLE_DIR)/%.html: samples/%.c $(TARGET)
	mkdir -p $(SAMPLE_DIR)
	$(CC) $< -o $@ $(SDL_STATIC_LIBS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS) --embed-file samples/assets@assets

# ---- Tests

$(TEST_DIR)/%.html: test/%.c $(TARGET)
	mkdir -p $(TEST_DIR)
	$(CC) $< -o $@ $(SDL_STATIC_LIBS) $(TEST_CFLAGS) $(INCLUDE_PATHS) $(TEST_LDFLAGS) --embed-file samples/assets@assets

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
	rm -rf $(BUILD_DIR)/test/*
	rm -rf $(BUILD_DIR)/licenses/*
	rm -rf $(BUILD_DIR)/*.tgz
	rm -f $(SDL_TGZ)
