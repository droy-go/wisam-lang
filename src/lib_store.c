#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#define STORE_DIR "./wisam_data/"
#define MAX_STORES 100

typedef struct {
    char *name;
    char *filename;
    bool encrypted;
} Store;

static Store stores[MAX_STORES];
static int store_count = 0;
static bool initialized = false;

void lib_store_init(void) {
    if (initialized) return;
    
    // إنشاء مجلد التخزين
    struct stat st = {0};
    if (stat(STORE_DIR, &st) == -1) {
        mkdir(STORE_DIR, 0700);
    }
    
    initialized = true;
}

// إنشاء مخزن جديد
Value lib_store_create(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: أنشئ يتطلب اسم المخزن\n");
        return value_create_null();
    }
    
    char *name = args[0].as.string;
    
    // التحقق من عدم وجود المخزن مسبقاً
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, name) == 0) {
            fprintf(stderr, "خطأ: المخزن '%s' موجود مسبقاً\n", name);
            return value_create_null();
        }
    }
    
    if (store_count >= MAX_STORES) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للمخازن\n");
        return value_create_null();
    }
    
    // إنشاء المخزن
    stores[store_count].name = strdup(name);
    char filename[512];
    snprintf(filename, sizeof(filename), "%s%s.wdb", STORE_DIR, name);
    stores[store_count].filename = strdup(filename);
    stores[store_count].encrypted = false;
    
    // إنشاء الملف
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "# Wisam Database: %s\n", name);
        fclose(f);
    }
    
    store_count++;
    
    return value_create_string(name);
}

// إضافة بيانات للمخزن
Value lib_store_add(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_STRING || 
        args[1].type != VAL_STRING || args[2].type != VAL_STRING) {
        fprintf(stderr, "خطأ: أضف يتطلب اسم المخزن والمفتاح والقيمة\n");
        return value_create_null();
    }
    
    char *store_name = args[0].as.string;
    char *key = args[1].as.string;
    char *value = args[2].as.string;
    
    // البحث عن المخزن
    char *filename = NULL;
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, store_name) == 0) {
            filename = stores[i].filename;
            break;
        }
    }
    
    if (!filename) {
        fprintf(stderr, "خطأ: المخزن '%s' غير موجود\n", store_name);
        return value_create_null();
    }
    
    // قراءة المحتوى الحالي
    FILE *f = fopen(filename, "r");
    char *content = NULL;
    long content_size = 0;
    
    if (f) {
        fseek(f, 0, SEEK_END);
        content_size = ftell(f);
        fseek(f, 0, SEEK_SET);
        content = malloc(content_size + 1);
        fread(content, 1, content_size, f);
        content[content_size] = '\0';
        fclose(f);
    }
    
    // كتابة البيانات الجديدة
    f = fopen(filename, "w");
    if (!f) {
        fprintf(stderr, "خطأ: لا يمكن فتح المخزن للكتابة\n");
        free(content);
        return value_create_null();
    }
    
    if (content) {
        fprintf(f, "%s", content);
        free(content);
    }
    
    fprintf(f, "%s=%s\n", key, value);
    fclose(f);
    
    return value_create_boolean(true);
}

// قراءة بيانات من المخزن
Value lib_store_read(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: اقرأ يتطلب اسم المخزن والمفتاح\n");
        return value_create_null();
    }
    
    char *store_name = args[0].as.string;
    char *key = args[1].as.string;
    
    // البحث عن المخزن
    char *filename = NULL;
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, store_name) == 0) {
            filename = stores[i].filename;
            break;
        }
    }
    
    if (!filename) {
        fprintf(stderr, "خطأ: المخزن '%s' غير موجود\n", store_name);
        return value_create_null();
    }
    
    // قراءة الملف
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "خطأ: لا يمكن فتح المخزن للقراءة\n");
        return value_create_null();
    }
    
    char line[1024];
    char result[1024] = "";
    int key_len = strlen(key);
    
    while (fgets(line, sizeof(line), f)) {
        // تخطي التعليقات
        if (line[0] == '#') continue;
        
        // البحث عن المفتاح
        if (strncmp(line, key, key_len) == 0 && line[key_len] == '=') {
            strcpy(result, line + key_len + 1);
            // إزالة newline
            result[strcspn(result, "\n")] = '\0';
            break;
        }
    }
    
    fclose(f);
    
    if (strlen(result) == 0) {
        return value_create_null();
    }
    
    return value_create_string(result);
}

