ARCH=arm64
BUILD_DIR=build/macos
CC=clang
AR=ar
CFLAGS=-std=c11 -Wall -O2 -arch $(ARCH) -Wno-deprecated-declarations
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/lib/libSDL2.dylib
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/20240804-4/macos-20240804-4-SDL2-2.30.5-SDL2_image-2.8.2-SDL2_mixer-2.8.0.tgz
SDL_TGZ=build/$(shell basename $(SDL_TGZ_URL))

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lSDL2 -lSDL2_image -lSDL2_mixer -framework OpenGL -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-macos.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-macos-samples.tgz

# ----

all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)
libs: $(OBJ_DIR) $(LIB_DIR) $(TARGET)
samples: libs $(SAMPLE_DIR) copy_assets copy_libs $(SAMPLE_EXES)

clean:
	rm -rf $(BUILD_DIR)

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@/ext

$(SDL_TGZ):
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $@)

$(LIBSDL2): $(SDL_TGZ)
	mkdir -p $(BUILD_DIR)
	tar -zxf $(SDL_TGZ) -C $(BUILD_DIR)

$(OBJ_DIR)/%.o: src/%.c $(LIBSDL2)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	libtool -static -o $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.exe: samples/%.c $(TARGET)
	$(CC) $< -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)
	install_name_tool -add_rpath @executable_path/lib $@

copy_assets:
	cp -R samples/assets $(SAMPLE_DIR)

copy_libs:
	mkdir -p $(SAMPLE_DIR)/lib
	cp -R $(BUILD_DIR)/lib/*.dylib $(SAMPLE_DIR)/lib

# ----

$(ARCHIVE):
	cp -R include $(BUILD_DIR)
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $@ lib licenses include

$(ARCHIVE_SAMPLES):
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $@ samples licenses
