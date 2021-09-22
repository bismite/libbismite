CC=clang
AR=ar
CFLAGS=-Wall -O3
INCLUDE_PATHS=-Ibuild/macos/SDL/include/SDL2

TARGET=build/macos/libbismite.a
OBJ_DIR=build/macos/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SDL_TGZ=build/macos/SDL-macOS-UniversalBinaries.tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-macOS-UniversalBinaries/releases/download/1.1/SDL-macOS-UniversalBinaries.tgz
SDL_DIR=build/macos/SDL

SAMPLE_DIR=build/macos/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LIBS = -lSDL2 -lSDL2_image -framework OpenGL -lbismite
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

# ----

all: $(OBJ_DIR) $(SDL_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl
clean:
	rm -rf build/macos

$(SDL_TGZ):
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))

$(SDL_DIR): $(SDL_TGZ)
	mkdir -p $@
	tar --strip-component 1 -zxf $(SDL_TGZ) -C $@

$(OBJ_DIR):
	mkdir -p $@/ext

$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.exe: samples/src/%.c
	$(CC) $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) -Lbuild/macos/SDL/lib -Lbuild/macos
	install_name_tool -add_rpath @executable_path/lib $@

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)
copysdl:
	cp -R build/macos/SDL/lib build/macos/SDL/licenses $(SAMPLE_DIR)
