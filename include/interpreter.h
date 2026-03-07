#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "parser.h"

// Interpreter state
typedef struct {
    ASTNode **call_stack;
    int stack_depth;
    int max_depth;
    bool in_function;
    Value *return_value;
} InterpreterState;

extern InterpreterState interpreter_state;

// Interpreter functions
void interpreter_init(void);
Value *interpret(ASTNode *node);
Value *interpret_program(ASTNode *node);
Value *interpret_statement(ASTNode *node);
Value *interpret_expression(ASTNode *node);

// Statement interpreters
Value *interpret_var_decl(ASTNode *node);
Value *interpret_const_decl(ASTNode *node);
Value *interpret_assignment(ASTNode *node);
Value *interpret_write(ASTNode *node);
Value *interpret_if(ASTNode *node);
Value *interpret_for(ASTNode *node);
Value *interpret_while(ASTNode *node);
Value *interpret_func_def(ASTNode *node);
Value *interpret_return(ASTNode *node);
Value *interpret_struct_def(ASTNode *node);
Value *interpret_import(ASTNode *node);
Value *interpret_block(ASTNode *node);

// Expression interpreters
Value *interpret_binary_op(ASTNode *node);
Value *interpret_unary_op(ASTNode *node);
Value *interpret_number_literal(ASTNode *node);
Value *interpret_string_literal(ASTNode *node);
Value *interpret_bool_literal(ASTNode *node);
Value *interpret_identifier(ASTNode *node);
Value *interpret_function_call(ASTNode *node);
Value *interpret_method_call(ASTNode *node);
Value *interpret_struct_access(ASTNode *node);
Value *interpret_interpolated_string(ASTNode *node);

// AI interpreters
Value *interpret_create_mind(ASTNode *node);
Value *interpret_create_ecosystem(ASTNode *node);
Value *interpret_create_neural(ASTNode *node);
Value *interpret_ai_learn(ASTNode *node);
Value *interpret_ai_ask(ASTNode *node);
Value *interpret_ai_add_mind(ASTNode *node);
Value *interpret_ai_create_memory(ASTNode *node);
Value *interpret_ai_self_learn(ASTNode *node);
Value *interpret_ai_respond(ASTNode *node);
Value *interpret_ai_train(ASTNode *node);
Value *interpret_ai_use_gpu(ASTNode *node);
Value *interpret_ai_run_app(ASTNode *node);
Value *interpret_ai_save(ASTNode *node);
Value *interpret_meta_access(ASTNode *node);

// Variable and function management
Variable *find_variable(const char *name);
void set_variable(const char *name, Value *value, bool is_const);
Function *find_function(const char *name);
WisamStruct *find_struct(const char *name);
Module *find_module(const char *name);

// Built-in functions
Value *builtin_print(Value **args, int arg_count);
Value *builtin_input(Value **args, int arg_count);
Value *builtin_length(Value **args, int arg_count);
Value *builtin_type(Value **args, int arg_count);
Value *builtin_to_string(Value **args, int arg_count);
Value *builtin_to_number(Value **args, int arg_count);
Value *builtin_random(Value **args, int arg_count);
Value *builtin_time(Value **args, int arg_count);
Value *builtin_date(Value **args, int arg_count);

// Error handling
void runtime_error(const char *message, int line);
void type_error(const char *expected, ValueType got, int line);

// Memory management
void interpreter_cleanup(void);

#endif // INTERPRETER_H
