BUILD_DIR=build/linux

CC=clang
AR=ar
CFLAGS=-Wall -std=c11 -O3 -flto -fPIC -D_REENTRANT
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/include -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/lib/libSDL2.so
SDL_TAG=linux-1.0.7
SDL_TGZ=build/SDL-$(SDL_TAG).tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/$(SDL_TAG)/SDL-$(SDL_TAG).tgz

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite -lSDL2 -lSDL2_image -lSDL2_mixer -lm -lGL '-Wl,-rpath=$$ORIGIN/../lib'
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-linux.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-linux-samples.tgz

# ----

all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)
lib: $(OBJ_DIR) $(LIB_DIR) $(TARGET)
samples: lib $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets

clean:
	rm -rf $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $@/ext

$(SDL_TGZ):
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))

$(LIBSDL2): $(SDL_TGZ)
	mkdir -p $(BUILD_DIR)
	tar xf $(SDL_TGZ) -C $(BUILD_DIR)

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: src/%.c $(LIBSDL2)
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.exe: samples/src/%.c
	$(CC) $< -o $@ $(CFLAGS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)

# ----

$(ARCHIVE):
	cp -R include $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) lib include licenses

$(ARCHIVE_SAMPLES):
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE_SAMPLES) samples licenses
