# Makefile for Wisam Programming Language

CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -I./include
LDFLAGS = -lm -lcurl -lssl -lcrypto

# Source files
SRCS = src/main.c \
       src/lexer.c \
       src/parser.c \
       src/interpreter.c \
       src/lib_text.c \
       src/lib_time.c \
       src/lib_store.c \
       src/lib_ai.c \
       src/lib_media.c \
       src/lib_gui.c \
       src/lib_net.c \
       src/lib_meta.c \
       src/lib_math.c \
       src/lib_list.c \
       src/lib_file.c \
       src/lib_json.c \
       src/lib_crypto.c \
       src/lib_regex.c \
       src/lib_system.c

# Object files
OBJS = $(SRCS:.c=.o)

# Target executable
TARGET = wisam

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "✓ تم بناء وسام بنجاح!"

# Compile source files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -f $(OBJS) $(TARGET)
	@echo "✓ تم تنظيف الملفات"

# Install (requires sudo)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/
	mkdir -p /usr/local/share/wisam
	cp -r include /usr/local/share/wisam/
	@echo "✓ تم تثبيت وسام"

# Uninstall (requires sudo)
uninstall:
	rm -f /usr/local/bin/$(TARGET)
	rm -rf /usr/local/share/wisam
	@echo "✓ تم إلغاء تثبيت وسام"

# Run tests
test: $(TARGET)
	./$(TARGET) examples/test.wsm

# Interactive mode
interactive: $(TARGET)
	./$(TARGET) -i

# Create release build
release: CFLAGS = -Wall -O3 -I./include -DNDEBUG
release: clean $(TARGET)
	strip $(TARGET)
	@echo "✓ تم إنشاء الإصدار النهائي"

# Show help
help:
	@echo "أوامر Makefile:"
	@echo "  make          بناء وسام"
	@echo "  make clean    تنظيف الملفات"
	@echo "  make install  تثبيت وسام (يتطلب sudo)"
	@echo "  make test     تشغيل الاختبارات"
	@echo "  make release  إنشاء إصدار نهائي"
	@echo "  make help     عرض هذه المساعدة"

.PHONY: all clean install uninstall test interactive release help
