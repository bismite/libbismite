CC=x86_64-w64-mingw32-gcc
AR=x86_64-w64-mingw32-ar
CFLAGS=-Wall -O3 `/usr/local/x86_64-w64-mingw32/bin/sdl2-config --cflags`
INCLUDE_PATHS=-I /usr/local/x86_64-w64-mingw32/include

BUILD_DIR=build/mingw
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
