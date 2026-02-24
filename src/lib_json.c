#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// تحويل كائن إلى JSON
Value lib_json_stringify(Value *args, int arg_count) {
    if (arg_count < 1) {
        return value_create_null();
    }
    
    static char buffer[65536];
    buffer[0] = '\0';
    
    if (args[0].type == VAL_OBJECT) {
        strcat(buffer, "{");
        for (int i = 0; i < args[0].as.object.count; i++) {
            if (i > 0) strcat(buffer, ",");
            strcat(buffer, "\"");
            strcat(buffer, args[0].as.object.keys[i]);
            strcat(buffer, "\":");
            
            Value *val = args[0].as.object.values[i];
            if (val->type == VAL_STRING) {
                strcat(buffer, "\"");
                strcat(buffer, val->as.string);
                strcat(buffer, "\"");
            } else if (val->type == VAL_NUMBER) {
                char num[32];
                snprintf(num, sizeof(num), "%g", val->as.number);
                strcat(buffer, num);
            } else if (val->type == VAL_BOOLEAN) {
                strcat(buffer, val->as.boolean ? "true" : "false");
            } else if (val->type == VAL_NULL) {
                strcat(buffer, "null");
            }
        }
        strcat(buffer, "}");
    } else if (args[0].type == VAL_ARRAY) {
        strcat(buffer, "[");
        for (int i = 0; i < args[0].as.array.count; i++) {
            if (i > 0) strcat(buffer, ",");
            
            Value *val = args[0].as.array.items[i];
            if (val->type == VAL_STRING) {
                strcat(buffer, "\"");
                strcat(buffer, val->as.string);
                strcat(buffer, "\"");
            } else if (val->type == VAL_NUMBER) {
                char num[32];
                snprintf(num, sizeof(num), "%g", val->as.number);
                strcat(buffer, num);
            } else if (val->type == VAL_BOOLEAN) {
                strcat(buffer, val->as.boolean ? "true" : "false");
            } else if (val->type == VAL_NULL) {
                strcat(buffer, "null");
            }
        }
        strcat(buffer, "]");
    } else if (args[0].type == VAL_STRING) {
        strcat(buffer, "\"");
        strcat(buffer, args[0].as.string);
        strcat(buffer, "\"");
    } else if (args[0].type == VAL_NUMBER) {
        char num[32];
        snprintf(num, sizeof(num), "%g", args[0].as.number);
        strcat(buffer, num);
    } else if (args[0].type == VAL_BOOLEAN) {
        strcat(buffer, args[0].as.boolean ? "true" : "false");
    } else if (args[0].type == VAL_NULL) {
        strcat(buffer, "null");
    }
    
    return value_create_string(buffer);
}

