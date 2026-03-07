# Wisam Programming Language Makefile
# لغة برمجة وسام

CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I./include
LDFLAGS = -lm

# Platform detection
ifeq ($(OS),Windows_NT)
    TARGET = wisam.exe
    LDFLAGS += -lws2_32
    RM = del /Q
else
    TARGET = wisam
    LDFLAGS += -lpthread
    RM = rm -f
endif

# Source files
SRC_DIR = src
STDLIB_DIR = stdlib
BUILD_DIR = build

SRCS = $(SRC_DIR)/main.c \
       $(SRC_DIR)/lexer.c \
       $(SRC_DIR)/parser.c \
       $(SRC_DIR)/interpreter.c \
       $(STDLIB_DIR)/text.c \
       $(STDLIB_DIR)/time.c \
       $(STDLIB_DIR)/storage.c \
       $(STDLIB_DIR)/network.c \
       $(STDLIB_DIR)/media.c \
       $(STDLIB_DIR)/ui.c \
       $(STDLIB_DIR)/ai.c

OBJS = $(patsubst %.c,$(BUILD_DIR)/%.o,$(notdir $(SRCS)))

# Default target
all: $(BUILD_DIR) $(TARGET)

# Create build directory
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Link executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "✅ Built $(TARGET) successfully!"

# Compile source files
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/lexer.o: $(SRC_DIR)/lexer.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/parser.o: $(SRC_DIR)/parser.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/interpreter.o: $(SRC_DIR)/interpreter.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/text.o: $(STDLIB_DIR)/text.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/time.o: $(STDLIB_DIR)/time.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/storage.o: $(STDLIB_DIR)/storage.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/network.o: $(STDLIB_DIR)/network.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/media.o: $(STDLIB_DIR)/media.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ui.o: $(STDLIB_DIR)/ui.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/ai.o: $(STDLIB_DIR)/ai.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run examples
run-hello: $(TARGET)
	./$(TARGET) examples/hello.wsm

run-calculator: $(TARGET)
	./$(TARGET) examples/calculator.wsm

run-loops: $(TARGET)
	./$(TARGET) examples/loops.wsm

run-structs: $(TARGET)
	./$(TARGET) examples/structs.wsm

# Interactive mode
shell: $(TARGET)
	./$(TARGET) -i

# Debug mode (with tokens and AST)
debug-hello: $(TARGET)
	./$(TARGET) -t -a examples/hello.wsm

# Clean build files
clean:
	$(RM) $(BUILD_DIR)/*.o
	$(RM) $(TARGET)
	@echo "🧹 Cleaned build files"

# Install (Linux/Mac)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	@echo "📦 Installed $(TARGET) to /usr/local/bin/"

# Uninstall
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	@echo "🗑️  Uninstalled $(TARGET)"

# Test
test: $(TARGET)
	@echo "🧪 Running tests..."
	./$(TARGET) examples/hello.wsm
	./$(TARGET) examples/calculator.wsm
	./$(TARGET) examples/loops.wsm
	./$(TARGET) examples/structs.wsm
	@echo "✅ All tests passed!"

# Show help
help:
	@echo "Wisam Programming Language - Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make              - Build the Wisam interpreter"
	@echo "  make run-hello    - Run hello.wsm example"
	@echo "  make run-loops    - Run loops.wsm example"
	@echo "  make shell        - Start interactive shell"
	@echo "  make clean        - Clean build files"
	@echo "  make install      - Install to /usr/local/bin"
	@echo "  make test         - Run all examples"
	@echo ""

.PHONY: all clean install uninstall test help shell run-hello run-calculator run-loops run-structs debug-hello
