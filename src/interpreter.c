#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// إنشاء قيمة رقمية
Value value_create_number(double num) {
    Value v;
    v.type = VAL_NUMBER;
    v.as.number = num;
    return v;
}

// إنشاء قيمة نصية
Value value_create_string(const char *str) {
    Value v;
    v.type = VAL_STRING;
    v.as.string = strdup(str);
    return v;
}

// إنشاء قيمة منطقية
Value value_create_boolean(bool boolean) {
    Value v;
    v.type = VAL_BOOLEAN;
    v.as.boolean = boolean;
    return v;
}

// إنشاء قيمة فارغة
Value value_create_null(void) {
    Value v;
    v.type = VAL_NULL;
    return v;
}

// إنشاء مصفوفة
Value value_create_array(void) {
    Value v;
    v.type = VAL_ARRAY;
    v.as.array.items = malloc(sizeof(Value*) * 10);
    v.as.array.count = 0;
    v.as.array.capacity = 10;
    return v;
}

// إنشاء كائن
Value value_create_object(void) {
    Value v;
    v.type = VAL_OBJECT;
    v.as.object.keys = malloc(sizeof(char*) * 10);
    v.as.object.values = malloc(sizeof(Value*) * 10);
    v.as.object.count = 0;
    v.as.object.capacity = 10;
    return v;
}

// إنشاء استثناء
Value value_create_exception(const char *message, int code) {
    Value v;
    v.type = VAL_EXCEPTION;
    v.as.exception.message = strdup(message);
    v.as.exception.code = code;
    v.as.exception.stack_trace = NULL;
    return v;
}

// تحرير قيمة
void value_free(Value *value) {
    if (!value) return;
    
    switch (value->type) {
        case VAL_STRING:
            free(value->as.string);
            break;
        case VAL_ARRAY:
            for (int i = 0; i < value->as.array.count; i++) {
                value_free(value->as.array.items[i]);
                free(value->as.array.items[i]);
            }
            free(value->as.array.items);
            break;
        case VAL_OBJECT:
            for (int i = 0; i < value->as.object.count; i++) {
                free(value->as.object.keys[i]);
                value_free(value->as.object.values[i]);
                free(value->as.object.values[i]);
            }
            free(value->as.object.keys);
            free(value->as.object.values);
            break;
        case VAL_EXCEPTION:
            free(value->as.exception.message);
            free(value->as.exception.stack_trace);
            break;
        default:
            break;
    }
}

// تحويل قيمة إلى نص
char *value_to_string(Value *value) {
    if (!value) return strdup("فارغ");
    
    char buffer[1024];
    
    switch (value->type) {
        case VAL_NUMBER:
            snprintf(buffer, sizeof(buffer), "%g", value->as.number);
            return strdup(buffer);
        case VAL_STRING:
            return strdup(value->as.string);
        case VAL_BOOLEAN:
            return strdup(value->as.boolean ? "صحيح" : "خطأ");
        case VAL_NULL:
            return strdup("فارغ");
        case VAL_ARRAY:
            {
                char *result = strdup("[");
                for (int i = 0; i < value->as.array.count; i++) {
                    char *item = value_to_string(value->as.array.items[i]);
                    result = realloc(result, strlen(result) + strlen(item) + 4);
                    strcat(result, item);
                    if (i < value->as.array.count - 1) strcat(result, ", ");
                    free(item);
                }
                result = realloc(result, strlen(result) + 2);
                strcat(result, "]");
                return result;
            }
        case VAL_OBJECT:
            {
                char *result = strdup("{");
                for (int i = 0; i < value->as.object.count; i++) {
                    result = realloc(result, strlen(result) + strlen(value->as.object.keys[i]) + 10);
                    strcat(result, value->as.object.keys[i]);
                    strcat(result, ": ");
                    char *val = value_to_string(value->as.object.values[i]);
                    result = realloc(result, strlen(result) + strlen(val) + 4);
                    strcat(result, val);
                    free(val);
                    if (i < value->as.object.count - 1) strcat(result, ", ");
                }
                result = realloc(result, strlen(result) + 2);
                strcat(result, "}");
                return result;
            }
        case VAL_EXCEPTION:
            snprintf(buffer, sizeof(buffer), "استثناء: %s (الكود: %d)", 
                     value->as.exception.message, value->as.exception.code);
            return strdup(buffer);
        default:
            return strdup("<كائن>");
    }
}

