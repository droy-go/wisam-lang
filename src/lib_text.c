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
        // ملاحظة: الحروف العربية لا يوجد لها حالة كبيرة/صغيرة
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
    
    const char *str = args[0].as.string;
    int start = (int)args[1].as.number;
    int length = (int)args[2].as.number;
    int str_len = strlen(str);
    
    // تعديل البداية إذا كانت سالبة
    if (start < 0) start = 0;
    if (start >= str_len) return value_create_string("");
    
    // تعديل الطول إذا تجاوز النص
    if (start + length > str_len) {
        length = str_len - start;
    }
    
    char *result = malloc(length + 1);
    strncpy(result, str + start, length);
    result[length] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// استبدال نص بآخر
Value lib_text_replace(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_STRING || 
        args[1].type != VAL_STRING || args[2].type != VAL_STRING) {
        fprintf(stderr, "خطأ: استبدل يتطلب نص ونص_قديم ونص_جديد\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *old_str = args[1].as.string;
    const char *new_str = args[2].as.string;
    
    // عدد الظهور
    int count = 0;
    const char *tmp = str;
    while ((tmp = strstr(tmp, old_str)) != NULL) {
        count++;
        tmp++;
    }
    
    if (count == 0) return value_create_string(str);
    
    // حساب الطول الجديد
    int old_len = strlen(old_str);
    int new_len = strlen(new_str);
    int result_len = strlen(str) + count * (new_len - old_len);
    
    char *result = malloc(result_len + 1);
    char *dst = result;
    const char *src = str;
    
    while ((tmp = strstr(src, old_str)) != NULL) {
        int prefix_len = tmp - src;
        memcpy(dst, src, prefix_len);
        dst += prefix_len;
        memcpy(dst, new_str, new_len);
        dst += new_len;
        src = tmp + old_len;
    }
    
    strcpy(dst, src);
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// تقسيم النص
Value lib_text_split(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: قسّم يتطلب نص وفاصل\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *delimiter = args[1].as.string;
    
    Value result = value_create_array();
    
    if (strlen(delimiter) == 0) {
        // تقسيم إلى حروف
        for (int i = 0; str[i]; i++) {
            char ch[2] = {str[i], '\0'};
            result.as.array.items[result.as.array.count] = malloc(sizeof(Value));
            *result.as.array.items[result.as.array.count] = value_create_string(ch);
            result.as.array.count++;
        }
        return result;
    }
    
    // تقسيم بالفاصل
    char *str_copy = strdup(str);
    char *token = strtok(str_copy, delimiter);
    
    while (token != NULL) {
        result.as.array.items[result.as.array.count] = malloc(sizeof(Value));
        *result.as.array.items[result.as.array.count] = value_create_string(token);
        result.as.array.count++;
        token = strtok(NULL, delimiter);
    }
    
    free(str_copy);
    return result;
}

// إزالة المسافات من البداية والنهاية
Value lib_text_trim(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: قص يتطلب نصاً\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    int start = 0;
    int end = strlen(str) - 1;
    
    // إزالة من البداية
    while (start <= end && isspace((unsigned char)str[start])) start++;
    
    // إزالة من النهاية
    while (end >= start && isspace((unsigned char)str[end])) end--;
    
    if (start > end) return value_create_string("");
    
    int len = end - start + 1;
    char *result = malloc(len + 1);
    strncpy(result, str + start, len);
    result[len] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// ترجمة النص (محاكاة)
Value lib_text_translate(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: ترجم يتطلب نص ولغة_هدف\n");
        return value_create_null();
    }
    
    // هذه وظيفة محاكاة - في التطبيق الحقيقي ستتصل بخدمة ترجمة
    const char *str = args[0].as.string;
    const char *target_lang = args[1].as.string;
    
    // قاموس بسيط للكلمات الشائعة
    if (strcmp(target_lang, "en") == 0) {
        if (strcmp(str, "مرحبا") == 0) return value_create_string("hello");
        if (strcmp(str, "شكرا") == 0) return value_create_string("thank you");
        if (strcmp(str, "نعم") == 0) return value_create_string("yes");
        if (strcmp(str, "لا") == 0) return value_create_string("no");
    } else if (strcmp(target_lang, "ar") == 0) {
        if (strcmp(str, "hello") == 0) return value_create_string("مرحبا");
        if (strcmp(str, "thank you") == 0) return value_create_string("شكرا");
        if (strcmp(str, "yes") == 0) return value_create_string("نعم");
        if (strcmp(str, "no") == 0) return value_create_string("لا");
    }
    
    // إرجاع النص الأصلي إذا لم يتم العثور على ترجمة
    return value_create_string(str);
}

// التحقق من بداية النص
Value lib_text_starts_with(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: يبدأ_بـ يتطلب نص وبادئة\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *prefix = args[1].as.string;
    
    return value_create_boolean(strncmp(str, prefix, strlen(prefix)) == 0);
}

// التحقق من نهاية النص
Value lib_text_ends_with(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: ينتهي_بـ يتطلب نص ولاحقة\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *suffix = args[1].as.string;
    
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) return value_create_boolean(false);
    
    return value_create_boolean(strcmp(str + str_len - suffix_len, suffix) == 0);
}

// التحقق من وجود نص داخل نص
Value lib_text_contains(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: يحتوي_على يتطلب نص ونص_فرعي\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *substring = args[1].as.string;
    
    return value_create_boolean(strstr(str, substring) != NULL);
}

// البحث عن موضع نص
Value lib_text_index_of(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: موضع يتطلب نص ونص_للبحث\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *substring = args[1].as.string;
    
    const char *found = strstr(str, substring);
    if (found == NULL) return value_create_number(-1);
    
    return value_create_number(found - str);
}

// البحث عن آخر موضع
Value lib_text_last_index_of(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: آخر_موضع يتطلب نص ونص_للبحث\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    const char *substring = args[1].as.string;
    
    const char *last_found = NULL;
    const char *current = str;
    
    while ((current = strstr(current, substring)) != NULL) {
        last_found = current;
        current++;
    }
    
    if (last_found == NULL) return value_create_number(-1);
    
    return value_create_number(last_found - str);
}

// تكرار النص
Value lib_text_repeat(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: كرّر يتطلب نص وعدد\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    int count = (int)args[1].as.number;
    
    if (count <= 0) return value_create_string("");
    
    int len = strlen(str);
    int result_len = len * count;
    char *result = malloc(result_len + 1);
    
    for (int i = 0; i < count; i++) {
        memcpy(result + i * len, str, len);
    }
    result[result_len] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// إضافة حشو من البداية
Value lib_text_pad_start(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: حشو_من_البداية يتطلب نص وطول\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    int target_length = (int)args[1].as.number;
    const char *pad_string = (arg_count >= 3 && args[2].type == VAL_STRING) ? 
                              args[2].as.string : " ";
    
    int str_len = strlen(str);
    if (str_len >= target_length) return value_create_string(str);
    
    int pad_len = strlen(pad_string);
    int total_pad = target_length - str_len;
    int result_len = target_length;
    
    char *result = malloc(result_len + 1);
    
    // إضافة الحشو
    for (int i = 0; i < total_pad; i++) {
        result[i] = pad_string[i % pad_len];
    }
    
    // إضافة النص الأصلي
    strcpy(result + total_pad, str);
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// إضافة حشو من النهاية
Value lib_text_pad_end(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: حشو_من_النهاية يتطلب نص وطول\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    int target_length = (int)args[1].as.number;
    const char *pad_string = (arg_count >= 3 && args[2].type == VAL_STRING) ? 
                              args[2].as.string : " ";
    
    int str_len = strlen(str);
    if (str_len >= target_length) return value_create_string(str);
    
    int pad_len = strlen(pad_string);
    int total_pad = target_length - str_len;
    int result_len = target_length;
    
    char *result = malloc(result_len + 1);
    
    // إضافة النص الأصلي
    strcpy(result, str);
    
    // إضافة الحشو
    for (int i = 0; i < total_pad; i++) {
        result[str_len + i] = pad_string[i % pad_len];
    }
    result[result_len] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// عكس النص
Value lib_text_reverse(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: اقلب يتطلب نصاً\n");
        return value_create_null();
    }
    
    const char *str = args[0].as.string;
    int len = strlen(str);
    
    char *result = malloc(len + 1);
    for (int i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// تنسيق النص (محاكاة printf)
Value lib_text_format(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: صيغ يتطلب نص_قالب\n");
        return value_create_null();
    }
    
    const char *format = args[0].as.string;
    char result[4096];
    int result_pos = 0;
    int arg_index = 1;
    
    for (int i = 0; format[i] && result_pos < sizeof(result) - 1; i++) {
        if (format[i] == '{' && format[i + 1] == '}') {
            if (arg_index < arg_count) {
                char *str = value_to_string(&args[arg_index]);
                int len = strlen(str);
                if (result_pos + len < sizeof(result) - 1) {
                    strcpy(result + result_pos, str);
                    result_pos += len;
                }
                free(str);
                arg_index++;
            }
            i++; // تخطي '}'
        } else {
            result[result_pos++] = format[i];
        }
    }
    
    result[result_pos] = '\0';
    return value_create_string(result);
}
