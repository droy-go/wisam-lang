# Makefile for Wisam Programming Language

CC = gcc
CFLAGS = -Wall -Wextra -O2 -std=c99 -I./include
LDFLAGS = -lm
DEBUG_FLAGS = -g -DDEBUG

# Directories
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin
EXAMPLES_DIR = examples
TESTS_DIR = tests

# Source files
SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))

# Target executable
TARGET = $(BIN_DIR)/wisam

# Library files
LIB_SOURCES = $(filter-out $(SRC_DIR)/main.c, $(SOURCES))
LIB_OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(LIB_SOURCES))

# Colors for output
RED = \033[0;31m
GREEN = \033[0;32m
YELLOW = \033[0;33m
BLUE = \033[0;34m
NC = \033[0m # No Color

.PHONY: all clean install uninstall test debug docs

# Default target
all: directories $(TARGET)
	@echo "$(GREEN)✓ تم بناء لغة وسام بنجاح!$(NC)"

# Create necessary directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)
	@echo "$(BLUE)📁 تم إنشاء المجلدات$(NC)"

# Build the main executable
$(TARGET): $(OBJECTS)
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "$(GREEN)⚙️ تم بناء المفسر$(NC)"

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "$(BLUE)🔨 $(notdir $<)$(NC)"

# Debug build
debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all
	@echo "$(YELLOW)🐛 وضع التصحيح مفعل$(NC)"

# Run tests
test: all
	@echo "$(BLUE)🧪 جاري تشغيل الاختبارات...$(NC)"
	@for test in $(TESTS_DIR)/*.wsm; do \
		if [ -f "$$test" ]; then \
			echo "$(YELLOW)تشغيل: $$test$(NC)"; \
			$(TARGET) "$$test"; \
		fi; \
	done
	@echo "$(GREEN)✓ تم الانتهاء من الاختبارات$(NC)"

# Run examples
examples: all
	@echo "$(BLUE)📚 جاري تشغيل الأمثلة...$(NC)"
	@for example in $(EXAMPLES_DIR)/*.wsm; do \
		if [ -f "$$example" ]; then \
			echo "$(YELLOW)تشغيل: $$example$(NC)"; \
			$(TARGET) "$$example"; \
			echo ""; \
		fi; \
	done
	@echo "$(GREEN)✓ تم الانتهاء من الأمثلة$(NC)"

# Install to system
install: all
	@echo "$(BLUE)📥 جاري التثبيت...$(NC)"
	@cp $(TARGET) /usr/local/bin/
	@mkdir -p /usr/local/share/wisam
	@cp -r $(EXAMPLES_DIR) /usr/local/share/wisam/
	@cp -r $(INC_DIR) /usr/local/share/wisam/
	@echo "$(GREEN)✓ تم التثبيت بنجاح!$(NC)"
	@echo "$(GREEN)يمكنك الآن استخدام 'wisam' من أي مكان$(NC)"

# Uninstall from system
uninstall:
	@echo "$(BLUE)🗑️ جاري إلغاء التثبيت...$(NC)"
	@rm -f /usr/local/bin/wisam
	@rm -rf /usr/local/share/wisam
	@echo "$(GREEN)✓ تم إلغاء التثبيت$(NC)"

# Clean build files
clean:
	@echo "$(YELLOW)🧹 جاري التنظيف...$(NC)"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "$(GREEN)✓ تم التنظيف$(NC)"

# Generate documentation
docs:
	@echo "$(BLUE)📖 جاري إنشاء التوثيق...$(NC)"
	@echo "# توثيق لغة وسام" > $(DOCS_DIR)/API.md
	@echo "" >> $(DOCS_DIR)/API.md
	@echo "## المكتبات المتاحة" >> $(DOCS_DIR)/API.md
	@echo "" >> $(DOCS_DIR)/API.md
	@echo "### مكتبة النصوص" >> $(DOCS_DIR)/API.md
	@echo "- حوّل_إلى_كبير(نص)" >> $(DOCS_DIR)/API.md
	@echo "- حوّل_إلى_صغير(نص)" >> $(DOCS_DIR)/API.md
	@echo "- الطول(نص)" >> $(DOCS_DIR)/API.md
	@echo "" >> $(DOCS_DIR)/API.md
	@echo "$(GREEN)✓ تم إنشاء التوثيق$(NC)"

# Show help
help:
	@echo "$(BLUE)لغة وسام - نظام البناء$(NC)"
	@echo ""
	@echo "الأوامر المتاحة:"
	@echo "  $(GREEN)make$(NC)           بناء اللغة"
	@echo "  $(GREEN)make debug$(NC)     بناء مع خاصية التصحيح"
	@echo "  $(GREEN)make test$(NC)      تشغيل الاختبارات"
	@echo "  $(GREEN)make examples$(NC)  تشغيل الأمثلة"
	@echo "  $(GREEN)make install$(NC)   تثبيت على النظام"
	@echo "  $(GREEN)make uninstall$(NC) إلغاء التثبيت"
	@echo "  $(GREEN)make clean$(NC)     تنظيف الملفات المؤقتة"
	@echo "  $(GREEN)make docs$(NC)      إنشاء التوثيق"
	@echo "  $(GREEN)make help$(NC)      عرض هذه المساعدة"
	@echo ""
	@echo "أمثلة الاستخدام:"
	@echo "  $(YELLOW)make && ./bin/wisam examples/hello.wsm$(NC)"
	@echo "  $(YELLOW)make debug && gdb ./bin/wisam$(NC)"
	@echo "  $(YELLOW)sudo make install$(NC)"

# Print version
version:
	@echo "$(BLUE)لغة وسام$(NC) $(GREEN)v2.0$(NC) - الإصدار الذهبي"
