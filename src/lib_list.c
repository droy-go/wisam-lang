#include "wisam.h"
#include <string.h>
#include <stdlib.h>

// إنشاء قائمة جديدة
Value lib_list_create(Value *args, int arg_count) {
    return value_create_array();
}

// إضافة عنصر
Value lib_list_add(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    Value *item = malloc(sizeof(Value));
    
    if (args[1].type == VAL_NUMBER) {
        *item = value_create_number(args[1].as.number);
    } else if (args[1].type == VAL_STRING) {
        *item = value_create_string(args[1].as.string);
    } else {
        *item = value_create_null();
    }
    
    if (arr->as.array.count >= arr->as.array.capacity) {
        arr->as.array.capacity *= 2;
        arr->as.array.items = realloc(arr->as.array.items, 
                                       sizeof(Value*) * arr->as.array.capacity);
    }
    
    arr->as.array.items[arr->as.array.count++] = item;
    return *arr;
}

// إدراج في موضع
Value lib_list_insert(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_ARRAY || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    int index = (int)args[1].as.number;
    
    if (index < 0 || index > arr->as.array.count) {
        return value_create_null();
    }
    
    if (arr->as.array.count >= arr->as.array.capacity) {
        arr->as.array.capacity *= 2;
        arr->as.array.items = realloc(arr->as.array.items, 
                                       sizeof(Value*) * arr->as.array.capacity);
    }
    
    // Shift elements
    for (int i = arr->as.array.count; i > index; i--) {
        arr->as.array.items[i] = arr->as.array.items[i - 1];
    }
    
    Value *item = malloc(sizeof(Value));
    if (args[2].type == VAL_NUMBER) {
        *item = value_create_number(args[2].as.number);
    } else if (args[2].type == VAL_STRING) {
        *item = value_create_string(args[2].as.string);
    }
    
    arr->as.array.items[index] = item;
    arr->as.array.count++;
    return *arr;
}

// حذف من موضع
Value lib_list_remove(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    int index = (int)args[1].as.number;
    
    if (index < 0 || index >= arr->as.array.count) {
        return value_create_null();
    }
    
    // Free the item
    value_free(arr->as.array.items[index]);
    free(arr->as.array.items[index]);
    
    // Shift elements
    for (int i = index; i < arr->as.array.count - 1; i++) {
        arr->as.array.items[i] = arr->as.array.items[i + 1];
    }
    
    arr->as.array.count--;
    return *arr;
}

// الحصول على عنصر
Value lib_list_get(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    int index = (int)args[1].as.number;
    
    if (index < 0 || index >= arr->as.array.count) {
        return value_create_null();
    }
    
    return *arr->as.array.items[index];
}

// تعيين عنصر
Value lib_list_set(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_ARRAY || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    int index = (int)args[1].as.number;
    
    if (index < 0 || index >= arr->as.array.count) {
        return value_create_null();
    }
    
    value_free(arr->as.array.items[index]);
    
    if (args[2].type == VAL_NUMBER) {
        *arr->as.array.items[index] = value_create_number(args[2].as.number);
    } else if (args[2].type == VAL_STRING) {
        *arr->as.array.items[index] = value_create_string(args[2].as.string);
    }
    
    return *arr;
}

// حجم القائمة
Value lib_list_size(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_number(0);
    }
    return value_create_number(args[0].as.array.count);
}

// فهرس عنصر
Value lib_list_index_of(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY) {
        return value_create_number(-1);
    }
    
    Value *arr = &args[0];
    for (int i = 0; i < arr->as.array.count; i++) {
        if (arr->as.array.items[i]->type == args[1].type) {
            if (args[1].type == VAL_NUMBER && 
                arr->as.array.items[i]->as.number == args[1].as.number) {
                return value_create_number(i);
            }
            if (args[1].type == VAL_STRING && 
                strcmp(arr->as.array.items[i]->as.string, args[1].as.string) == 0) {
                return value_create_number(i);
            }
        }
    }
    return value_create_number(-1);
}

// يحتوي؟
Value lib_list_contains(Value *args, int arg_count) {
    Value idx = lib_list_index_of(args, arg_count);
    return value_create_boolean(idx.as.number >= 0);
}

// عكس القائمة
Value lib_list_reverse(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    int n = arr->as.array.count;
    for (int i = 0; i < n / 2; i++) {
        Value *temp = arr->as.array.items[i];
        arr->as.array.items[i] = arr->as.array.items[n - 1 - i];
        arr->as.array.items[n - 1 - i] = temp;
    }
    return *arr;
}

