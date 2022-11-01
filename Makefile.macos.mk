BUILD_DIR=build/macos

CC=clang
AR=ar
CFLAGS=-Wall -O3 -fPIC -arch arm64 -arch x86_64 -Wno-deprecated-declarations
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=build/macos/lib/libSDL2.dylib
SDL_TGZ=build/SDL-macOS-UniversalBinaries.tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-macOS-UniversalBinaries/releases/download/1.3.1/SDL-macOS-UniversalBinaries.tgz

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lSDL2 -lSDL2_image -framework OpenGL -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-macos.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-macos-samples.tgz

# ----

all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)
libs: $(OBJ_DIR) $(LIB_DIR) $(LIBSDL2) $(TARGET)
samples: libs $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl

clean:
	rm -rf $(BUILD_DIR)

$(SDL_TGZ):
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))

$(LIBSDL2): $(SDL_TGZ)
	mkdir -p $(BUILD_DIR)
	tar --strip-component 1 -zxf $(SDL_TGZ) -C $(BUILD_DIR)

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR):
	mkdir -p $@/ext

$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	libtool -static -o $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.exe: samples/src/%.c
	$(CC) $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)
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
