BUILD_DIR=build/mingw

CC=x86_64-w64-mingw32-gcc
AR=x86_64-w64-mingw32-ar
CFLAGS=-Wall -std=c11 -O2 -Dmain=SDL_main
INCLUDE_PATHS= -Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/bin/SDL2.dll
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/20240804-4/mingw-20240804-4-SDL2-2.30.5-SDL2_image-2.8.2-SDL2_mixer-2.8.0.tgz
SDL_TGZ=build/$(shell basename $(SDL_TGZ_URL))

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS=-L$(BUILD_DIR)/bin
SAMPLE_LIBS = $(BUILD_DIR)/lib/libbismite.a -lmingw32 $(BUILD_DIR)/lib/libSDL2main.a -lSDL2 -mwindows -lSDL2_mixer -lSDL2_image -lopengl32
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-mingw.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-mingw-samples.tgz

# ----

all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)
libs: $(OBJ_DIR) $(LIB_DIR) $(LIBSDL2) $(TARGET)
samples: libs $(SAMPLE_DIR) $(SAMPLE_EXES) copy_assets copy_libs

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

$(SAMPLE_DIR)/%.exe: samples/%.c
	$(CC) $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) $(SAMPLE_LDFLAGS)

copy_assets:
	cp -R samples/assets $(SAMPLE_DIR)
copy_libs:
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
