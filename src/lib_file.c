#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

// قراءة ملف
Value lib_file_read(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *filename = args[0].as.string;
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "خطأ: لا يمكن فتح الملف '%s'\n", filename);
        return value_create_null();
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    Value result = value_create_string(content);
    free(content);
    return result;
}

// كتابة ملف
Value lib_file_write(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    char *filename = args[0].as.string;
    char *content = args[1].as.string;
    
    FILE *f = fopen(filename, "w");
    if (!f) {
        return value_create_boolean(false);
    }
    
    fprintf(f, "%s", content);
    fclose(f);
    return value_create_boolean(true);
}

// إلحاق بملف
Value lib_file_append(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    char *filename = args[0].as.string;
    char *content = args[1].as.string;
    
    FILE *f = fopen(filename, "a");
    if (!f) {
        return value_create_boolean(false);
    }
    
    fprintf(f, "%s", content);
    fclose(f);
    return value_create_boolean(true);
}

// حذف ملف
Value lib_file_delete(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(remove(args[0].as.string) == 0);
}

// التحقق من وجود ملف
Value lib_file_exists(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    struct stat st;
    return value_create_boolean(stat(args[0].as.string, &st) == 0);
}

// حجم الملف
Value lib_file_size(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_number(-1);
    }
    
    struct stat st;
    if (stat(args[0].as.string, &st) != 0) {
        return value_create_number(-1);
    }
    return value_create_number(st.st_size);
}

// إنشاء مجلد
Value lib_file_mkdir(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    #ifdef _WIN32
    return value_create_boolean(mkdir(args[0].as.string) == 0);
    #else
    return value_create_boolean(mkdir(args[0].as.string, 0755) == 0);
    #endif
}

// حذف مجلد
Value lib_file_rmdir(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    return value_create_boolean(rmdir(args[0].as.string) == 0);
}

// قراءة سطور الملف
Value lib_file_read_lines(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *filename = args[0].as.string;
    FILE *f = fopen(filename, "r");
    if (!f) return value_create_null();
    
    Value result = value_create_array();
    char line[1024];
    
    while (fgets(line, sizeof(line), f)) {
        line[strcspn(line, "\n")] = '\0';
        Value *item = malloc(sizeof(Value));
        *item = value_create_string(line);
        
        if (result.as.array.count >= result.as.array.capacity) {
            result.as.array.capacity *= 2;
            result.as.array.items = realloc(result.as.array.items, 
                                             sizeof(Value*) * result.as.array.capacity);
        }
        result.as.array.items[result.as.array.count++] = item;
    }
    
    fclose(f);
    return result;
}

// كتابة سطور
Value lib_file_write_lines(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_ARRAY) {
        return value_create_boolean(false);
    }
    
    char *filename = args[0].as.string;
    FILE *f = fopen(filename, "w");
    if (!f) return value_create_boolean(false);
    
    for (int i = 0; i < args[1].as.array.count; i++) {
        if (args[1].as.array.items[i]->type == VAL_STRING) {
            fprintf(f, "%s\n", args[1].as.array.items[i]->as.string);
        }
    }
    
    fclose(f);
    return value_create_boolean(true);
}

// نسخ ملف
Value lib_file_copy(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    char *source = args[0].as.string;
    char *dest = args[1].as.string;
    
    FILE *src = fopen(source, "rb");
    if (!src) return value_create_boolean(false);
    
    FILE *dst = fopen(dest, "wb");
    if (!dst) {
        fclose(src);
        return value_create_boolean(false);
    }
    
    char buffer[4096];
    size_t n;
    while ((n = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        fwrite(buffer, 1, n, dst);
    }
    
    fclose(src);
    fclose(dst);
    return value_create_boolean(true);
}

// نقل ملف
Value lib_file_move(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    return value_create_boolean(rename(args[0].as.string, args[1].as.string) == 0);
}

// قائمة ملفات المجلد
Value lib_file_list_dir(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    DIR *dir = opendir(args[0].as.string);
    if (!dir) return value_create_null();
    
    Value result = value_create_array();
    struct dirent *entry;
    
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        Value *item = malloc(sizeof(Value));
        *item = value_create_string(entry->d_name);
        
        if (result.as.array.count >= result.as.array.capacity) {
            result.as.array.capacity *= 2;
            result.as.array.items = realloc(result.as.array.items, 
                                             sizeof(Value*) * result.as.array.capacity);
        }
        result.as.array.items[result.as.array.count++] = item;
    }
    
    closedir(dir);
    return result;
}

// المجلد الحالي
Value lib_file_get_cwd(Value *args, int arg_count) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return value_create_string(cwd);
    }
    return value_create_null();
}

// تغيير المجلد
Value lib_file_set_cwd(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    return value_create_boolean(chdir(args[0].as.string) == 0);
}

// معلومات الملف
Value lib_file_info(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    struct stat st;
    if (stat(args[0].as.string, &st) != 0) {
        return value_create_null();
    }
    
    Value result = value_create_object();
    
    result.as.object.keys[0] = strdup("الحجم");
    result.as.object.values[0] = malloc(sizeof(Value));
    *result.as.object.values[0] = value_create_number(st.st_size);
    
    result.as.object.keys[1] = strdup("تاريخ_التعديل");
    result.as.object.values[1] = malloc(sizeof(Value));
    *result.as.object.values[1] = value_create_number(st.st_mtime);
    
    result.as.object.keys[2] = strdup("هو_ملف");
    result.as.object.values[2] = malloc(sizeof(Value));
    *result.as.object.values[2] = value_create_boolean(S_ISREG(st.st_mode));
    
    result.as.object.keys[3] = strdup("هو_مجلد");
    result.as.object.values[3] = malloc(sizeof(Value));
    *result.as.object.values[3] = value_create_boolean(S_ISDIR(st.st_mode));
    
    result.as.object.count = 4;
    return result;
}