// تحليل JSON بسيط
Value lib_json_parse(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *json = args[0].as.string;
    
    // تخطي المسافات
    while (isspace(*json)) json++;
    
    // كائن
    if (*json == '{') {
        Value result = value_create_object();
        json++;
        
        while (*json && *json != '}') {
            // تخطي المسافات
            while (isspace(*json)) json++;
            
            // قراءة المفتاح
            if (*json == '"') {
                json++;
                char key[256];
                int i = 0;
                while (*json && *json != '"' && i < 255) {
                    key[i++] = *json++;
                }
                key[i] = '\0';
                if (*json == '"') json++;
                
                // تخطي للقيمة
                while (isspace(*json) || *json == ':') json++;
                
                // قراءة القيمة
                Value val;
                if (*json == '"') {
                    json++;
                    char str[256];
                    i = 0;
                    while (*json && *json != '"' && i < 255) {
                        str[i++] = *json++;
                    }
                    str[i] = '\0';
                    if (*json == '"') json++;
                    val = value_create_string(str);
                } else if (isdigit(*json) || *json == '-') {
                    double num = strtod(json, &json);
                    val = value_create_number(num);
                } else if (strncmp(json, "true", 4) == 0) {
                    val = value_create_boolean(true);
                    json += 4;
                } else if (strncmp(json, "false", 5) == 0) {
                    val = value_create_boolean(false);
                    json += 5;
                } else if (strncmp(json, "null", 4) == 0) {
                    val = value_create_null();
                    json += 4;
                }
                
                // إضافة للكائن
                if (result.as.object.count >= result.as.object.capacity) {
                    result.as.object.capacity *= 2;
                    result.as.object.keys = realloc(result.as.object.keys, 
                                                    sizeof(char*) * result.as.object.capacity);
                    result.as.object.values = realloc(result.as.object.values, 
                                                      sizeof(Value*) * result.as.object.capacity);
                }
                result.as.object.keys[result.as.object.count] = strdup(key);
                result.as.object.values[result.as.object.count] = malloc(sizeof(Value));
                *result.as.object.values[result.as.object.count] = val;
                result.as.object.count++;
            }
            
            // تخطي الفاصلة
            while (isspace(*json)) json++;
            if (*json == ',') json++;
        }
        
        return result;
    }
    
    // مصفوفة
    if (*json == '[') {
        Value result = value_create_array();
        json++;
        
        while (*json && *json != ']') {
            while (isspace(*json)) json++;
            
            Value *item = malloc(sizeof(Value));
            
            if (*json == '"') {
                json++;
                char str[256];
                int i = 0;
                while (*json && *json != '"' && i < 255) {
                    str[i++] = *json++;
                }
                str[i] = '\0';
                if (*json == '"') json++;
                *item = value_create_string(str);
            } else if (isdigit(*json) || *json == '-') {
                double num = strtod(json, &json);
                *item = value_create_number(num);
            } else if (strncmp(json, "true", 4) == 0) {
                *item = value_create_boolean(true);
                json += 4;
            } else if (strncmp(json, "false", 5) == 0) {
                *item = value_create_boolean(false);
                json += 5;
            } else if (strncmp(json, "null", 4) == 0) {
                *item = value_create_null();
                json += 4;
            }
            
            if (result.as.array.count >= result.as.array.capacity) {
                result.as.array.capacity *= 2;
                result.as.array.items = realloc(result.as.array.items, 
                                                 sizeof(Value*) * result.as.array.capacity);
            }
            result.as.array.items[result.as.array.count++] = item;
            
            while (isspace(*json)) json++;
            if (*json == ',') json++;
        }
        
        return result;
    }
    
    return value_create_null();
}

// قراءة JSON من ملف
Value lib_json_read_file(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    FILE *f = fopen(args[0].as.string, "r");
    if (!f) return value_create_null();
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    fread(content, 1, size, f);
    content[size] = '\0';
    fclose(f);
    
    Value json_str = value_create_string(content);
    free(content);
    
    Value parse_args[1] = {json_str};
    return lib_json_parse(parse_args, 1);
}

// كتابة JSON لملف
Value lib_json_write_file(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    Value str = lib_json_stringify(&args[1], 1);
    if (str.type != VAL_STRING) return value_create_boolean(false);
    
    FILE *f = fopen(args[0].as.string, "w");
    if (!f) return value_create_boolean(false);
    
    fprintf(f, "%s", str.as.string);
    fclose(f);
    
    return value_create_boolean(true);
}

// الحصول على قيمة من JSON
Value lib_json_get(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_OBJECT || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    for (int i = 0; i < args[0].as.object.count; i++) {
        if (strcmp(args[0].as.object.keys[i], args[1].as.string) == 0) {
            return *args[0].as.object.values[i];
        }
    }
    return value_create_null();
}

// تعيين قيمة في JSON
Value lib_json_set(Value *args, int arg_count) {
    if (arg_count < 3 || args[0].type != VAL_OBJECT || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    // البحث عن المفتاح
    for (int i = 0; i < args[0].as.object.count; i++) {
        if (strcmp(args[0].as.object.keys[i], args[1].as.string) == 0) {
            value_free(args[0].as.object.values[i]);
            *args[0].as.object.values[i] = args[2];
            return args[0];
        }
    }
    
    // إضافة مفتاح جديد
    if (args[0].as.object.count >= args[0].as.object.capacity) {
        args[0].as.object.capacity *= 2;
        args[0].as.object.keys = realloc(args[0].as.object.keys, 
                                          sizeof(char*) * args[0].as.object.capacity);
        args[0].as.object.values = realloc(args[0].as.object.values, 
                                            sizeof(Value*) * args[0].as.object.capacity);
    }
    
    args[0].as.object.keys[args[0].as.object.count] = strdup(args[1].as.string);
    args[0].as.object.values[args[0].as.object.count] = malloc(sizeof(Value));
    *args[0].as.object.values[args[0].as.object.count] = args[2];
    args[0].as.object.count++;
    
    return args[0];
}
