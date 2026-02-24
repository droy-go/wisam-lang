#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// دوال القيم
Value value_create_number(double num) {
    Value v;
    v.type = VAL_NUMBER;
    v.as.number = num;
    return v;
}

Value value_create_string(const char *str) {
    Value v;
    v.type = VAL_STRING;
    v.as.string = strdup(str);
    return v;
}

Value value_create_boolean(bool boolean) {
    Value v;
    v.type = VAL_BOOLEAN;
    v.as.boolean = boolean;
    return v;
}

Value value_create_null(void) {
    Value v;
    v.type = VAL_NULL;
    return v;
}

Value value_create_array(void) {
    Value v;
    v.type = VAL_ARRAY;
    v.as.array.items = malloc(sizeof(Value*) * 10);
    v.as.array.count = 0;
    v.as.array.capacity = 10;
    return v;
}

Value value_create_object(void) {
    Value v;
    v.type = VAL_OBJECT;
    v.as.object.keys = malloc(sizeof(char*) * 10);
    v.as.object.values = malloc(sizeof(Value*) * 10);
    v.as.object.count = 0;
    v.as.object.capacity = 10;
    return v;
}

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
        default:
            break;
    }
}

char *value_to_string(Value *value) {
    static char buffer[1024];
    
    switch (value->type) {
        case VAL_NUMBER:
            if (value->as.number == (int)value->as.number) {
                snprintf(buffer, sizeof(buffer), "%d", (int)value->as.number);
            } else {
                snprintf(buffer, sizeof(buffer), "%f", value->as.number);
            }
            return strdup(buffer);
        case VAL_STRING:
            return strdup(value->as.string);
        case VAL_BOOLEAN:
            return strdup(value->as.boolean ? "صح" : "خطأ");
        case VAL_NULL:
            return strdup("فارغ");
        default:
            return strdup("<كائن>");
    }
}

// البيئة
Environment *environment_create(Environment *parent) {
    Environment *env = malloc(sizeof(Environment));
    env->var_count = 0;
    env->parent = parent;
    return env;
}

void environment_destroy(Environment *env) {
    if (!env) return;
    
    for (int i = 0; i < env->var_count; i++) {
        free(env->variables[i].name);
        value_free(&env->variables[i].value);
    }
    free(env);
}

void environment_define(Environment *env, const char *name, Value value, bool is_constant) {
    // التحقق من عدم وجود المتغير مسبقاً
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            value_free(&env->variables[i].value);
            env->variables[i].value = value;
            return;
        }
    }
    
    if (env->var_count >= MAX_VARIABLES) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للمتغيرات\n");
        exit(1);
    }
    
    env->variables[env->var_count].name = strdup(name);
    env->variables[env->var_count].value = value;
    env->variables[env->var_count].is_constant = is_constant;
    env->var_count++;
}

Value *environment_get(Environment *env, const char *name) {
    // البحث في البيئة الحالية
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            return &env->variables[i].value;
        }
    }
    
    // البحث في البيئة الأم
    if (env->parent) {
        return environment_get(env->parent, name);
    }
    
    return NULL;
}

void environment_set(Environment *env, const char *name, Value value) {
    for (int i = 0; i < env->var_count; i++) {
        if (strcmp(env->variables[i].name, name) == 0) {
            if (env->variables[i].is_constant) {
                fprintf(stderr, "خطأ: لا يمكن تعديل الثابت '%s'\n", name);
                exit(1);
            }
            value_free(&env->variables[i].value);
            env->variables[i].value = value;
            return;
        }
    }
    
    // إذا لم يوجد، ننشئ متغيراً جديداً
    environment_define(env, name, value, false);
}

bool environment_exists(Environment *env, const char *name) {
    return environment_get(env, name) != NULL;
}

// المفسر
Interpreter *interpreter_create(void) {
    Interpreter *interp = malloc(sizeof(Interpreter));
    interp->global_env = environment_create(NULL);
    interp->current_env = interp->global_env;
    
    // تهيئة المكتبات
    lib_store_init();
    lib_ai_init();
    
    return interp;
}

void interpreter_destroy(Interpreter *interpreter) {
    if (!interpreter) return;
    environment_destroy(interpreter->global_env);
    free(interpreter);
}

// تقييم التعبيرات
static Value evaluate_expression(Interpreter *interp, ASTNode *node);