// نسخ القائمة
Value lib_list_copy(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    Value result = value_create_array();
    
    for (int i = 0; i < arr->as.array.count; i++) {
        Value *item = malloc(sizeof(Value));
        if (arr->as.array.items[i]->type == VAL_NUMBER) {
            *item = value_create_number(arr->as.array.items[i]->as.number);
        } else if (arr->as.array.items[i]->type == VAL_STRING) {
            *item = value_create_string(arr->as.array.items[i]->as.string);
        }
        
        if (result.as.array.count >= result.as.array.capacity) {
            result.as.array.capacity *= 2;
            result.as.array.items = realloc(result.as.array.items, 
                                             sizeof(Value*) * result.as.array.capacity);
        }
        result.as.array.items[result.as.array.count++] = item;
    }
    return result;
}

// دمج قائمتين
Value lib_list_merge(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY || args[1].type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value result = lib_list_copy(args, 1);
    Value *second = &args[1];
    
    for (int i = 0; i < second->as.array.count; i++) {
        Value add_args[2] = {result, *second->as.array.items[i]};
        result = lib_list_add(add_args, 2);
    }
    return result;
}

// تصفية القائمة
Value lib_list_filter(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    // Simplified - would need function pointer in real implementation
    return args[0];
}

// خريطة القائمة
Value lib_list_map(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    // Simplified - would need function pointer in real implementation
    return args[0];
}

// ترتيب القائمة
Value lib_list_sort(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    int n = arr->as.array.count;
    
    // Bubble sort
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            int should_swap = 0;
            if (arr->as.array.items[j]->type == VAL_NUMBER && 
                arr->as.array.items[j + 1]->type == VAL_NUMBER) {
                should_swap = arr->as.array.items[j]->as.number > 
                             arr->as.array.items[j + 1]->as.number;
            } else if (arr->as.array.items[j]->type == VAL_STRING && 
                      arr->as.array.items[j + 1]->type == VAL_STRING) {
                should_swap = strcmp(arr->as.array.items[j]->as.string, 
                                    arr->as.array.items[j + 1]->as.string) > 0;
            }
            
            if (should_swap) {
                Value *temp = arr->as.array.items[j];
                arr->as.array.items[j] = arr->as.array.items[j + 1];
                arr->as.array.items[j + 1] = temp;
            }
        }
    }
    return *arr;
}

// مسح القائمة
Value lib_list_clear(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    
    Value *arr = &args[0];
    for (int i = 0; i < arr->as.array.count; i++) {
        value_free(arr->as.array.items[i]);
        free(arr->as.array.items[i]);
    }
    arr->as.array.count = 0;
    return *arr;
}

// أول عنصر
Value lib_list_first(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY || args[0].as.array.count == 0) {
        return value_create_null();
    }
    return *args[0].as.array.items[0];
}

// آخر عنصر
Value lib_list_last(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY || args[0].as.array.count == 0) {
        return value_create_null();
    }
    return *args[0].as.array.items[args[0].as.array.count - 1];
}

// أخذ n عنصر من البداية
Value lib_list_take(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    int n = (int)args[1].as.number;
    if (n < 0) n = 0;
    if (n > args[0].as.array.count) n = args[0].as.array.count;
    
    Value result = value_create_array();
    for (int i = 0; i < n; i++) {
        Value *item = malloc(sizeof(Value));
        if (args[0].as.array.items[i]->type == VAL_NUMBER) {
            *item = value_create_number(args[0].as.array.items[i]->as.number);
        } else if (args[0].as.array.items[i]->type == VAL_STRING) {
            *item = value_create_string(args[0].as.array.items[i]->as.string);
        }
        result.as.array.items[result.as.array.count++] = item;
    }
    return result;
}

// تخطي n عنصر
Value lib_list_skip(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_ARRAY || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    int n = (int)args[1].as.number;
    if (n < 0) n = 0;
    if (n > args[0].as.array.count) n = args[0].as.array.count;
    
    Value result = value_create_array();
    for (int i = n; i < args[0].as.array.count; i++) {
        Value *item = malloc(sizeof(Value));
        if (args[0].as.array.items[i]->type == VAL_NUMBER) {
            *item = value_create_number(args[0].as.array.items[i]->as.number);
        } else if (args[0].as.array.items[i]->type == VAL_STRING) {
            *item = value_create_string(args[0].as.array.items[i]->as.string);
        }
        result.as.array.items[result.as.array.count++] = item;
    }
    return result;
}
