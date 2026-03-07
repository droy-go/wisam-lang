#include "interpreter.h"
#include <sys/time.h>
#include <sys/resource.h>

// Global state
Variable variables[MAX_VARS];
int var_count = 0;
Function functions[MAX_FUNCS];
int func_count = 0;
WisamStruct structs[MAX_STRUCTS];
int struct_count = 0;
Module modules[MAX_MODULES];
int module_count = 0;
InterpreterState interpreter_state = {0};

// Forward declarations
static Value *call_function(const char *name, Value **args, int arg_count);

void interpreter_init(void) {
    var_count = 0;
    func_count = 0;
    struct_count = 0;
    module_count = 0;
    interpreter_state.call_stack = NULL;
    interpreter_state.stack_depth = 0;
    interpreter_state.max_depth = 1000;
    interpreter_state.in_function = false;
    interpreter_state.return_value = NULL;
    
    // Initialize built-in modules
    // This would be expanded to register all stdlib modules
}

Value *create_value(ValueType type) {
    Value *val = calloc(1, sizeof(Value));
    if (!val) return NULL;
    
    val->type = type;
    return val;
}

void free_value(Value *val) {
    if (!val) return;
    
    switch (val->type) {
        case TYPE_STRING:
            free(val->data.string);
            break;
        case TYPE_ARRAY:
            if (val->data.array) {
                for (int i = 0; i < val->data.array->count; i++) {
                    free_value(val->data.array->items[i]);
                }
                free(val->data.array->items);
                free(val->data.array);
            }
            break;
        case TYPE_STRUCT:
            if (val->data.strct) {
                free(val->data.strct->name);
                if (val->data.strct->members) {
                    for (int i = 0; i < val->data.strct->member_count; i++) {
                        free(val->data.strct->members[i].name);
                        free_value(val->data.strct->members[i].value);
                    }
                    free(val->data.strct->members);
                }
                free(val->data.strct);
            }
            break;
        case TYPE_AI_MIND:
            if (val->data.ai_mind) {
                for (int i = 0; i < val->data.ai_mind->count; i++) {
                    free(val->data.ai_mind->patterns[i]);
                    free(val->data.ai_mind->responses[i]);
                }
                free(val->data.ai_mind->patterns);
                free(val->data.ai_mind->responses);
                free(val->data.ai_mind);
            }
            break;
        case TYPE_AI_ECOSYSTEM:
            if (val->data.ai_ecosystem) {
                free(val->data.ai_ecosystem->name);
                free(val->data.ai_ecosystem);
            }
            break;
        case TYPE_NEURAL_NETWORK:
            if (val->data.neural_net) {
                free(val->data.neural_net->name);
                free(val->data.neural_net->training_data);
                free(val->data.neural_net);
            }
            break;
        default:
            break;
    }
    
    free(val);
}

Value *copy_value(Value *val) {
    if (!val) return NULL;
    
    Value *copy = create_value(val->type);
    
    switch (val->type) {
        case TYPE_NUMBER:
            copy->data.number = val->data.number;
            break;
        case TYPE_STRING:
            copy->data.string = strdup(val->data.string);
            break;
        case TYPE_BOOL:
            copy->data.boolean = val->data.boolean;
            break;
        default:
            // For complex types, just reference for now
            // In a full implementation, we'd do deep copies
            break;
    }
    
    return copy;
}

char *value_to_string(Value *val) {
    if (!val) return strdup("فارغ");
    
    char *result = malloc(1024);
    
    switch (val->type) {
        case TYPE_NUMBER:
            if (val->data.number == (int)val->data.number) {
                sprintf(result, "%d", (int)val->data.number);
            } else {
                sprintf(result, "%f", val->data.number);
            }
            break;
        case TYPE_STRING:
            free(result);
            return strdup(val->data.string);
        case TYPE_BOOL:
            strcpy(result, val->data.boolean ? "صحيح" : "خطأ");
            break;
        case TYPE_NULL:
            strcpy(result, "فارغ");
            break;
        case TYPE_STRUCT:
            sprintf(result, "<هيكل %s>", val->data.strct->name);
            break;
        case TYPE_FUNCTION:
            strcpy(result, "<دالة>");
            break;
        case TYPE_AI_MIND:
            sprintf(result, "<عقل %p>", (void*)val->data.ai_mind);
            break;
        case TYPE_AI_ECOSYSTEM:
            sprintf(result, "<منظومة %s>", val->data.ai_ecosystem->name);
            break;
        case TYPE_NEURAL_NETWORK:
            sprintf(result, "<شبكة_عصبية %s>", val->data.neural_net->name);
            break;
        default:
            strcpy(result, "<غير_معروف>");
            break;
    }
    
    return result;
}

