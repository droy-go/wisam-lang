#include "wisam.h"
#include "interpreter.h"
#include <ctype.h>

// نصوص (Text) Module - Standard library for string operations

// Convert string to uppercase
static Value *text_to_upper(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = strdup(args[0]->data.string);
    for (int i = 0; str[i]; i++) {
        // Handle Arabic text - keep as is
        unsigned char c = (unsigned char)str[i];
        if (c < 0x80) {
            str[i] = toupper(c);
        }
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = str;
    return result;
}

// Convert string to lowercase
static Value *text_to_lower(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = strdup(args[0]->data.string);
    for (int i = 0; str[i]; i++) {
        unsigned char c = (unsigned char)str[i];
        if (c < 0x80) {
            str[i] = tolower(c);
        }
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = str;
    return result;
}

// Get string length
static Value *text_length(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = 0;
        return result;
    }
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = strlen(args[0]->data.string);
    return result;
}

// Substring
static Value *text_substring(Value **args, int arg_count) {
    if (arg_count < 3 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = args[0]->data.string;
    int start = (int)value_to_number(args[1]);
    int length = (int)value_to_number(args[2]);
    
    int str_len = strlen(str);
    if (start < 0) start = 0;
    if (start >= str_len) {
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup("");
        return result;
    }
    
    if (length < 0) length = 0;
    if (start + length > str_len) length = str_len - start;
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = malloc(length + 1);
    strncpy(result->data.string, str + start, length);
    result->data.string[length] = '\0';
    
    return result;
}

// Replace substring
static Value *text_replace(Value **args, int arg_count) {
    if (arg_count < 3 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING || args[2]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = args[0]->data.string;
    char *old = args[1]->data.string;
    char *new_str = args[2]->data.string;
    
    // Count occurrences
    int count = 0;
    char *tmp = str;
    while ((tmp = strstr(tmp, old)) != NULL) {
        count++;
        tmp += strlen(old);
    }
    
    // Allocate result
    size_t result_len = strlen(str) + count * (strlen(new_str) - strlen(old)) + 1;
    char *result_str = malloc(result_len);
    
    // Replace
    char *dst = result_str;
    char *src = str;
    char *pos;
    while ((pos = strstr(src, old)) != NULL) {
        int len = pos - src;
        memcpy(dst, src, len);
        dst += len;
        memcpy(dst, new_str, strlen(new_str));
        dst += strlen(new_str);
        src = pos + strlen(old);
    }
    strcpy(dst, src);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = result_str;
    return result;
}

// Split string
static Value *text_split(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = strdup(args[0]->data.string);
    char *delimiter = args[1]->data.string;
    
    Value *result = create_value(TYPE_ARRAY);
    result->data.array = malloc(sizeof(Array));
    result->data.array->items = NULL;
    result->data.array->count = 0;
    result->data.array->capacity = 0;
    
    char *token = strtok(str, delimiter);
    while (token) {
        if (result->data.array->count >= result->data.array->capacity) {
            result->data.array->capacity = result->data.array->capacity * 2 + 4;
            result->data.array->items = realloc(result->data.array->items,
                                                sizeof(Value *) * result->data.array->capacity);
        }
        
        Value *item = create_value(TYPE_STRING);
        item->data.string = strdup(token);
        result->data.array->items[result->data.array->count++] = item;
        
        token = strtok(NULL, delimiter);
    }
    
    free(str);
    return result;
}

// Trim whitespace
static Value *text_trim(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = args[0]->data.string;
    
    // Trim leading
    while (isspace((unsigned char)*str)) str++;
    
    // Trim trailing
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(str);
    return result;
}

// Check if string contains substring
static Value *text_contains(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = (strstr(args[0]->data.string, args[1]->data.string) != NULL);
    return result;
}

// Find substring position
static Value *text_find(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    char *pos = strstr(args[0]->data.string, args[1]->data.string);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = pos ? (pos - args[0]->data.string) : -1;
    return result;
}

// Reverse string
static Value *text_reverse(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = strdup(args[0]->data.string);
    int len = strlen(str);
    
    for (int i = 0; i < len / 2; i++) {
        char tmp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = tmp;
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = str;
    return result;
}

// Translate (simplified - just returns the same string for now)
static Value *text_translate(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *text = args[0]->data.string;
    char *to_lang = args[1]->data.string;
    
    // Simple translation dictionary
    if (strcmp(to_lang, "English") == 0 || strcmp(to_lang, "الإنجليزية") == 0) {
        if (strcmp(text, "مرحبا") == 0) {
            Value *result = create_value(TYPE_STRING);
            result->data.string = strdup("Hello");
            return result;
        } else if (strcmp(text, "شكرا") == 0) {
            Value *result = create_value(TYPE_STRING);
            result->data.string = strdup("Thank you");
            return result;
        } else if (strcmp(text, "وداعا") == 0) {
            Value *result = create_value(TYPE_STRING);
            result->data.string = strdup("Goodbye");
            return result;
        }
    }
    
    // Return original if no translation found
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(text);
    return result;
}

// Join array into string
static Value *text_join(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_ARRAY || args[1]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    Array *arr = args[0]->data.array;
    char *separator = args[1]->data.string;
    
    // Calculate total length
    size_t total_len = 0;
    for (int i = 0; i < arr->count; i++) {
        char *str = value_to_string(arr->items[i]);
        total_len += strlen(str);
        free(str);
        if (i < arr->count - 1) {
            total_len += strlen(separator);
        }
    }
    
    char *result_str = malloc(total_len + 1);
    result_str[0] = '\0';
    
    for (int i = 0; i < arr->count; i++) {
        char *str = value_to_string(arr->items[i]);
        strcat(result_str, str);
        free(str);
        if (i < arr->count - 1) {
            strcat(result_str, separator);
        }
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = result_str;
    return result;
}

// Check if string starts with prefix
static Value *text_starts_with(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    char *str = args[0]->data.string;
    char *prefix = args[1]->data.string;
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = (strncmp(str, prefix, strlen(prefix)) == 0);
    return result;
}

// Check if string ends with suffix
static Value *text_ends_with(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    char *str = args[0]->data.string;
    char *suffix = args[1]->data.string;
    
    int str_len = strlen(str);
    int suffix_len = strlen(suffix);
    
    Value *result = create_value(TYPE_BOOL);
    if (suffix_len > str_len) {
        result->data.boolean = false;
    } else {
        result->data.boolean = (strcmp(str + str_len - suffix_len, suffix) == 0);
    }
    return result;
}

// Repeat string
static Value *text_repeat(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = args[0]->data.string;
    int times = (int)value_to_number(args[1]);
    
    if (times < 0) times = 0;
    if (times > 1000) times = 1000; // Limit
    
    size_t len = strlen(str);
    char *result_str = malloc(len * times + 1);
    result_str[0] = '\0';
    
    for (int i = 0; i < times; i++) {
        strcat(result_str, str);
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = result_str;
    return result;
}

// Pad string
static Value *text_pad(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    char *str = args[0]->data.string;
    int total_length = (int)value_to_number(args[1]);
    char pad_char = (arg_count >= 3 && args[2]->type == TYPE_STRING) ? 
                    args[2]->data.string[0] : ' ';
    
    int str_len = strlen(str);
    if (total_length <= str_len) {
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup(str);
        return result;
    }
    
    int pad_left = (total_length - str_len) / 2;
    int pad_right = total_length - str_len - pad_left;
    
    char *result_str = malloc(total_length + 1);
    memset(result_str, pad_char, pad_left);
    strcpy(result_str + pad_left, str);
    memset(result_str + pad_left + str_len, pad_char, pad_right);
    result_str[total_length] = '\0';
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = result_str;
    return result;
}

// Module method dispatcher
Value *text_module_call(const char *method, Value **args, int arg_count) {
    if (strcmp(method, "حوّل_إلى_كبير") == 0 || strcmp(method, "to_upper") == 0) {
        return text_to_upper(args, arg_count);
    } else if (strcmp(method, "حوّل_إلى_صغير") == 0 || strcmp(method, "to_lower") == 0) {
        return text_to_lower(args, arg_count);
    } else if (strcmp(method, "الطول") == 0 || strcmp(method, "length") == 0) {
        return text_length(args, arg_count);
    } else if (strcmp(method, "جزء") == 0 || strcmp(method, "substring") == 0) {
        return text_substring(args, arg_count);
    } else if (strcmp(method, "استبدل") == 0 || strcmp(method, "replace") == 0) {
        return text_replace(args, arg_count);
    } else if (strcmp(method, "قسّم") == 0 || strcmp(method, "split") == 0) {
        return text_split(args, arg_count);
    } else if (strcmp(method, "تقليم") == 0 || strcmp(method, "trim") == 0) {
        return text_trim(args, arg_count);
    } else if (strcmp(method, "يحتوي") == 0 || strcmp(method, "contains") == 0) {
        return text_contains(args, arg_count);
    } else if (strcmp(method, "ابحث") == 0 || strcmp(method, "find") == 0) {
        return text_find(args, arg_count);
    } else if (strcmp(method, "اعكس") == 0 || strcmp(method, "reverse") == 0) {
        return text_reverse(args, arg_count);
    } else if (strcmp(method, "ترجم") == 0 || strcmp(method, "translate") == 0) {
        return text_translate(args, arg_count);
    } else if (strcmp(method, "ادمج") == 0 || strcmp(method, "join") == 0) {
        return text_join(args, arg_count);
    } else if (strcmp(method, "يبدأ_بـ") == 0 || strcmp(method, "starts_with") == 0) {
        return text_starts_with(args, arg_count);
    } else if (strcmp(method, "ينتهي_بـ") == 0 || strcmp(method, "ends_with") == 0) {
        return text_ends_with(args, arg_count);
    } else if (strcmp(method, "كرّر") == 0 || strcmp(method, "repeat") == 0) {
        return text_repeat(args, arg_count);
    } else if (strcmp(method, "حشو") == 0 || strcmp(method, "pad") == 0) {
        return text_pad(args, arg_count);
    }
    
    return create_value(TYPE_NULL);
}

// Initialize text module
void text_module_init(void) {
    // Register the module
    if (module_count < MAX_MODULES) {
        modules[module_count].name = strdup("نصوص");
        modules[module_count].init = text_module_init;
        modules[module_count].call = text_module_call;
        module_count++;
    }
}
