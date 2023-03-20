BUILD_DIR=build/x86_64-w64-mingw32

CC=x86_64-w64-mingw32-gcc
AR=x86_64-w64-mingw32-ar
CFLAGS=-Wall -std=c11 -O3 -flto -Dmain=SDL_main
INCLUDE_PATHS= -Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/bin/SDL2.dll
SDL_TAG=mingw-1.0.2
SDL_TGZ=build/SDL-$(SDL_TAG).tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/$(SDL_TAG)/SDL-$(SDL_TAG).tgz

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS=-L$(BUILD_DIR)/bin
SAMPLE_LIBS = $(BUILD_DIR)/lib/libbismite.a -lmingw32 $(BUILD_DIR)/lib/libSDL2main.a -lSDL2 -mwindows -lSDL2_mixer -lSDL2_image -lopengl32
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-x86_64-w64-mingw32.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-x86_64-w64-mingw32-samples.tgz

# ----

all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)
libs: $(OBJ_DIR) $(LIB_DIR) $(LIBSDL2) $(TARGET)
samples: libs $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl

clean:
	rm -rf $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $@/ext

$(LIB_DIR):
	mkdir -p $@

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

$(SAMPLE_DIR)/%.exe: samples/src/%.c
	$(CC) $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) $(SAMPLE_LDFLAGS)

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)
copysdl:
	cp -R $(BUILD_DIR)/bin/*.dll $(SAMPLE_DIR)

# ----

$(ARCHIVE):
	cp -R include $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) bin lib include licenses

$(ARCHIVE_SAMPLES):
	cp -R include $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE_SAMPLES) samples licenses