double value_to_number(Value *val) {
    if (!val) return 0;
    
    switch (val->type) {
        case TYPE_NUMBER:
            return val->data.number;
        case TYPE_STRING:
            return atof(val->data.string);
        case TYPE_BOOL:
            return val->data.boolean ? 1 : 0;
        default:
            return 0;
    }
}

Variable *find_variable(const char *name) {
    for (int i = var_count - 1; i >= 0; i--) {
        if (strcmp(variables[i].name, name) == 0) {
            return &variables[i];
        }
    }
    return NULL;
}

void set_variable(const char *name, Value *value, bool is_const) {
    Variable *existing = find_variable(name);
    
    if (existing) {
        if (existing->is_constant) {
            runtime_error("Cannot modify constant variable", 0);
            return;
        }
        free_value(existing->value);
        existing->value = copy_value(value);
    } else {
        if (var_count >= MAX_VARS) {
            runtime_error("Too many variables", 0);
            return;
        }
        variables[var_count].name = strdup(name);
        variables[var_count].value = copy_value(value);
        variables[var_count].is_constant = is_const;
        var_count++;
    }
}

Function *find_function(const char *name) {
    for (int i = 0; i < func_count; i++) {
        if (strcmp(functions[i].name, name) == 0) {
            return &functions[i];
        }
    }
    return NULL;
}

WisamStruct *find_struct(const char *name) {
    for (int i = 0; i < struct_count; i++) {
        if (strcmp(structs[i].name, name) == 0) {
            return &structs[i];
        }
    }
    return NULL;
}

Module *find_module(const char *name) {
    for (int i = 0; i < module_count; i++) {
        if (strcmp(modules[i].name, name) == 0) {
            return &modules[i];
        }
    }
    return NULL;
}

void runtime_error(const char *message, int line) {
    fprintf(stderr, "خطأ زمني");
    if (line > 0) {
        fprintf(stderr, " في السطر %d", line);
    }
    fprintf(stderr, ": %s\n", message);
}

void type_error(const char *expected, ValueType got, int line) {
    fprintf(stderr, "خطأ نوعي في السطر %d: يتوقع %s، حصل على %d\n", 
            line, expected, got);
}

Value *interpret(ASTNode *node) {
    if (!node) return create_value(TYPE_NULL);
    
    switch (node->type) {
        case AST_PROGRAM:
        case AST_BLOCK:
            return interpret_block(node);
            
        case AST_VAR_DECL:
            return interpret_var_decl(node);
            
        case AST_CONST_DECL:
            return interpret_const_decl(node);
            
        case AST_ASSIGNMENT:
            return interpret_assignment(node);
            
        case AST_WRITE:
            return interpret_write(node);
            
        case AST_IF:
            return interpret_if(node);
            
        case AST_FOR:
            return interpret_for(node);
            
        case AST_WHILE:
            return interpret_while(node);
            
        case AST_FUNCTION_DEF:
            return interpret_func_def(node);
            
        case AST_RETURN:
            return interpret_return(node);
            
        case AST_STRUCT_DEF:
            return interpret_struct_def(node);
            
        case AST_IMPORT:
            return interpret_import(node);
            
        case AST_BINARY_OP:
            return interpret_binary_op(node);
            
        case AST_UNARY_OP:
            return interpret_unary_op(node);
            
        case AST_NUMBER_LITERAL:
            return interpret_number_literal(node);
            
        case AST_STRING_LITERAL:
            return interpret_string_literal(node);
            
        case AST_BOOL_LITERAL:
            return interpret_bool_literal(node);
            
        case AST_IDENTIFIER:
            return interpret_identifier(node);
            
        case AST_FUNCTION_CALL:
            return interpret_function_call(node);
            
        case AST_METHOD_CALL:
            return interpret_method_call(node);
            
        case AST_STRUCT_ACCESS:
            return interpret_struct_access(node);
            
        case AST_INTERPOLATED_STRING:
            return interpret_interpolated_string(node);
            
        case AST_CREATE_MIND:
            return interpret_create_mind(node);
            
        case AST_CREATE_ECOSYSTEM:
            return interpret_create_ecosystem(node);
            
        case AST_CREATE_NEURAL:
            return interpret_create_neural(node);
            
        case AST_AI_LEARN:
            return interpret_ai_learn(node);
            
        case AST_AI_ASK:
            return interpret_ai_ask(node);
            
        case AST_AI_ADD_MIND:
            return interpret_ai_add_mind(node);
            
        case AST_AI_CREATE_MEMORY:
            return interpret_ai_create_memory(node);
            
        case AST_AI_SELF_LEARN:
            return interpret_ai_self_learn(node);
            
        case AST_AI_RESPOND:
            return interpret_ai_respond(node);
            
        case AST_AI_TRAIN:
            return interpret_ai_train(node);
            
        case AST_AI_USE_GPU:
            return interpret_ai_use_gpu(node);
            
        case AST_AI_RUN_APP:
            return interpret_ai_run_app(node);
            
        case AST_AI_SAVE:
            return interpret_ai_save(node);
            
        case AST_META_ACCESS:
            return interpret_meta_access(node);
            
        default:
            runtime_error("Unknown node type", node->line);
            return create_value(TYPE_NULL);
    }
}

