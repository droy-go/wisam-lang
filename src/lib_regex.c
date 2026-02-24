#include "wisam.h"
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// مطابقة نمط
Value lib_regex_match(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    char *pattern = args[0].as.string;
    char *text = args[1].as.string;
    
    regex_t regex;
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) {
        return value_create_boolean(false);
    }
    
    ret = regexec(&regex, text, 0, NULL, 0);
    regfree(&regex);
    
    return value_create_boolean(ret == 0);
}

// البحث عن أول تطابق
Value lib_regex_search(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *pattern = args[0].as.string;
    char *text = args[1].as.string;
    
    regex_t regex;
    regmatch_t match;
    
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) return value_create_null();
    
    ret = regexec(&regex, text, 1, &match, 0);
    if (ret != 0) {
        regfree(&regex);
        return value_create_null();
    }
    
    int len = match.rm_eo - match.rm_so;
    char *result = malloc(len + 1);
    strncpy(result, text + match.rm_so, len);
    result[len] = '\0';
    
    regfree(&regex);
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// استبدال باستخدام regex
Value lib_regex_replace(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_STRING || 
        args[1].type != VAL_STRING || args[2].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *pattern = args[0].as.string;
    char *replacement = args[1].as.string;
    char *text = args[2].as.string;
    
    // Simple implementation - replace first occurrence
    regex_t regex;
    regmatch_t match;
    
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) return value_create_string(text);
    
    ret = regexec(&regex, text, 1, &match, 0);
    if (ret != 0) {
        regfree(&regex);
        return value_create_string(text);
    }
    
    int text_len = strlen(text);
    int repl_len = strlen(replacement);
    int match_len = match.rm_eo - match.rm_so;
    
    char *result = malloc(text_len - match_len + repl_len + 1);
    
    strncpy(result, text, match.rm_so);
    strcpy(result + match.rm_so, replacement);
    strcpy(result + match.rm_so + repl_len, text + match.rm_eo);
    
    regfree(&regex);
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// تقسيم باستخدام regex
Value lib_regex_split(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *pattern = args[0].as.string;
    char *text = args[1].as.string;
    
    Value result = value_create_array();
    regex_t regex;
    regmatch_t match;
    
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) return result;
    
    char *ptr = text;
    while (regexec(&regex, ptr, 1, &match, 0) == 0) {
        if (match.rm_so > 0) {
            Value *item = malloc(sizeof(Value));
            char *part = malloc(match.rm_so + 1);
            strncpy(part, ptr, match.rm_so);
            part[match.rm_so] = '\0';
            *item = value_create_string(part);
            free(part);
            
            if (result.as.array.count >= result.as.array.capacity) {
                result.as.array.capacity *= 2;
                result.as.array.items = realloc(result.as.array.items, 
                                                 sizeof(Value*) * result.as.array.capacity);
            }
            result.as.array.items[result.as.array.count++] = item;
        }
        ptr += match.rm_eo;
    }
    
    // Add remaining text
    if (*ptr) {
        Value *item = malloc(sizeof(Value));
        *item = value_create_string(ptr);
        if (result.as.array.count >= result.as.array.capacity) {
            result.as.array.capacity *= 2;
            result.as.array.items = realloc(result.as.array.items, 
                                             sizeof(Value*) * result.as.array.capacity);
        }
        result.as.array.items[result.as.array.count++] = item;
    }
    
    regfree(&regex);
    return result;
}

// العثور على كل التطابقات
Value lib_regex_find_all(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *pattern = args[0].as.string;
    char *text = args[1].as.string;
    
    Value result = value_create_array();
    regex_t regex;
    regmatch_t match;
    
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) return result;
    
    char *ptr = text;
    while (regexec(&regex, ptr, 1, &match, 0) == 0) {
        int len = match.rm_eo - match.rm_so;
        char *part = malloc(len + 1);
        strncpy(part, ptr + match.rm_so, len);
        part[len] = '\0';
        
        Value *item = malloc(sizeof(Value));
        *item = value_create_string(part);
        free(part);
        
        if (result.as.array.count >= result.as.array.capacity) {
            result.as.array.capacity *= 2;
            result.as.array.items = realloc(result.as.array.items, 
                                             sizeof(Value*) * result.as.array.capacity);
        }
        result.as.array.items[result.as.array.count++] = item;
        
        ptr += match.rm_eo;
    }
    
    regfree(&regex);
    return result;
}

// استخراج مجموعات
Value lib_regex_groups(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *pattern = args[0].as.string;
    char *text = args[1].as.string;
    
    Value result = value_create_array();
    regex_t regex;
    regmatch_t matches[10];
    
    int ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret) return result;
    
    ret = regexec(&regex, text, 10, matches, 0);
    if (ret == 0) {
        for (int i = 0; i < 10 && matches[i].rm_so != -1; i++) {
            int len = matches[i].rm_eo - matches[i].rm_so;
            char *part = malloc(len + 1);
            strncpy(part, text + matches[i].rm_so, len);
            part[len] = '\0';
            
            Value *item = malloc(sizeof(Value));
            *item = value_create_string(part);
            free(part);
            
            if (result.as.array.count >= result.as.array.capacity) {
                result.as.array.capacity *= 2;
                result.as.array.items = realloc(result.as.array.items, 
                                                 sizeof(Value*) * result.as.array.capacity);
            }
            result.as.array.items[result.as.array.count++] = item;
        }
    }
    
    regfree(&regex);
    return result;
}

// أنماط مفيدة
Value lib_regex_email_pattern(Value *args, int arg_count) {
    return value_create_string("^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$");
}

Value lib_regex_url_pattern(Value *args, int arg_count) {
    return value_create_string("^https?://[^\\s/$.?#].[^\\s]*$");
}

Value lib_regex_phone_pattern(Value *args, int arg_count) {
    return value_create_string("^\\+?[0-9]{10,15}$");
}

Value lib_regex_ip_pattern(Value *args, int arg_count) {
    return value_create_string("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
}