// التحقق من صحة القيمة
bool value_is_truthy(Value *value) {
    if (!value) return false;
    
    switch (value->type) {
        case VAL_NULL:
            return false;
        case VAL_BOOLEAN:
            return value->as.boolean;
        case VAL_NUMBER:
            return value->as.number != 0;
        case VAL_STRING:
            return strlen(value->as.string) > 0;
        case VAL_ARRAY:
            return value->as.array.count > 0;
        case VAL_OBJECT:
            return value->as.object.count > 0;
        default:
            return true;
    }
}

// مقارنة قيمتين
bool value_equals(Value *a, Value *b) {
    if (!a || !b) return a == b;
    if (a->type != b->type) return false;
    
    switch (a->type) {
        case VAL_NULL:
            return true;
        case VAL_NUMBER:
            return a->as.number == b->as.number;
        case VAL_STRING:
            return strcmp(a->as.string, b->as.string) == 0;
        case VAL_BOOLEAN:
            return a->as.boolean == b->as.boolean;
        default:
            return false;
    }
}

// نسخ قيمة
Value value_copy(Value *value) {
    if (!value) return value_create_null();
    
    switch (value->type) {
        case VAL_NUMBER:
            return value_create_number(value->as.number);
        case VAL_STRING:
            return value_create_string(value->as.string);
        case VAL_BOOLEAN:
            return value_create_boolean(value->as.boolean);
        case VAL_NULL:
            return value_create_null();
        case VAL_ARRAY:
            {
                Value copy = value_create_array();
                for (int i = 0; i < value->as.array.count; i++) {
                    Value item = value_copy(value->as.array.items[i]);
                    copy.as.array.items[copy.as.array.count] = malloc(sizeof(Value));
                    *copy.as.array.items[copy.as.array.count] = item;
                    copy.as.array.count++;
                }
                return copy;
            }
        case VAL_OBJECT:
            {
                Value copy = value_create_object();
                for (int i = 0; i < value->as.object.count; i++) {
                    copy.as.object.keys[copy.as.object.count] = strdup(value->as.object.keys[i]);
                    copy.as.object.values[copy.as.object.count] = malloc(sizeof(Value));
                    *copy.as.object.values[copy.as.object.count] = value_copy(value->as.object.values[i]);
                    copy.as.object.count++;
                }
                return copy;
            }
        default:
            return value_create_null();
    }
}

// إنشاء بيئة
Environment *environment_create(Environment *parent, const char *name) {
    Environment *env = malloc(sizeof(Environment));
    env->var_count = 0;
    env->parent = parent;
    env->name = name ? strdup(name) : NULL;
    env->is_class_scope = false;
    env->is_function_scope = false;
    return env;
}

// تدمير بيئة
void environment_destroy(Environment *env) {
    if (!env) return;
    
    for (int i = 0; i < env->var_count; i++) {
        free(env->variables[i].name);
        value_free(&env->variables[i].value);
        free(env->variables[i].type_hint);
    }
    
    free(env->name);
    free(env);
}

// تعريف متغير
void environment_define(Environment *env, const char *name, Value value, bool is_constant) {
    if (!env || !name) return;
    
    // التحقق من عدم وجود المتغير مسبقاً
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            // تحديث القيمة إذا لم يكن ثابتاً
            if (!env->variables[i].is_constant) {
                value_free(&env->variables[i].value);
                env->variables[i].value = value;
            }
            return;
        }
    }
    
    // إضافة متغير جديد
    if (env->var_count < MAX_VARIABLES) {
        env->variables[env->var_count].name = strdup(name);
        env->variables[env->var_count].value = value;
        env->variables[env->var_count].is_constant = is_constant;
        env->variables[env->var_count].type_hint = NULL;
        env->var_count++;
    }
}

// الحصول على قيمة متغير
Value *environment_get(Environment *env, const char *name) {
    if (!env || !name) return NULL;
    
    // البحث في البيئة الحالية
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return &env->variables[i].value;
        }
    }
    
    // البحث في البيئة الأب
    if (env->parent) {
        return environment_get(env->parent, name);
    }
    
    return NULL;
}

