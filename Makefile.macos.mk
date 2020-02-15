CC=clang
AR=ar
CFLAGS=-Wall -O3
INCLUDE_PATHS=

BUILD_DIR=build/macos
SOURCES = $(wildcard src/*.c)
OBJECTS = $(addprefix $(BUILD_DIR)/, $(notdir $(SOURCES:.c=.o)))
TARGET = $(BUILD_DIR)/libbi.a

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	mkdir -p $@

$(BUILD_DIR)/%.o: src/%.c
	$(CC) -c $^ -o $@ -I include $(CFLAGS) $(INCLUDE_PATHS)

$(TARGET): $(OBJECTS)
	$(AR) rcs $@ $^

clean:
	rm -rf $(BUILD_DIR)
