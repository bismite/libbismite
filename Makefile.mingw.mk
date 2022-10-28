BUILD_DIR=build/x86_64-w64-mingw32

CC=x86_64-w64-mingw32-gcc
AR=x86_64-w64-mingw32-ar
CFLAGS=-Wall -O3 -Dmain=SDL_main
INCLUDE_PATHS= -Iinclude -I$(BUILD_DIR)/include/SDL2

LIB_DIR=$(BUILD_DIR)/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=$(BUILD_DIR)/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

LIBSDL2=$(BUILD_DIR)/lib/libSDL2.a
SDL_TGZ=$(BUILD_DIR)/SDL-x86_64-w64-mingw32.tgz
SDL_TGZ_URL=https://github.com/bismite/SDL-x86_64-w64-mingw32/releases/download/1.1.1/SDL-x86_64-w64-mingw32.tgz

SAMPLE_DIR=$(BUILD_DIR)/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS=-L$(BUILD_DIR)/bin -L$(LIB_DIR)
SAMPLE_LIBS = -lbismite -lmingw32 -lSDL2main -lSDL2 -mwindows -lSDL2_mixer -lSDL2_image -lopengl32
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=$(BUILD_DIR)/libbismite-x86_64-w64-mingw32.tgz
ARCHIVE_SAMPLES=$(BUILD_DIR)/libbismite-x86_64-w64-mingw32-samples.tgz

# ----

libs: $(OBJ_DIR) $(LIB_DIR) $(LIBSDL2) $(TARGET)
samples: libs $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets copysdl
all: samples $(ARCHIVE) $(ARCHIVE_SAMPLES)

clean:
	rm -rf $(BUILD_DIR)

$(LIBSDL2):
	mkdir -p $(BUILD_DIR)
	$(shell ./scripts/download.sh $(SDL_TGZ_URL) $(SDL_TGZ))
	tar xf $(SDL_TGZ) -C $(BUILD_DIR)

$(OBJ_DIR):
	mkdir -p $@/ext

$(LIB_DIR):
	mkdir -p $@

$(OBJ_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

# ----

$(SAMPLE_DIR):
	mkdir -p $@

$(SAMPLE_DIR)/%.exe: samples/src/%.c
	$(CC) $^ -o $@ $(CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) $(SAMPLE_LDFLAGS)

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