// تعيين قيمة متغير
void environment_set(Environment *env, const char *name, Value value) {
    if (!env || !name) return;
    
    // البحث في البيئة الحالية
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            if (!env->variables[i].is_constant) {
                value_free(&env->variables[i].value);
                env->variables[i].value = value;
            }
            return;
        }
    }
    
    // البحث في البيئة الأب
    if (env->parent) {
        environment_set(env->parent, name, value);
    }
}

// التحقق من وجود متغير
bool environment_exists(Environment *env, const char *name) {
    return environment_get(env, name) != NULL;
}

// التحقق من كون المتغير ثابتاً
bool environment_is_constant(Environment *env, const char *name) {
    if (!env || !name) return false;
    
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return env->variables[i].is_constant;
        }
    }
    
    if (env->parent) {
        return environment_is_constant(env->parent, name);
    }
    
    return false;
}

// تعيين تلميح النوع
void environment_set_type_hint(Environment *env, const char *name, const char *type_hint) {
    if (!env || !name) return;
    
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            free(env->variables[i].type_hint);
            env->variables[i].type_hint = type_hint ? strdup(type_hint) : NULL;
            return;
        }
    }
}

// الحصول على تلميح النوع
char *environment_get_type_hint(Environment *env, const char *name) {
    if (!env || !name) return NULL;
    
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return env->variables[i].type_hint;
        }
    }
    
    if (env->parent) {
        return environment_get_type_hint(env->parent, name);
    }
    
    return NULL;
}

// إنشاء المفسر
Interpreter *interpreter_create(void) {
    Interpreter *interp = malloc(sizeof(Interpreter));
    interp->global_env = environment_create(NULL, "عالمي");
    interp->current_env = interp->global_env;
    interp->return_value = NULL;
    interp->is_returning = false;
    interp->is_breaking = false;
    interp->is_continuing = false;
    interp->exception = NULL;
    interp->is_try_block = false;
    
    // تعريف الثوابت الأساسية
    environment_define(interp->global_env, "صحيح", value_create_boolean(true), true);
    environment_define(interp->global_env, "خطأ", value_create_boolean(false), true);
    environment_define(interp->global_env, "فارغ", value_create_null(), true);
    environment_define(interp->global_env, "PI", value_create_number(3.14159265359), true);
    environment_define(interp->global_env, "E", value_create_number(2.71828182846), true);
    
    return interp;
}

// تدمير المفسر
void interpreter_destroy(Interpreter *interp) {
    if (!interp) return;
    
    environment_destroy(interp->global_env);
    if (interp->return_value) {
        value_free(interp->return_value);
        free(interp->return_value);
    }
    if (interp->exception) {
        value_free(interp->exception);
        free(interp->exception);
    }
    free(interp);
}

// تعيين متغير
void interpreter_set_variable(Interpreter *interp, const char *name, Value value) {
    if (!interp || !name) return;
    environment_define(interp->global_env, name, value, false);
}

// الحصول على متغير
Value *interpreter_get_variable(Interpreter *interp, const char *name) {
    if (!interp || !name) return NULL;
    return environment_get(interp->current_env, name);
}