Value *interpret_block(ASTNode *node) {
    Value *result = create_value(TYPE_NULL);
    
    for (int i = 0; i < node->data.block.count; i++) {
        free_value(result);
        result = interpret(node->data.block.statements[i]);
        
        // Check for return value
        if (interpreter_state.return_value) {
            free_value(result);
            return interpreter_state.return_value;
        }
    }
    
    return result;
}

Value *interpret_var_decl(ASTNode *node) {
    Value *value = interpret(node->data.var_decl.value);
    set_variable(node->data.var_decl.name, value, false);
    free_value(value);
    return create_value(TYPE_NULL);
}

Value *interpret_const_decl(ASTNode *node) {
    Value *value = interpret(node->data.const_decl.value);
    set_variable(node->data.const_decl.name, value, true);
    free_value(value);
    return create_value(TYPE_NULL);
}

Value *interpret_assignment(ASTNode *node) {
    Value *value = interpret(node->data.assignment.value);
    set_variable(node->data.assignment.name, value, false);
    free_value(value);
    return create_value(TYPE_NULL);
}

Value *interpret_write(ASTNode *node) {
    Value *value = interpret(node->data.write_stmt.value);
    char *str = value_to_string(value);
    printf("%s\n", str);
    free(str);
    free_value(value);
    return create_value(TYPE_NULL);
}

Value *interpret_if(ASTNode *node) {
    Value *condition = interpret(node->data.if_stmt.condition);
    bool is_true = false;
    
    if (condition->type == TYPE_BOOL) {
        is_true = condition->data.boolean;
    } else if (condition->type == TYPE_NUMBER) {
        is_true = condition->data.number != 0;
    } else if (condition->type == TYPE_STRING) {
        is_true = strlen(condition->data.string) > 0;
    }
    
    free_value(condition);
    
    Value *result = create_value(TYPE_NULL);
    
    if (is_true) {
        for (int i = 0; i < node->data.if_stmt.then_count; i++) {
            free_value(result);
            result = interpret(node->data.if_stmt.then_body[i]);
            if (interpreter_state.return_value) {
                return interpreter_state.return_value;
            }
        }
    } else {
        for (int i = 0; i < node->data.if_stmt.else_count; i++) {
            free_value(result);
            result = interpret(node->data.if_stmt.else_body[i]);
            if (interpreter_state.return_value) {
                return interpreter_state.return_value;
            }
        }
    }
    
    return result;
}

Value *interpret_for(ASTNode *node) {
    Value *start_val = interpret(node->data.for_loop.start);
    Value *end_val = interpret(node->data.for_loop.end);
    
    int start = (int)value_to_number(start_val);
    int end = (int)value_to_number(end_val);
    
    free_value(start_val);
    free_value(end_val);
    
    Value *loop_var = create_value(TYPE_NUMBER);
    
    for (int i = start; i <= end; i++) {
        loop_var->data.number = i;
        set_variable(node->data.for_loop.var_name, loop_var, false);
        
        for (int j = 0; j < node->data.for_loop.body_count; j++) {
            interpret(node->data.for_loop.body[j]);
            if (interpreter_state.return_value) {
                free_value(loop_var);
                return interpreter_state.return_value;
            }
        }
    }
    
    free_value(loop_var);
    return create_value(TYPE_NULL);
}

