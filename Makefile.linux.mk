BUILD_DIR=build/linux

CC=clang
AR=ar
CFLAGS=-Wall -std=c11 -O2 -fPIC -D_REENTRANT
INCLUDE_PATHS=-Iinclude -I$(BUILD_DIR)/include -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/lib/libSDL2.so
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/20241117/linux-20241117-SDL2-2.30.9-SDL2_image-2.8.2-SDL2_mixer-2.8.0.tgz
SDL_TGZ=build/$(shell basename $(SDL_TGZ_URL))

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite -lSDL2 -lSDL2_image -lSDL2_mixer -lm -lGL '-Wl,-rpath=$$ORIGIN/lib'
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-linux.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-linux-samples.tgz

# ----

all: $(ARCHIVE) $(ARCHIVE_SAMPLES)
samples: $(TARGET) $(SAMPLE_DIR) $(SAMPLE_EXES) copy_assets copy_libs

$(BUILD_DIR):
	mkdir -p $@
$(OBJ_DIR)/ext:
	mkdir -p $@/ext
$(SDL_TGZ): $(BUILD_DIR)
	@echo $(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))
$(LIBSDL2): $(SDL_TGZ)
	tar xf $(SDL_TGZ) -C $(BUILD_DIR)
$(OBJ_DIR)/%.o: src/%.c $(LIBSDL2) $(OBJ_DIR)/ext
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)
$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@
$(SAMPLE_DIR)/%.exe: samples/%.c $(TARGET) $(SAMPLE_DIR)
	$(CC) $< -o $@ $(CFLAGS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)
copy_assets:
	cp -R samples/assets $(SAMPLE_DIR)
copy_libs: $(TARGET)
	mkdir -p $(SAMPLE_DIR)/lib
	cp build/linux/lib/*.so $(SAMPLE_DIR)/lib/

# ----

$(ARCHIVE): $(TARGET)
	cp -R include $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) lib include licenses

$(ARCHIVE_SAMPLES): samples
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE_SAMPLES) samples licenses

clean:
	rm -rf $(BUILD_DIR)/objs/*
	rm -rf $(BUILD_DIR)/lib/*
	rm -rf $(BUILD_DIR)/bin/*
	rm -rf $(BUILD_DIR)/include/*
	rm -rf $(BUILD_DIR)/samples/*
	rm -rf $(BUILD_DIR)/licenses/*
	rm -rf $(BUILD_DIR)/*.tgz
	rm -f $(SDL_TGZ)
