CC=x86_64-w64-mingw32-gcc
AR=x86_64-w64-mingw32-ar
CFLAGS=-Wall -O3 -Dmain=SDL_main
INCLUDE_PATHS=-I build/x86_64-w64-mingw32/SDL/include/SDL2

TARGET=build/x86_64-w64-mingw32/libbismite.a
OBJ_DIR=build/x86_64-w64-mingw32/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SDL_TGZ=build/x86_64-w64-mingw32/SDL-x86_64-w64-mingw32.tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-x86_64-w64-mingw32/releases/download/0.2.0/SDL-x86_64-w64-mingw32.tgz
SDL_DIR=build/x86_64-w64-mingw32/SDL

SAMPLE_DIR=build/x86_64-w64-mingw32/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS=-Lbuild/x86_64-w64-mingw32/SDL/lib -Lbuild/x86_64-w64-mingw32/SDL/bin -Lbuild/x86_64-w64-mingw32
SAMPLE_LIBS = -lbismite -lmingw32 -lSDL2main -lSDL2 -mwindows -lSDL2_mixer -lSDL2_image -lopengl32
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

# ----

all: $(OBJ_DIR) $(SDL_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl
clean:
	rm -rf build/x86_64-w64-mingw32

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
	$(CC) $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) $(SAMPLE_LDFLAGS)

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)
copysdl:
	cp -R build/x86_64-w64-mingw32/SDL/bin/*.dll build/x86_64-w64-mingw32/SDL/licenses $(SAMPLE_DIR)