static Value eval_binary_op(Interpreter *interp, ASTNode *node) {
    Value left = evaluate_expression(interp, node->as.binary_op.left);
    Value right = evaluate_expression(interp, node->as.binary_op.right);
    TokenType op = node->as.binary_op.op;
    
    // التعامل مع السلاسل النصية
    if (op == TOKEN_PLUS && (left.type == VAL_STRING || right.type == VAL_STRING)) {
        char *lstr = value_to_string(&left);
        char *rstr = value_to_string(&right);
        char *result = malloc(strlen(lstr) + strlen(rstr) + 1);
        strcpy(result, lstr);
        strcat(result, rstr);
        Value v = value_create_string(result);
        free(lstr);
        free(rstr);
        free(result);
        value_free(&left);
        value_free(&right);
        return v;
    }
    
    // العمليات الحسابية
    if (left.type != VAL_NUMBER || right.type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: العمليات الحسابية تتطلب أرقاماً\n");
        exit(1);
    }
    
    double result = 0;
    bool bool_result = false;
    
    switch (op) {
        case TOKEN_PLUS:
            result = left.as.number + right.as.number;
            break;
        case TOKEN_MINUS:
            result = left.as.number - right.as.number;
            break;
        case TOKEN_MULTIPLY:
            result = left.as.number * right.as.number;
            break;
        case TOKEN_DIVIDE:
            if (right.as.number == 0) {
                fprintf(stderr, "خطأ: القسمة على صفر\n");
                exit(1);
            }
            result = left.as.number / right.as.number;
            break;
        case TOKEN_MODULO:
            result = fmod(left.as.number, right.as.number);
            break;
        case TOKEN_POWER:
            result = pow(left.as.number, right.as.number);
            break;
        case TOKEN_GREATER:
            bool_result = left.as.number > right.as.number;
            value_free(&left);
            value_free(&right);
            return value_create_boolean(bool_result);
        case TOKEN_LESS:
            bool_result = left.as.number < right.as.number;
            value_free(&left);
            value_free(&right);
            return value_create_boolean(bool_result);
        case TOKEN_GREATER_EQ:
            bool_result = left.as.number >= right.as.number;
            value_free(&left);
            value_free(&right);
            return value_create_boolean(bool_result);
        case TOKEN_LESS_EQ:
            bool_result = left.as.number <= right.as.number;
            value_free(&left);
            value_free(&right);
            return value_create_boolean(bool_result);
        case TOKEN_EQUAL:
            bool_result = left.as.number == right.as.number;
            value_free(&left);
            value_free(&right);
            return value_create_boolean(bool_result);
        case TOKEN_NOT_EQUAL:
            bool_result = left.as.number != right.as.number;
            value_free(&left);
            value_free(&right);
            return value_create_boolean(bool_result);
        default:
            fprintf(stderr, "خطأ: عملية غير معروفة\n");
            exit(1);
    }
    
    value_free(&left);
    value_free(&right);
    return value_create_number(result);
}

static Value eval_unary_op(Interpreter *interp, ASTNode *node) {
    Value operand = evaluate_expression(interp, node->as.unary_op.operand);
    TokenType op = node->as.unary_op.op;
    
    if (op == TOKEN_MINUS) {
        if (operand.type != VAL_NUMBER) {
            fprintf(stderr, "خطأ: '-' يتطلب رقماً\n");
            exit(1);
        }
        double result = -operand.as.number;
        value_free(&operand);
        return value_create_number(result);
    } else if (op == TOKEN_NOT) {
        bool result = !(operand.type == VAL_BOOLEAN && operand.as.boolean);
        value_free(&operand);
        return value_create_boolean(result);
    }
    
    return operand;
}

static Value eval_function_call(Interpreter *interp, ASTNode *node) {
    char *name = node->as.function_call.name;
    
    // دوف مدمجة
    if (strcmp(name, "جمع") == 0 || strcmp(name, "add") == 0) {
        if (node->as.function_call.arg_count != 2) {
            fprintf(stderr, "خطأ: دالة 'جمع' تتطلب معاملين\n");
            exit(1);
        }
        Value a = evaluate_expression(interp, node->as.function_call.args[0]);
        Value b = evaluate_expression(interp, node->as.function_call.args[1]);
        if (a.type != VAL_NUMBER || b.type != VAL_NUMBER) {
            fprintf(stderr, "خطأ: 'جمع' تتطلب أرقاماً\n");
            exit(1);
        }
        double result = a.as.number + b.as.number;
        value_free(&a);
        value_free(&b);
        return value_create_number(result);
    }
    
    // البحث عن الدالة المعرفة من المستخدم
    Value *func_val = environment_get(interp->current_env, name);
    if (func_val && func_val->type == VAL_FUNCTION) {
        // إنشاء بيئة جديدة للدالة
        Environment *func_env = environment_create(interp->current_env);
        
        // تمرير المعاملات
        for (int i = 0; i < node->as.function_call.arg_count && i < func_val->as.function.param_count; i++) {
            Value arg_val = evaluate_expression(interp, node->as.function_call.args[i]);
            environment_define(func_env, func_val->as.function.params[i], arg_val, false);
        }
        
        // تنفيذ جسم الدالة
        Environment *prev_env = interp->current_env;
        interp->current_env = func_env;
        
        Value result = value_create_null();
        for (int i = 0; i < func_val->as.function.body->as.program.count; i++) {
            ASTNode *stmt = func_val->as.function.body->as.program.statements[i];
            if (stmt->type == AST_RETURN) {
                result = evaluate_expression(interp, stmt->as.return_stmt.value);
                break;
            }
            evaluate_expression(interp, stmt);
        }
        
        interp->current_env = prev_env;
        environment_destroy(func_env);
        return result;
    }
    
    fprintf(stderr, "خطأ: دالة غير معروفة '%s'\n", name);
    exit(1);
}

