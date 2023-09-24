ARCH=arm64
BUILD_DIR=build/macos-$(ARCH)
CC=clang
AR=ar
CFLAGS=-std=c11 -Wall -O3 -fPIC -flto -arch $(ARCH) -Wno-deprecated-declarations
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/lib/libSDL2.dylib
SDL_TAG=macos-$(ARCH)-1.0.8
SDL_TGZ=build/SDL-$(SDL_TAG).tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/$(SDL_TAG)/SDL-$(SDL_TAG).tgz

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
samples: libs $(SAMPLE_DIR) copyassets copysdl $(SAMPLE_EXES)

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

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)
copysdl:
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
