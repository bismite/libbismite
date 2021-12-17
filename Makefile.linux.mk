CC=clang
AR=ar
CFLAGS=-Wall -O3 -fPIC `sdl2-config --cflags`
INCLUDE_PATHS=-Iinclude

LIB_DIR=build/linux/lib
TARGET=$(LIB_DIR)/libbismite.a
OBJ_DIR=build/linux/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SAMPLE_DIR=build/linux/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_LDFLAGS =-L$(LIB_DIR) -lbismite `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -lm -lGL
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

ARCHIVE=build/linux/libbismite-linux.tgz

# ----

all: $(OBJ_DIR) $(LIB_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets
release: all $(ARCHIVE)
clean:
	rm -rf build/linux

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
	$(CC) $^ -o $@ -I include $(CFLAGS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LDFLAGS)

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)

# ----

$(ARCHIVE):
	cp -R include build/linux
	cp LICENSE.txt build/linux/LICENSE.txt
	tar -cz -C build/linux -f $(ARCHIVE) lib LICENSE.txt include