static Value eval_struct_access(Interpreter *interp, ASTNode *node) {
    char *obj_name = node->as.struct_access.object->as.identifier.name;
    char *field = node->as.struct_access.field;
    
    Value *obj = environment_get(interp->current_env, obj_name);
    if (!obj) {
        fprintf(stderr, "خطأ: متغير غير معروف '%s'\n", obj_name);
        exit(1);
    }
    
    if (obj->type == VAL_OBJECT) {
        for (int i = 0; i < obj->as.object.count; i++) {
            if (strcmp(obj->as.object.keys[i], field) == 0) {
                return *obj->as.object.values[i];
            }
        }
        fprintf(stderr, "خطأ: حقل غير موجود '%s'\n", field);
        exit(1);
    }
    
    fprintf(stderr, "خطأ: '%s' ليس كائنًا\n", obj_name);
    exit(1);
}

static Value evaluate_expression(Interpreter *interp, ASTNode *node) {
    if (!node) return value_create_null();
    
    switch (node->type) {
        case AST_LITERAL:
            return node->as.literal.value;
        case AST_IDENTIFIER: {
            Value *val = environment_get(interp->current_env, node->as.identifier.name);
            if (!val) {
                fprintf(stderr, "خطأ: متغير غير معروف '%s'\n", node->as.identifier.name);
                exit(1);
            }
            // إرجاع نسخة
            if (val->type == VAL_NUMBER) return value_create_number(val->as.number);
            if (val->type == VAL_STRING) return value_create_string(val->as.string);
            if (val->type == VAL_BOOLEAN) return value_create_boolean(val->as.boolean);
            return value_create_null();
        }
        case AST_BINARY_OP:
            return eval_binary_op(interp, node);
        case AST_UNARY_OP:
            return eval_unary_op(interp, node);
        case AST_FUNCTION_CALL:
            return eval_function_call(interp, node);
        case AST_STRUCT_ACCESS:
            return eval_struct_access(interp, node);
        default:
            return value_create_null();
    }
}

// تنفيذ الأوامر
static void execute_statement(Interpreter *interp, ASTNode *node);

static void execute_let(Interpreter *interp, ASTNode *node) {
    Value value = evaluate_expression(interp, node->as.let.value);
    environment_define(interp->current_env, node->as.let.name, value, false);
}

static void execute_const(Interpreter *interp, ASTNode *node) {
    Value value = evaluate_expression(interp, node->as.constant.value);
    environment_define(interp->current_env, node->as.constant.name, value, true);
}

static void execute_assign(Interpreter *interp, ASTNode *node) {
    Value value = evaluate_expression(interp, node->as.assign.value);
    environment_set(interp->current_env, node->as.assign.name, value);
}

static void execute_print(Interpreter *interp, ASTNode *node) {
    Value value = evaluate_expression(interp, node->as.print.expression);
    char *str = value_to_string(&value);
    
    // معالجة التعبيرات داخل الأقواس المعقوفة
    printf("%s\n", str);
    
    free(str);
    value_free(&value);
}

static void execute_if(Interpreter *interp, ASTNode *node) {
    Value condition = evaluate_expression(interp, node->as.if_stmt.condition);
    bool is_true = (condition.type == VAL_BOOLEAN && condition.as.boolean) ||
                   (condition.type == VAL_NUMBER && condition.as.number != 0);
    
    if (is_true) {
        for (int i = 0; i < node->as.if_stmt.then_branch->as.program.count; i++) {
            execute_statement(interp, node->as.if_stmt.then_branch->as.program.statements[i]);
        }
    } else if (node->as.if_stmt.else_branch) {
        for (int i = 0; i < node->as.if_stmt.else_branch->as.program.count; i++) {
            execute_statement(interp, node->as.if_stmt.else_branch->as.program.statements[i]);
        }
    }
    
    value_free(&condition);
}

