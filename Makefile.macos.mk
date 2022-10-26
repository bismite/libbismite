BUILD_DIR=build/macos
CC=clang
AR=ar
CFLAGS=-Wall -O3 -fPIC -arch arm64 -arch x86_64
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/SDL/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SDL_TGZ=build/SDL-macOS-UniversalBinaries.tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-macOS-UniversalBinaries/releases/download/1.2/SDL-macOS-UniversalBinaries.tgz
SDL_DIR=$(BUILD_DIR)/SDL

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -L$(BUILD_DIR)/SDL/lib -lSDL2 -lSDL2_image -framework OpenGL -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-macos.tgz

# ----

all: $(OBJ_DIR) $(LIB_DIR) $(SDL_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl
release: all $(ARCHIVE)
clean:
	rm -rf $(BUILD_DIR)

$(SDL_TGZ):
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))

$(SDL_DIR): $(SDL_TGZ)
	mkdir -p $@
	tar --strip-component 1 -zxf $(SDL_TGZ) -C $@

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
	cp -R $(BUILD_DIR)/SDL/lib $(BUILD_DIR)/SDL/licenses $(SAMPLE_DIR)

# ----

$(ARCHIVE):
	cp -R include $(BUILD_DIR)
	cp LICENSE.txt $(BUILD_DIR)/LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) lib LICENSE.txt include
