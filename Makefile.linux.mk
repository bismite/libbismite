CC=clang
AR=ar
CFLAGS=-Wall -O3 `sdl2-config --cflags`
INCLUDE_PATHS=

TARGET=build/linux/libbismite.a
OBJ_DIR=build/linux/objs
SOURCES = $(wildcard src/*.c) $(wildcard src/ext/*.c)
OBJECTS = $(SOURCES:src/%.c=$(OBJ_DIR)/%.o)

SAMPLE_DIR=build/linux/samples
SAMPLE_SOURCES = $(wildcard samples/src/*.c)
SAMPLE_EXES = $(SAMPLE_SOURCES:samples/src/%.c=$(SAMPLE_DIR)/%.exe)
SAMPLE_CFLAGS=
SAMPLE_LIBS = `sdl2-config --libs` -lSDL2_image -lSDL2_mixer -lbismite -lm -lGL
SAMPLE_ASSETS = $(wildcard samples/assets/**/*)

# ----

all: $(OBJ_DIR) $(TARGET)
samples: all $(SAMPLE_DIR) $(SAMPLE_EXES) copyassets
clean:
	rm -rf build/linux

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
	$(CC) $^ -o $@ -I include $(CFLAGS) $(SAMPLE_CFLAGS) $(INCLUDE_PATHS) $(SAMPLE_LIBS) -Lbuild/linux

copyassets:
	cp -R samples/assets $(SAMPLE_DIR)