Value *interpret_while(ASTNode *node) {
    Value *result = create_value(TYPE_NULL);
    
    while (1) {
        Value *condition = interpret(node->data.while_loop.condition);
        bool is_true = false;
        
        if (condition->type == TYPE_BOOL) {
            is_true = condition->data.boolean;
        } else if (condition->type == TYPE_NUMBER) {
            is_true = condition->data.number != 0;
        } else if (condition->type == TYPE_STRING) {
            is_true = strlen(condition->data.string) > 0;
        }
        
        free_value(condition);
        
        if (!is_true) break;
        
        for (int i = 0; i < node->data.while_loop.body_count; i++) {
            free_value(result);
            result = interpret(node->data.while_loop.body[i]);
            if (interpreter_state.return_value) {
                return interpreter_state.return_value;
            }
        }
    }
    
    return result;
}

Value *interpret_func_def(ASTNode *node) {
    if (func_count >= MAX_FUNCS) {
        runtime_error("Too many functions", node->line);
        return create_value(TYPE_NULL);
    }
    
    Function *func = &functions[func_count++];
    func->name = strdup(node->data.func_def.name);
    func->param_count = node->data.func_def.param_count;
    func->params = malloc(sizeof(char *) * func->param_count);
    
    for (int i = 0; i < func->param_count; i++) {
        func->params[i] = strdup(node->data.func_def.params[i]);
    }
    
    func->body_count = node->data.func_def.body_count;
    func->body = malloc(sizeof(ASTNode *) * func->body_count);
    
    for (int i = 0; i < func->body_count; i++) {
        func->body[i] = node->data.func_def.body[i];
    }
    
    return create_value(TYPE_NULL);
}

Value *interpret_return(ASTNode *node) {
    Value *value = interpret(node->data.return_stmt.value);
    interpreter_state.return_value = copy_value(value);
    return value;
}

Value *interpret_struct_def(ASTNode *node) {
    if (struct_count >= MAX_STRUCTS) {
        runtime_error("Too many structs", node->line);
        return create_value(TYPE_NULL);
    }
    
    WisamStruct *strct = &structs[struct_count++];
    strct->name = strdup(node->data.struct_def.name);
    strct->field_count = node->data.struct_def.field_count;
    strct->fields = malloc(sizeof(StructField) * strct->field_count);
    
    for (int i = 0; i < strct->field_count; i++) {
        strct->fields[i].name = strdup(node->data.struct_def.fields[i]);
        strct->fields[i].type = TYPE_NULL;
    }
    
    return create_value(TYPE_NULL);
}

Value *interpret_import(ASTNode *node) {
    // Register the module for later use
    // The actual module initialization happens in stdlib
    printf("استيراد الوحدة: %s\n", node->data.import.module_name);
    return create_value(TYPE_NULL);
}

Value *interpret_binary_op(ASTNode *node) {
    Value *left = interpret(node->data.binary_op.left);
    Value *right = interpret(node->data.binary_op.right);
    Value *result = create_value(TYPE_NUMBER);
    
    double lnum = value_to_number(left);
    double rnum = value_to_number(right);
    
    switch (node->data.binary_op.op) {
        case TOKEN_PLUS:
            if (left->type == TYPE_STRING || right->type == TYPE_STRING) {
                // String concatenation
                char *lstr = value_to_string(left);
                char *rstr = value_to_string(right);
                result->type = TYPE_STRING;
                result->data.string = malloc(strlen(lstr) + strlen(rstr) + 1);
                strcpy(result->data.string, lstr);
                strcat(result->data.string, rstr);
                free(lstr);
                free(rstr);
            } else {
                result->data.number = lnum + rnum;
            }
            break;
            
        case TOKEN_MINUS:
            result->data.number = lnum - rnum;
            break;
            
        case TOKEN_MULTIPLY:
            result->data.number = lnum * rnum;
            break;
            
        case TOKEN_DIVIDE:
            if (rnum != 0) {
                result->data.number = lnum / rnum;
            } else {
                runtime_error("Division by zero", node->line);
                result->data.number = 0;
            }
            break;
            
        case TOKEN_MODULO:
            if ((int)rnum != 0) {
                result->data.number = (int)lnum % (int)rnum;
            } else {
                runtime_error("Modulo by zero", node->line);
                result->data.number = 0;
            }
            break;
            
        case TOKEN_EQUAL:
            result->type = TYPE_BOOL;
            result->data.boolean = (lnum == rnum);
            break;
            
        case TOKEN_NOT_EQUAL:
            result->type = TYPE_BOOL;
            result->data.boolean = (lnum != rnum);
            break;
            
        case TOKEN_LESS:
            result->type = TYPE_BOOL;
            result->data.boolean = (lnum < rnum);
            break;
            
        case TOKEN_GREATER:
            result->type = TYPE_BOOL;
            result->data.boolean = (lnum > rnum);
            break;
            
        case TOKEN_LESS_EQUAL:
            result->type = TYPE_BOOL;
            result->data.boolean = (lnum <= rnum);
            break;
            
        case TOKEN_GREATER_EQUAL:
            result->type = TYPE_BOOL;
            result->data.boolean = (lnum >= rnum);
            break;
            
        default:
            runtime_error("Unknown binary operator", node->line);
            break;
    }
    
    free_value(left);
    free_value(right);
    return result;
}

