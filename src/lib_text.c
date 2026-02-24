#include "wisam.h"
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// تحويل النص إلى حروف كبيرة
Value lib_text_upper(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: حوّل_إلى_كبير يتطلب نصاً\n");
        return value_create_null();
    }
    
    char *str = strdup(args[0].as.string);
    for (int i = 0; str[i]; i++) {
        // تحويل الحروف الإنجليزية الصغيرة إلى كبيرة
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = str[i] - 'a' + 'A';
        }
    }
    
    Value result = value_create_string(str);
    free(str);
    return result;
}

// تحويل النص إلى حروف صغيرة
Value lib_text_lower(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: حوّل_إلى_صغير يتطلب نصاً\n");
        return value_create_null();
    }
    
    char *str = strdup(args[0].as.string);
    for (int i = 0; str[i]; i++) {
        // تحويل الحروف الإنجليزية الكبيرة إلى صغيرة
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = str[i] - 'A' + 'a';
        }
    }
    
    Value result = value_create_string(str);
    free(str);
    return result;
}

// الحصول على طول النص
Value lib_text_length(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: الطول يتطلب نصاً\n");
        return value_create_null();
    }
    
    return value_create_number(strlen(args[0].as.string));
}

// استخراج جزء من النص
Value lib_text_substring(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_STRING || 
        args[1].type != VAL_NUMBER || args[2].type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: جزء_من_نص يتطلب نص وبداية وطول\n");
        return value_create_null();
    }
    
    char *str = args[0].as.string;
    int start = (int)args[1].as.number;
    int length = (int)args[2].as.number;
    int str_len = strlen(str);
    
    if (start < 0) start = 0;
    if (start >= str_len) return value_create_string("");
    if (start + length > str_len) length = str_len - start;
    
    char *result = malloc(length + 1);
    strncpy(result, str + start, length);
    result[length] = '\0';
    
    Value v = value_create_string(result);
    free(result);
    return v;
}

// استبدال نص بآخر
Value lib_text_replace(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_STRING || 
        args[1].type != VAL_STRING || args[2].type != VAL_STRING) {
        fprintf(stderr, "خطأ: استبدل يتطلب نصاً ونصاً للبحث ونصاً للاستبدال\n");
        return value_create_null();
    }
    
    char *str = args[0].as.string;
    char *old = args[1].as.string;
    char *new_str = args[2].as.string;
    
    // حساب عدد التكرارات
    int count = 0;
    char *tmp = str;
    while ((tmp = strstr(tmp, old)) != NULL) {
        count++;
        tmp += strlen(old);
    }
    
    if (count == 0) return value_create_string(str);
    
    // إنشاء النتيجة
    int result_len = strlen(str) + count * (strlen(new_str) - strlen(old));
    char *result = malloc(result_len + 1);
    
    char *dst = result;
    char *src = str;
    char *found;
    
    while ((found = strstr(src, old)) != NULL) {
        int len = found - src;
        memcpy(dst, src, len);
        dst += len;
        memcpy(dst, new_str, strlen(new_str));
        dst += strlen(new_str);
        src = found + strlen(old);
    }
    
    strcpy(dst, src);
    
    Value v = value_create_string(result);
    free(result);
    return v;
}

// تقسيم النص
Value lib_text_split(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: قسّم يتطلب نصاً وفاصلاً\n");
        return value_create_null();
    }
    
    char *str = args[0].as.string;
    char *delim = args[1].as.string;
    
    Value result = value_create_array();
    char *copy = strdup(str);
    char *token = strtok(copy, delim);
    
    while (token != NULL) {
        Value *item = malloc(sizeof(Value));
        *item = value_create_string(token);
        
        if (result.as.array.count >= result.as.array.capacity) {
            result.as.array.capacity *= 2;
            result.as.array.items = realloc(result.as.array.items, 
                                           sizeof(Value*) * result.as.array.capacity);
        }
        
        result.as.array.items[result.as.array.count++] = item;
        token = strtok(NULL, delim);
    }
    
    free(copy);
    return result;
}

// إزالة المسافات من البداية والنهاية
Value lib_text_trim(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: قص يتطلب نصاً\n");
        return value_create_null();
    }
    
    char *str = args[0].as.string;
    int start = 0;
    int end = strlen(str) - 1;
    
    while (start <= end && isspace((unsigned char)str[start])) start++;
    while (end >= start && isspace((unsigned char)str[end])) end--;
    
    int len = end - start + 1;
    char *result = malloc(len + 1);
    strncpy(result, str + start, len);
    result[len] = '\0';
    
    Value v = value_create_string(result);
    free(result);
    return v;
}

// ترجمة نص (محاكاة)
Value lib_text_translate(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: ترجم يتطلب نصاً ولغة\n");
        return value_create_null();
    }
    
    char *text = args[0].as.string;
    char *to_lang = args[1].as.string;
    
    // محاكاة للترجمة - في التطبيق الحقيقي ستستخدم مكتبة ترجمة
    char result[1024];
    
    if (strcmp(to_lang, "English") == 0 || strcmp(to_lang, "الإنجليزية") == 0) {
        if (strcmp(text, "مرحبا") == 0) {
            strcpy(result, "Hello");
        } else if (strcmp(text, "اسمك") == 0) {
            strcpy(result, "Your name");
        } else {
            snprintf(result, sizeof(result), "[Translated to English: %s]", text);
        }
    } else if (strcmp(to_lang, "العربية") == 0 || strcmp(to_lang, "Arabic") == 0) {
        if (strcmp(text, "Hello") == 0) {
            strcpy(result, "مرحبا");
        } else {
            snprintf(result, sizeof(result), "[مترجم للعربية: %s]", text);
        }
    } else {
        snprintf(result, sizeof(result), "[Translation to %s: %s]", to_lang, text);
    }
    
    return value_create_string(result);
}