// تقييم العقدة
Value interpreter_evaluate(Interpreter *interp, ASTNode *node) {
    if (!node) return value_create_null();
    
    switch (node->type) {
        case AST_LITERAL:
            return value_copy(&node->as.literal.value);
            
        case AST_IDENTIFIER:
            {
                Value *val = environment_get(interp->current_env, node->as.identifier.name);
                if (val) {
                    return value_copy(val);
                } else {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), 
                             "المتغير '%s' غير معرف", node->as.identifier.name);
                    return value_create_exception(error_msg, 1);
                }
            }
            
        case AST_BINARY_OP:
            {
                Value left = interpreter_evaluate(interp, node->as.binary_op.left);
                Value right = interpreter_evaluate(interp, node->as.binary_op.right);
                Value result = value_create_null();
                
                if (left.type == VAL_EXCEPTION) {
                    value_free(&right);
                    return left;
                }
                if (right.type == VAL_EXCEPTION) {
                    value_free(&left);
                    return right;
                }
                
                switch (node->as.binary_op.op) {
                    case TOKEN_PLUS:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_number(left.as.number + right.as.number);
                        } else if (left.type == VAL_STRING || right.type == VAL_STRING) {
                            char *ls = value_to_string(&left);
                            char *rs = value_to_string(&right);
                            char *combined = malloc(strlen(ls) + strlen(rs) + 1);
                            strcpy(combined, ls);
                            strcat(combined, rs);
                            result = value_create_string(combined);
                            free(ls);
                            free(rs);
                            free(combined);
                        }
                        break;
                    case TOKEN_MINUS:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_number(left.as.number - right.as.number);
                        }
                        break;
                    case TOKEN_MULTIPLY:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_number(left.as.number * right.as.number);
                        }
                        break;
                    case TOKEN_DIVIDE:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            if (right.as.number != 0) {
                                result = value_create_number(left.as.number / right.as.number);
                            } else {
                                result = value_create_exception("قسمة على صفر", 2);
                            }
                        }
                        break;
                    case TOKEN_MODULO:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_number(fmod(left.as.number, right.as.number));
                        }
                        break;
                    case TOKEN_POWER:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_number(pow(left.as.number, right.as.number));
                        }
                        break;
                    case TOKEN_EQUAL:
                        result = value_create_boolean(value_equals(&left, &right));
                        break;
                    case TOKEN_NOT_EQUAL:
                        result = value_create_boolean(!value_equals(&left, &right));
                        break;
                    case TOKEN_GREATER:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_boolean(left.as.number > right.as.number);
                        }
                        break;
                    case TOKEN_LESS:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_boolean(left.as.number < right.as.number);
                        }
                        break;
                    case TOKEN_GREATER_EQ:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_boolean(left.as.number >= right.as.number);
                        }
                        break;
                    case TOKEN_LESS_EQ:
                        if (left.type == VAL_NUMBER && right.type == VAL_NUMBER) {
                            result = value_create_boolean(left.as.number <= right.as.number);
                        }
                        break;
                    case TOKEN_AND:
                        result = value_create_boolean(value_is_truthy(&left) && value_is_truthy(&right));
                        break;
                    case TOKEN_OR:
                        result = value_create_boolean(value_is_truthy(&left) || value_is_truthy(&right));
                        break;
                    default:
                        break;
                }
                
                value_free(&left);
                value_free(&right);
                return result;
            }
            
        case AST_UNARY_OP:
            {
                Value operand = interpreter_evaluate(interp, node->as.unary_op.operand);
                Value result = value_create_null();
                
                if (operand.type == VAL_EXCEPTION) {
                    return operand;
                }
                
                switch (node->as.unary_op.op) {
                    case TOKEN_MINUS:
                        if (operand.type == VAL_NUMBER) {
                            result = value_create_number(-operand.as.number);
                        }
                        break;
                    case TOKEN_NOT:
                        result = value_create_boolean(!value_is_truthy(&operand));
                        break;
                    default:
                        break;
                }
                
                value_free(&operand);
                return result;
            }
            
        case AST_PRINT:
            {
                Value val = interpreter_evaluate(interp, node->as.print.expression);
                if (val.type == VAL_EXCEPTION) {
                    return val;
                }
                char *str = value_to_string(&val);
                printf("%s\n", str);
                free(str);
                value_free(&val);
                return value_create_null();
            }
            
        case AST_INPUT:
            {
                if (node->as.input.prompt) {
                    printf("%s", node->as.input.prompt);
                }
                char buffer[1024];
                if (fgets(buffer, sizeof(buffer), stdin)) {
                    // إزالة newline
                    buffer[strcspn(buffer, "\n")] = 0;
                    return value_create_string(buffer);
                }
                return value_create_null();
            }
            
        case AST_LET:
        case AST_CONST:
            {
                Value val = interpreter_evaluate(interp, node->as.let.value);
                if (val.type == VAL_EXCEPTION) {
                    return val;
                }
                bool is_const = (node->type == AST_CONST);
                environment_define(interp->current_env, node->as.let.name, val, is_const);
                return value_create_null();
            }
            
        case AST_ASSIGN:
            {
                Value val = interpreter_evaluate(interp, node->as.assign.value);
                if (val.type == VAL_EXCEPTION) {
                    return val;
                }
                environment_set(interp->current_env, node->as.assign.name, val);
                return value_create_null();
            }
            
        case AST_IF:
            {
                Value cond = interpreter_evaluate(interp, node->as.if_stmt.condition);
                if (cond.type == VAL_EXCEPTION) {
                    return cond;
                }
                
                bool is_true = value_is_truthy(&cond);
                value_free(&cond);
                
                if (is_true) {
                    return interpreter_evaluate(interp, node->as.if_stmt.then_branch);
                } else if (node->as.if_stmt.else_branch) {
                    return interpreter_evaluate(interp, node->as.if_stmt.else_branch);
                }
                
                return value_create_null();
            }
            
        case AST_WHILE:
            {
                Value result = value_create_null();
                
                while (true) {
                    Value cond = interpreter_evaluate(interp, node->as.while_loop.condition);
                    if (cond.type == VAL_EXCEPTION) {
                        value_free(&result);
                        return cond;
                    }
                    
                    if (!value_is_truthy(&cond)) {
                        value_free(&cond);
                        break;
                    }
                    value_free(&cond);
                    
                    value_free(&result);
                    result = interpreter_evaluate(interp, node->as.while_loop.body);
                    
                    if (result.type == VAL_EXCEPTION) {
                        return result;
                    }
                    
                    if (interp->is_breaking) {
                        interp->is_breaking = false;
                        break;
                    }
                    
                    if (interp->is_continuing) {
                        interp->is_continuing = false;
                        continue;
                    }
                    
                    if (interp->is_returning) {
                        return result;
                    }
                }
                
                return result;
            }
            
        case AST_FOR:
            {
                Value result = value_create_null();
                
                // إنشاء بيئة جديدة للحلقة
                Environment *loop_env = environment_create(interp->current_env, "حلقة");
                interp->current_env = loop_env;
                
                // تعريف متغير الحلقة
                Value start_val = interpreter_evaluate(interp, node->as.for_loop.start);
                environment_define(loop_env, node->as.for_loop.var_name, start_val, false);
                
                Value end_val = interpreter_evaluate(interp, node->as.for_loop.end);
                double end = end_val.as.number;
                
                while (true) {
                    Value *current = environment_get(loop_env, node->as.for_loop.var_name);
                    if (!current || current->as.number > end) break;
                    
                    value_free(&result);
                    result = interpreter_evaluate(interp, node->as.for_loop.body);
                    
                    if (result.type == VAL_EXCEPTION) {
                        interp->current_env = loop_env->parent;
                        environment_destroy(loop_env);
                        return result;
                    }
                    
                    if (interp->is_breaking) {
                        interp->is_breaking = false;
                        break;
                    }
                    
                    if (interp->is_continuing) {
                        interp->is_continuing = false;
                    }
                    
                    if (interp->is_returning) {
                        interp->current_env = loop_env->parent;
                        environment_destroy(loop_env);
                        return result;
                    }
                    
                    // زيادة العداد
                    current->as.number++;
                }
                
                interp->current_env = loop_env->parent;
                environment_destroy(loop_env);
                
                value_free(&end_val);
                return result;
            }
            
        case AST_ARRAY:
            {
                Value arr = value_create_array();
                for (int i = 0; i < node->as.array.count; i++) {
                    Value elem = interpreter_evaluate(interp, node->as.array.elements[i]);
                    if (elem.type == VAL_EXCEPTION) {
                        value_free(&arr);
                        return elem;
                    }
                    
                    if (arr.as.array.count >= arr.as.array.capacity) {
                        arr.as.array.capacity *= 2;
                        arr.as.array.items = realloc(arr.as.array.items, 
                                                      sizeof(Value*) * arr.as.array.capacity);
                    }
                    
                    arr.as.array.items[arr.as.array.count] = malloc(sizeof(Value));
                    *arr.as.array.items[arr.as.array.count] = elem;
                    arr.as.array.count++;
                }
                return arr;
            }
            
        case AST_ARRAY_ACCESS:
            {
                Value arr = interpreter_evaluate(interp, node->as.array_access.array);
                if (arr.type == VAL_EXCEPTION) return arr;
                
                Value idx = interpreter_evaluate(interp, node->as.array_access.index);
                if (idx.type == VAL_EXCEPTION) {
                    value_free(&arr);
                    return idx;
                }
                
                Value result = value_create_null();
                
                if (arr.type == VAL_ARRAY && idx.type == VAL_NUMBER) {
                    int index = (int)idx.as.number;
                    if (index >= 0 && index < arr.as.array.count) {
                        result = value_copy(arr.as.array.items[index]);
                    } else {
                        result = value_create_exception("فهرس خارج النطاق", 3);
                    }
                } else if (arr.type == VAL_STRING && idx.type == VAL_NUMBER) {
                    int index = (int)idx.as.number;
                    if (index >= 0 && index < (int)strlen(arr.as.string)) {
                        char ch[2] = {arr.as.string[index], '\0'};
                        result = value_create_string(ch);
                    } else {
                        result = value_create_exception("فهرس خارج النطاق", 3);
                    }
                } else {
                    result = value_create_exception("نوع غير صالح للوصول بالفهرس", 4);
                }
                
                value_free(&arr);
                value_free(&idx);
                return result;
            }
            
        case AST_FUNCTION_CALL:
            {
                // البحث عن الدالة
                Value *func_val = environment_get(interp->current_env, node->as.function_call.name);
                if (!func_val || func_val->type != VAL_FUNCTION) {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), 
                             "الدالة '%s' غير معرفة", node->as.function_call.name);
                    return value_create_exception(error_msg, 5);
                }
                
                // تقييم المعاملات
                Value *args = malloc(sizeof(Value) * node->as.function_call.arg_count);
                for (int i = 0; i < node->as.function_call.arg_count; i++) {
                    args[i] = interpreter_evaluate(interp, node->as.function_call.args[i]);
                    if (args[i].type == VAL_EXCEPTION) {
                        for (int j = 0; j < i; j++) {
                            value_free(&args[j]);
                        }
                        free(args);
                        return args[i];
                    }
                }
                
                // استدعاء الدالة الأصلية
                if (func_val->as.function.is_native) {
                    Value result = func_val->as.function.native_fn(args, node->as.function_call.arg_count);
                    for (int i = 0; i < node->as.function_call.arg_count; i++) {
                        value_free(&args[i]);
                    }
                    free(args);
                    return result;
                }
                
                // إنشاء بيئة جديدة للدالة
                Environment *func_env = environment_create(
                    func_val->as.function.closure ? func_val->as.function.closure : interp->global_env, 
                    node->as.function_call.name
                );
                func_env->is_function_scope = true;
                
                // تعريف المعاملات
                for (int i = 0; i < func_val->as.function.param_count && i < node->as.function_call.arg_count; i++) {
                    environment_define(func_env, func_val->as.function.params[i], args[i], false);
                }
                
                free(args);
                
                // تنفيذ جسم الدالة
                Environment *prev_env = interp->current_env;
                interp->current_env = func_env;
                
                Value result = interpreter_evaluate(interp, func_val->as.function.body);
                
                interp->current_env = prev_env;
                environment_destroy(func_env);
                
                if (interp->is_returning) {
                    interp->is_returning = false;
                    Value return_val = value_copy(interp->return_value);
                    if (interp->return_value) {
                        value_free(interp->return_value);
                        free(interp->return_value);
                        interp->return_value = NULL;
                    }
                    return return_val;
                }
                
                return result;
            }
            
        case AST_RETURN:
            {
                if (node->as.return_stmt.value) {
                    interp->return_value = malloc(sizeof(Value));
                    *interp->return_value = interpreter_evaluate(interp, node->as.return_stmt.value);
                }
                interp->is_returning = true;
                return value_create_null();
            }
            
        case AST_BREAK:
            interp->is_breaking = true;
            return value_create_null();
            
        case AST_CONTINUE:
            interp->is_continuing = true;
            return value_create_null();
            
        case AST_PROGRAM:
            {
                Value result = value_create_null();
                for (int i = 0; i < node->as.program.count; i++) {
                    value_free(&result);
                    result = interpreter_evaluate(interp, node->as.program.statements[i]);
                    
                    if (result.type == VAL_EXCEPTION) {
                        return result;
                    }
                    
                    if (interp->is_returning || interp->is_breaking || interp->is_continuing) {
                        return result;
                    }
                }
                return result;
            }
            
        default:
            return value_create_exception("نوع عقدة غير مدعوم", 99);
    }
}

// تشغيل البرنامج
void interpreter_run(Interpreter *interp, ASTNode *program) {
    if (!interp || !program) return;
    
    Value result = interpreter_evaluate(interp, program);
    
    if (result.type == VAL_EXCEPTION) {
        char *str = value_to_string(&result);
        fprintf(stderr, "خطأ: %s\n", str);
        free(str);
    }
    
    value_free(&result);
}