Value *interpret_unary_op(ASTNode *node) {
    Value *operand = interpret(node->data.unary_op.operand);
    Value *result = create_value(TYPE_NUMBER);
    
    double val = value_to_number(operand);
    
    switch (node->data.unary_op.op) {
        case TOKEN_MINUS:
            result->data.number = -val;
            break;
        default:
            result->data.number = val;
            break;
    }
    
    free_value(operand);
    return result;
}

Value *interpret_number_literal(ASTNode *node) {
    Value *val = create_value(TYPE_NUMBER);
    val->data.number = node->data.number_literal.value;
    return val;
}

Value *interpret_string_literal(ASTNode *node) {
    Value *val = create_value(TYPE_STRING);
    val->data.string = strdup(node->data.string_literal.value);
    return val;
}

Value *interpret_bool_literal(ASTNode *node) {
    Value *val = create_value(TYPE_BOOL);
    val->data.boolean = node->data.bool_literal.value;
    return val;
}

Value *interpret_identifier(ASTNode *node) {
    Variable *var = find_variable(node->data.identifier.name);
    
    if (var) {
        return copy_value(var->value);
    }
    
    // Check if it's a struct type
    WisamStruct *strct = find_struct(node->data.identifier.name);
    if (strct) {
        // Create a new struct instance
        Value *val = create_value(TYPE_STRUCT);
        val->data.strct = malloc(sizeof(WisamStruct));
        val->data.strct->name = strdup(strct->name);
        val->data.strct->member_count = 0;
        val->data.strct->members = NULL;
        return val;
    }
    
    runtime_error("Undefined variable", node->line);
    return create_value(TYPE_NULL);
}

Value *interpret_function_call(ASTNode *node) {
    // Evaluate arguments
    Value **args = malloc(sizeof(Value *) * node->data.func_call.arg_count);
    for (int i = 0; i < node->data.func_call.arg_count; i++) {
        args[i] = interpret(node->data.func_call.args[i]);
    }
    
    Value *result = call_function(node->data.func_call.name, args, node->data.func_call.arg_count);
    
    // Free arguments
    for (int i = 0; i < node->data.func_call.arg_count; i++) {
        free_value(args[i]);
    }
    free(args);
    
    return result;
}

static Value *call_function(const char *name, Value **args, int arg_count) {
    // Check for built-in functions
    if (strcmp(name, "اطبع") == 0 || strcmp(name, "print") == 0) {
        return builtin_print(args, arg_count);
    }
    if (strcmp(name, "ادخل") == 0 || strcmp(name, "input") == 0) {
        return builtin_input(args, arg_count);
    }
    if (strcmp(name, "الطول") == 0 || strcmp(name, "length") == 0) {
        return builtin_length(args, arg_count);
    }
    if (strcmp(name, "النوع") == 0 || strcmp(name, "type") == 0) {
        return builtin_type(args, arg_count);
    }
    if (strcmp(name, "للنص") == 0 || strcmp(name, "to_string") == 0) {
        return builtin_to_string(args, arg_count);
    }
    if (strcmp(name, "للرقم") == 0 || strcmp(name, "to_number") == 0) {
        return builtin_to_number(args, arg_count);
    }
    if (strcmp(name, "عشوائي") == 0 || strcmp(name, "random") == 0) {
        return builtin_random(args, arg_count);
    }
    if (strcmp(name, "الوقت") == 0 || strcmp(name, "time") == 0) {
        return builtin_time(args, arg_count);
    }
    if (strcmp(name, "التاريخ") == 0 || strcmp(name, "date") == 0) {
        return builtin_date(args, arg_count);
    }
    
    // Find user-defined function
    Function *func = find_function(name);
    if (!func) {
        fprintf(stderr, "Undefined function: %s\n", name);
        return create_value(TYPE_NULL);
    }
    
    // Check argument count
    if (arg_count != func->param_count) {
        fprintf(stderr, "Function %s expects %d arguments, got %d\n",
                name, func->param_count, arg_count);
        return create_value(TYPE_NULL);
    }
    
    // Save current variables
    int saved_var_count = var_count;
    
    // Set up parameters as local variables
    for (int i = 0; i < func->param_count; i++) {
        set_variable(func->params[i], args[i], false);
    }
    
    // Execute function body
    interpreter_state.in_function = true;
    interpreter_state.return_value = NULL;
    
    Value *result = create_value(TYPE_NULL);
    
    for (int i = 0; i < func->body_count; i++) {
        free_value(result);
        result = interpret(func->body[i]);
        
        if (interpreter_state.return_value) {
            free_value(result);
            result = interpreter_state.return_value;
            interpreter_state.return_value = NULL;
            break;
        }
    }
    
    interpreter_state.in_function = false;
    
    // Restore variables (remove local variables)
    for (int i = var_count - 1; i >= saved_var_count; i--) {
        free(variables[i].name);
        free_value(variables[i].value);
    }
    var_count = saved_var_count;
    
    return result;
}

