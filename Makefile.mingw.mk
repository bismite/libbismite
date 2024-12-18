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
SDL_TGZ_URL=https://github.com/bismite/SDL-binaries/releases/download/20241117/mingw-20241117-SDL2-2.30.9-SDL2_image-2.8.2-SDL2_mixer-2.8.0.tgz
SDL_TGZ=build/$(shell basename $(SDL_TGZ_URL))

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS=-L$(BUILD_DIR)/bin
SAMPLE_LIBS = $(BUILD_DIR)/lib/libbismite.a -lmingw32 $(BUILD_DIR)/lib/libSDL2main.a -lSDL2 -mwindows -lSDL2_mixer -lSDL2_image -lopengl32
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

TEST_DIR=$(BUILD_DIR)/test
TEST_SOURCES = $(wildcard test/*.c)
TEST_EXES = $(TEST_SOURCES:test/%.c=$(TEST_DIR)/%.exe)
TEST_LDFLAGS = $(SAMPLE_LDFLAGS)
TEST_LIBS = $(BUILD_DIR)/lib/libbismite.a -lmingw32 $(BUILD_DIR)/lib/libSDL2main.a -lSDL2 -mwindows -lSDL2_mixer -lSDL2_image -lopengl32
TEST_ASSETS = $(SAMPLE_ASSETS)

ARCHIVE=$(BUILD_DIR)/libbismite-mingw.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-mingw-samples.tgz

# ----

all: $(ARCHIVE) $(ARCHIVE_SAMPLES) test
samples: copy_assets copy_libs $(SAMPLE_EXES)
test: copy_test_assets copy_test_libs $(TEST_EXES)

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

# ---- Sample

$(SAMPLE_DIR)/%.exe: samples/%.c $(TARGET)
	mkdir -p $(SAMPLE_DIR)
	$(CC) $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) $(SAMPLE_LDFLAGS)
copy_assets:
	mkdir -p $(SAMPLE_DIR)
	cp -R samples/assets $(SAMPLE_DIR)
copy_libs: $(TARGET)
	mkdir -p $(SAMPLE_DIR)
	cp -R $(BUILD_DIR)/bin/*.dll $(SAMPLE_DIR)

# ---- Test

$(TEST_DIR)/%.exe: test/%.c $(TARGET)
	mkdir -p $(TEST_DIR)
	$(CC) $< -o $@ $(CFLAGS) $(TEST_CFLAGS) $(INCLUDE_PATHS) $(TEST_LIBS) $(TEST_LDFLAGS)
copy_test_assets:
	mkdir -p $(TEST_DIR)
	cp -R samples/assets $(TEST_DIR)
copy_test_libs: $(TARGET)
	mkdir -p $(TEST_DIR)/lib
	cp -R $(BUILD_DIR)/bin/*.dll $(TEST_DIR)

# ----

$(ARCHIVE): $(TARGET)
	cp -R include $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)/licenses
	cp LICENSE.txt $(BUILD_DIR)/licenses/libbismite-LICENSE.txt
	tar -cz -C $(BUILD_DIR) -f $(ARCHIVE) bin lib include licenses

$(ARCHIVE_SAMPLES): samples
	cp -R include $(BUILD_DIR)
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