static void execute_for(Interpreter *interp, ASTNode *node) {
    Value start_val = evaluate_expression(interp, node->as.for_loop.start);
    Value end_val = evaluate_expression(interp, node->as.for_loop.end);
    
    if (start_val.type != VAL_NUMBER || end_val.type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: حلقة 'لكل' تتطلب أرقاماً\n");
        exit(1);
    }
    
    int start = (int)start_val.as.number;
    int end = (int)end_val.as.number;
    
    for (int i = start; i <= end; i++) {
        environment_define(interp->current_env, node->as.for_loop.var_name, 
                          value_create_number(i), false);
        
        for (int j = 0; j < node->as.for_loop.body->as.program.count; j++) {
            execute_statement(interp, node->as.for_loop.body->as.program.statements[j]);
        }
    }
    
    value_free(&start_val);
    value_free(&end_val);
}

static void execute_function_def(Interpreter *interp, ASTNode *node) {
    Value func;
    func.type = VAL_FUNCTION;
    func.as.function.name = strdup(node->as.function_def.name);
    func.as.function.params = malloc(sizeof(char*) * node->as.function_def.param_count);
    func.as.function.param_count = node->as.function_def.param_count;
    
    for (int i = 0; i < node->as.function_def.param_count; i++) {
        func.as.function.params[i] = strdup(node->as.function_def.params[i]);
    }
    
    func.as.function.body = node->as.function_def.body;
    
    environment_define(interp->current_env, node->as.function_def.name, func, true);
}

static void execute_struct_def(Interpreter *interp, ASTNode *node) {
    Value struct_val;
    struct_val.type = VAL_STRUCT_DEF;
    struct_val.as.struct_def.name = strdup(node->as.struct_def.name);
    struct_val.as.struct_def.fields = malloc(sizeof(char*) * node->as.struct_def.field_count);
    struct_val.as.struct_def.field_count = node->as.struct_def.field_count;
    
    for (int i = 0; i < node->as.struct_def.field_count; i++) {
        struct_val.as.struct_def.fields[i] = strdup(node->as.struct_def.fields[i]);
    }
    
    environment_define(interp->current_env, node->as.struct_def.name, struct_val, true);
}

static void execute_import(Interpreter *interp, ASTNode *node) {
    // تسجيل المكتبة المستوردة
    char *module = node->as.import.module_name;
    environment_define(interp->current_env, module, value_create_string(module), true);
}

static void execute_mind_create(Interpreter *interp, ASTNode *node) {
    Value args[1];
    args[0] = value_create_string(node->as.mind_create.name);
    Value result = lib_mind_create(args, 1);
    environment_define(interp->current_env, node->as.mind_create.name, result, false);
}

static void execute_system_create(Interpreter *interp, ASTNode *node) {
    Value args[1];
    args[0] = value_create_string(node->as.system_create.name);
    Value result = lib_system_create(args, 1);
    environment_define(interp->current_env, node->as.system_create.name, result, false);
}

static void execute_neural_create(Interpreter *interp, ASTNode *node) {
    Value args[1];
    args[0] = value_create_string(node->as.neural_create.name);
    Value result = lib_neural_create(args, 1);
    environment_define(interp->current_env, node->as.neural_create.name, result, false);
}

static void execute_statement(Interpreter *interp, ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_LET:
            execute_let(interp, node);
            break;
        case AST_CONST:
            execute_const(interp, node);
            break;
        case AST_ASSIGN:
            execute_assign(interp, node);
            break;
        case AST_PRINT:
            execute_print(interp, node);
            break;
        case AST_IF:
            execute_if(interp, node);
            break;
        case AST_FOR:
            execute_for(interp, node);
            break;
        case AST_FUNCTION_DEF:
            execute_function_def(interp, node);
            break;
        case AST_STRUCT_DEF:
            execute_struct_def(interp, node);
            break;
        case AST_IMPORT:
            execute_import(interp, node);
            break;
        case AST_MIND_CREATE:
            execute_mind_create(interp, node);
            break;
        case AST_SYSTEM_CREATE:
            execute_system_create(interp, node);
            break;
        case AST_NEURAL_CREATE:
            execute_neural_create(interp, node);
            break;
        case AST_FUNCTION_CALL:
            eval_function_call(interp, node);
            break;
        default:
            // تعبيرات أخرى
            evaluate_expression(interp, node);
            break;
    }
}

void interpreter_run(Interpreter *interpreter, ASTNode *program) {
    if (program->type != AST_PROGRAM) {
        fprintf(stderr, "خطأ: البرنامج يجب أن يكون من نوع AST_PROGRAM\n");
        exit(1);
    }
    
    for (int i = 0; i < program->as.program.count; i++) {
        execute_statement(interpreter, program->as.program.statements[i]);
    }
}

Value interpreter_evaluate(Interpreter *interpreter, ASTNode *node) {
    return evaluate_expression(interpreter, node);
}