Value *interpret_method_call(ASTNode *node) {
    // Handle method calls like نصوص.حوّل_إلى_كبير()
    // This is a simplified implementation
    
    Variable *obj = find_variable(node->data.method_call.object);
    if (!obj) {
        fprintf(stderr, "Undefined object: %s\n", node->data.method_call.object);
        return create_value(TYPE_NULL);
    }
    
    // Evaluate arguments
    Value **args = malloc(sizeof(Value *) * node->data.method_call.arg_count);
    for (int i = 0; i < node->data.method_call.arg_count; i++) {
        args[i] = interpret(node->data.method_call.args[i]);
    }
    
    Value *result = create_value(TYPE_NULL);
    
    // Handle struct field assignment
    if (strcmp(node->data.method_call.method, "__set_field__") == 0) {
        if (obj->value->type == TYPE_STRUCT && node->data.method_call.arg_count >= 2) {
            const char *field_name = args[0]->data.string;
            
            // Find or create field
            int field_idx = -1;
            for (int i = 0; i < obj->value->data.strct->member_count; i++) {
                if (strcmp(obj->value->data.strct->members[i].name, field_name) == 0) {
                    field_idx = i;
                    break;
                }
            }
            
            if (field_idx == -1) {
                field_idx = obj->value->data.strct->member_count++;
                obj->value->data.strct->members = realloc(
                    obj->value->data.strct->members,
                    sizeof(StructMember) * obj->value->data.strct->member_count
                );
                obj->value->data.strct->members[field_idx].name = strdup(field_name);
            } else {
                free_value(obj->value->data.strct->members[field_idx].value);
            }
            
            obj->value->data.strct->members[field_idx].value = copy_value(args[1]);
        }
    }
    
    for (int i = 0; i < node->data.method_call.arg_count; i++) {
        free_value(args[i]);
    }
    free(args);
    
    return result;
}

Value *interpret_struct_access(ASTNode *node) {
    Variable *var = find_variable(node->data.struct_access.struct_name);
    
    if (!var) {
        fprintf(stderr, "Undefined struct: %s\n", node->data.struct_access.struct_name);
        return create_value(TYPE_NULL);
    }
    
    if (var->value->type != TYPE_STRUCT) {
        fprintf(stderr, "Not a struct: %s\n", node->data.struct_access.struct_name);
        return create_value(TYPE_NULL);
    }
    
    // Find the field
    for (int i = 0; i < var->value->data.strct->member_count; i++) {
        if (strcmp(var->value->data.strct->members[i].name, 
                   node->data.struct_access.field_name) == 0) {
            return copy_value(var->value->data.strct->members[i].value);
        }
    }
    
    fprintf(stderr, "Field not found: %s.%s\n", 
            node->data.struct_access.struct_name,
            node->data.struct_access.field_name);
    return create_value(TYPE_NULL);
}

Value *interpret_interpolated_string(ASTNode *node) {
    // Simplified - just return the string
    Value *val = create_value(TYPE_STRING);
    val->data.string = strdup(node->data.string_literal.value);
    return val;
}

