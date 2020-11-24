CC=emcc
AR=emar
CFLAGS=-Wall -Oz -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=[png]
INCLUDE_PATHS=

BUILD_DIR=build/emscripten
SOURCES = $(wildcard src/*.c)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))
TARGET=$(BUILD_DIR)/libbismite-core.a

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	rm -rf $(BUILD_DIR)
