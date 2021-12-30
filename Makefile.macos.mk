CC=clang
AR=ar
CFLAGS=-Wall -O3 -fPIC -arch arm64 -arch x86_64
INCLUDE_PATHS=-Iinclude -Ibuild/macos/SDL/include/SDL2

LIB_DIR=build/macos/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=build/macos/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SDL_TGZ=build/SDL-macOS-UniversalBinaries.tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-macOS-UniversalBinaries/releases/download/1.2/SDL-macOS-UniversalBinaries.tgz
SDL_DIR=build/macos/SDL

SAMPLE_DIR=build/macos/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -Lbuild/macos/SDL/lib -lSDL2 -lSDL2_image -framework OpenGL -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=build/macos/libbismite-macos.tgz

# ----

all: $(OBJ_DIR) $(LIB_DIR) $(SDL_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl
release: all $(ARCHIVE)
clean:
	rm -rf build/macos

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
	cp -R build/macos/SDL/lib build/macos/SDL/licenses $(SAMPLE_DIR)

# ----

$(ARCHIVE):
	cp -R include build/macos
	cp LICENSE.txt build/macos/LICENSE.txt
	tar -cz -C build/macos -f $(ARCHIVE) lib LICENSE.txt include