// حذف بيانات من المخزن
Value lib_store_delete(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: احذف يتطلب اسم المخزن والمفتاح\n");
        return value_create_null();
    }
    
    char *store_name = args[0].as.string;
    char *key = args[1].as.string;
    
    // البحث عن المخزن
    char *filename = NULL;
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, store_name) == 0) {
            filename = stores[i].filename;
            break;
        }
    }
    
    if (!filename) {
        fprintf(stderr, "خطأ: المخزن '%s' غير موجود\n", store_name);
        return value_create_null();
    }
    
    // قراءة المحتوى
    FILE *f = fopen(filename, "r");
    if (!f) {
        return value_create_null();
    }
    
    char **lines = malloc(sizeof(char*) * 1000);
    int line_count = 0;
    char line[1024];
    int key_len = strlen(key);
    bool found = false;
    
    while (fgets(line, sizeof(line), f)) {
        if (strncmp(line, key, key_len) == 0 && line[key_len] == '=') {
            found = true;
            continue; // تخطي هذا السطر
        }
        lines[line_count] = strdup(line);
        line_count++;
    }
    fclose(f);
    
    if (!found) {
        for (int i = 0; i < line_count; i++) free(lines[i]);
        free(lines);
        return value_create_boolean(false);
    }
    
    // إعادة الكتابة
    f = fopen(filename, "w");
    for (int i = 0; i < line_count; i++) {
        fprintf(f, "%s", lines[i]);
        free(lines[i]);
    }
    fclose(f);
    free(lines);
    
    return value_create_boolean(true);
}

// قائمة جميع المفاتيح في المخزن
Value lib_store_list(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: قائمة يتطلب اسم المخزن\n");
        return value_create_null();
    }
    
    char *store_name = args[0].as.string;
    
    // البحث عن المخزن
    char *filename = NULL;
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, store_name) == 0) {
            filename = stores[i].filename;
            break;
        }
    }
    
    if (!filename) {
        fprintf(stderr, "خطأ: المخزن '%s' غير موجود\n", store_name);
        return value_create_null();
    }
    
    // قراءة الملف
    FILE *f = fopen(filename, "r");
    if (!f) {
        return value_create_array();
    }
    
    Value result = value_create_array();
    char line[1024];
    
    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#') continue;
        
        char *eq = strchr(line, '=');
        if (eq) {
            *eq = '\0';
            Value *item = malloc(sizeof(Value));
            *item = value_create_string(line);
            
            if (result.as.array.count >= result.as.array.capacity) {
                result.as.array.capacity *= 2;
                result.as.array.items = realloc(result.as.array.items, 
                                               sizeof(Value*) * result.as.array.capacity);
            }
            
            result.as.array.items[result.as.array.count++] = item;
        }
    }
    
    fclose(f);
    return result;
}

// مسح المخزن
Value lib_store_clear(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: امسح يتطلب اسم المخزن\n");
        return value_create_null();
    }
    
    char *store_name = args[0].as.string;
    
    // البحث عن المخزن
    char *filename = NULL;
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, store_name) == 0) {
            filename = stores[i].filename;
            break;
        }
    }
    
    if (!filename) {
        fprintf(stderr, "خطأ: المخزن '%s' غير موجود\n", store_name);
        return value_create_null();
    }
    
    // إعادة إنشاء الملف
    FILE *f = fopen(filename, "w");
    if (f) {
        fprintf(f, "# Wisam Database: %s\n", store_name);
        fclose(f);
        return value_create_boolean(true);
    }
    
    return value_create_boolean(false);
}

// حذف المخزن
Value lib_store_remove(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: أزل يتطلب اسم المخزن\n");
        return value_create_null();
    }
    
    char *store_name = args[0].as.string;
    
    // البحث عن المخزن
    int idx = -1;
    for (int i = 0; i < store_count; i++) {
        if (strcmp(stores[i].name, store_name) == 0) {
            idx = i;
            break;
        }
    }
    
    if (idx == -1) {
        fprintf(stderr, "خطأ: المخزن '%s' غير موجود\n", store_name);
        return value_create_null();
    }
    
    // حذف الملف
    remove(stores[idx].filename);
    
    // تحرير الذاكرة
    free(stores[idx].name);
    free(stores[idx].filename);
    
    // إزالة من القائمة
    for (int i = idx; i < store_count - 1; i++) {
        stores[i] = stores[i + 1];
    }
    store_count--;
    
    return value_create_boolean(true);
}