// AI Functions
Value *interpret_create_mind(ASTNode *node) {
    Value *val = create_value(TYPE_AI_MIND);
    val->data.ai_mind = calloc(1, sizeof(AIMind));
    val->data.ai_mind->capacity = 100;
    val->data.ai_mind->patterns = malloc(sizeof(char *) * val->data.ai_mind->capacity);
    val->data.ai_mind->responses = malloc(sizeof(char *) * val->data.ai_mind->capacity);
    
    set_variable(node->data.create_mind.name, val, false);
    free_value(val);
    
    return create_value(TYPE_NULL);
}

Value *interpret_create_ecosystem(ASTNode *node) {
    Value *val = create_value(TYPE_AI_ECOSYSTEM);
    val->data.ai_ecosystem = calloc(1, sizeof(AIEcosystem));
    val->data.ai_ecosystem->name = strdup(node->data.create_ecosystem.name);
    
    set_variable(node->data.create_ecosystem.name, val, false);
    free_value(val);
    
    return create_value(TYPE_NULL);
}

Value *interpret_create_neural(ASTNode *node) {
    Value *val = create_value(TYPE_NEURAL_NETWORK);
    val->data.neural_net = calloc(1, sizeof(NeuralNetwork));
    val->data.neural_net->name = strdup(node->data.create_neural.name);
    val->data.neural_net->layers = 3;
    val->data.neural_net->learning_rate = 0.01;
    val->data.neural_net->use_gpu = false;
    
    set_variable(node->data.create_neural.name, val, false);
    free_value(val);
    
    return create_value(TYPE_NULL);
}

Value *interpret_ai_learn(ASTNode *node) {
    // Simplified AI learning
    printf("[AI] تعلم الأنماط: %d أنماط\n", node->data.ai_learn.count);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_ask(ASTNode *node) {
    Value *question = interpret(node->data.ai_ask.question);
    
    // Simplified response
    Value *response = create_value(TYPE_STRING);
    response->data.string = strdup("أهلا بك! أنا مساعد وسام.");
    
    free_value(question);
    return response;
}

Value *interpret_ai_add_mind(ASTNode *node) {
    printf("[AI] إضافة عقل إلى المنظومة: %s\n", node->data.ai_add_mind.eco_name);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_create_memory(ASTNode *node) {
    printf("[AI] إنشاء ذاكرة طويلة للمنظومة: %s\n", node->data.ai_create_memory.eco_name);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_self_learn(ASTNode *node) {
    printf("[AI] تفعيل التعلم الذاتي للمنظومة: %s\n", node->data.ai_self_learn.eco_name);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_respond(ASTNode *node) {
    Value *input = interpret(node->data.ai_respond.input);
    
    Value *response = create_value(TYPE_STRING);
    response->data.string = strdup("شكراً لمشاركتك شعورك!");
    
    free_value(input);
    return response;
}

Value *interpret_ai_train(ASTNode *node) {
    printf("[AI] تدريب الشبكة العصبية: %s باستخدام البيانات: %s\n",
           node->data.ai_train.net_name, node->data.ai_train.data_file);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_use_gpu(ASTNode *node) {
    printf("[AI] تفعيل GPU للشبكة: %s\n", node->data.ai_use_gpu.net_name);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_run_app(ASTNode *node) {
    printf("[AI] تشغيل التطبيق للمنظومة: %s\n", node->data.ai_run_app.eco_name);
    return create_value(TYPE_NULL);
}

Value *interpret_ai_save(ASTNode *node) {
    printf("[AI] حفظ الكيان: %s إلى الملف: %s\n", 
           node->data.ai_save.ai_name, node->data.ai_save.filename);
    return create_value(TYPE_NULL);
}

Value *interpret_meta_access(ASTNode *node) {
    Value *result = create_value(TYPE_STRING);
    
    if (strcmp(node->data.meta_access.property, "اسم_البرنامج") == 0 ||
        strcmp(node->data.meta_access.property, "program_name") == 0) {
        result->data.string = strdup("وسام");
    } else if (strcmp(node->data.meta_access.property, "نظام_التشغيل") == 0 ||
               strcmp(node->data.meta_access.property, "os") == 0) {
        result->data.string = strdup(WISAM_OS);
    } else if (strcmp(node->data.meta_access.property, "استخدام_الذاكرة") == 0 ||
               strcmp(node->data.meta_access.property, "memory_usage") == 0) {
        #ifdef _WIN32
            result->data.string = strdup("غير متوفر على Windows");
        #else
            struct rusage usage;
            if (getrusage(RUSAGE_SELF, &usage) == 0) {
                char mem_str[64];
                sprintf(mem_str, "%ld KB", usage.ru_maxrss);
                result->data.string = strdup(mem_str);
            } else {
                result->data.string = strdup("غير معروف");
            }
        #endif
    } else if (strcmp(node->data.meta_access.property, "الإصدار") == 0 ||
               strcmp(node->data.meta_access.property, "version") == 0) {
        result->data.string = strdup(WISAM_VERSION);
    } else {
        result->data.string = strdup("غير معروف");
    }
    
    return result;
}

// Built-in functions
Value *builtin_print(Value **args, int arg_count) {
    for (int i = 0; i < arg_count; i++) {
        char *str = value_to_string(args[i]);
        printf("%s", str);
        free(str);
        if (i < arg_count - 1) printf(" ");
    }
    printf("\n");
    return create_value(TYPE_NULL);
}

Value *builtin_input(Value **args, int arg_count) {
    if (arg_count > 0) {
        char *prompt = value_to_string(args[0]);
        printf("%s", prompt);
        free(prompt);
    }
    
    char buffer[1024];
    if (fgets(buffer, sizeof(buffer), stdin)) {
        // Remove newline
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
        
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup(buffer);
        return result;
    }
    
    return create_value(TYPE_NULL);
}

Value *builtin_length(Value **args, int arg_count) {
    if (arg_count < 1) {
        return create_value(TYPE_NULL);
    }
    
    Value *result = create_value(TYPE_NUMBER);
    
    if (args[0]->type == TYPE_STRING) {
        result->data.number = strlen(args[0]->data.string);
    } else if (args[0]->type == TYPE_ARRAY) {
        result->data.number = args[0]->data.array->count;
    } else {
        result->data.number = 0;
    }
    
    return result;
}

Value *builtin_type(Value **args, int arg_count) {
    if (arg_count < 1) {
        return create_value(TYPE_NULL);
    }
    
    Value *result = create_value(TYPE_STRING);
    
    switch (args[0]->type) {
        case TYPE_NUMBER:
            result->data.string = strdup("رقم");
            break;
        case TYPE_STRING:
            result->data.string = strdup("نص");
            break;
        case TYPE_BOOL:
            result->data.string = strdup("منطقي");
            break;
        case TYPE_ARRAY:
            result->data.string = strdup("مصفوفة");
            break;
        case TYPE_STRUCT:
            result->data.string = strdup("هيكل");
            break;
        case TYPE_FUNCTION:
            result->data.string = strdup("دالة");
            break;
        default:
            result->data.string = strdup("غير_معروف");
            break;
    }
    
    return result;
}

Value *builtin_to_string(Value **args, int arg_count) {
    if (arg_count < 1) {
        return create_value(TYPE_NULL);
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = value_to_string(args[0]);
    return result;
}

Value *builtin_to_number(Value **args, int arg_count) {
    if (arg_count < 1) {
        return create_value(TYPE_NULL);
    }
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = value_to_number(args[0]);
    return result;
}

Value *builtin_random(Value **args, int arg_count) {
    Value *result = create_value(TYPE_NUMBER);
    
    if (arg_count >= 2) {
        int min = (int)value_to_number(args[0]);
        int max = (int)value_to_number(args[1]);
        result->data.number = min + rand() % (max - min + 1);
    } else {
        result->data.number = (double)rand() / RAND_MAX;
    }
    
    return result;
}

Value *builtin_time(Value **args, int arg_count) {
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = (double)time(NULL);
    return result;
}

Value *builtin_date(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(buffer);
    return result;
}

void interpreter_cleanup(void) {
    // Clean up all variables
    for (int i = 0; i < var_count; i++) {
        free(variables[i].name);
        free_value(variables[i].value);
    }
    var_count = 0;
    
    // Clean up functions
    for (int i = 0; i < func_count; i++) {
        free(functions[i].name);
        for (int j = 0; j < functions[i].param_count; j++) {
            free(functions[i].params[j]);
        }
        free(functions[i].params);
        free(functions[i].body);
    }
    func_count = 0;
    
    // Clean up structs
    for (int i = 0; i < struct_count; i++) {
        free(structs[i].name);
        for (int j = 0; j < structs[i].field_count; j++) {
            free(structs[i].fields[j].name);
        }
        free(structs[i].fields);
    }
    struct_count = 0;
    
    // Clean up return value
    if (interpreter_state.return_value) {
        free_value(interpreter_state.return_value);
        interpreter_state.return_value = NULL;
    }
}
