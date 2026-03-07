#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// AST Node Types
typedef enum {
    // Statements
    AST_PROGRAM,
    AST_VAR_DECL,
    AST_CONST_DECL,
    AST_ASSIGNMENT,
    AST_WRITE,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_FUNCTION_DEF,
    AST_RETURN,
    AST_STRUCT_DEF,
    AST_IMPORT,
    AST_BLOCK,
    
    // Expressions
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_NUMBER_LITERAL,
    AST_STRING_LITERAL,
    AST_BOOL_LITERAL,
    AST_IDENTIFIER,
    AST_ARRAY_LITERAL,
    AST_STRUCT_ACCESS,
    AST_FUNCTION_CALL,
    AST_METHOD_CALL,
    AST_INTERPOLATED_STRING,
    
    // AI-specific
    AST_CREATE_MIND,
    AST_CREATE_ECOSYSTEM,
    AST_CREATE_NEURAL,
    AST_AI_LEARN,
    AST_AI_ASK,
    AST_AI_ADD_MIND,
    AST_AI_CREATE_MEMORY,
    AST_AI_SELF_LEARN,
    AST_AI_RESPOND,
    AST_AI_TRAIN,
    AST_AI_USE_GPU,
    AST_AI_RUN_APP,
    AST_AI_SAVE,
    AST_META_ACCESS
} ASTNodeType;

typedef struct ASTNode ASTNode;

struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    
    union {
        // Variable declaration
        struct {
            char *name;
            ASTNode *value;
        } var_decl;
        
        // Constant declaration
        struct {
            char *name;
            ASTNode *value;
        } const_decl;
        
        // Assignment
        struct {
            char *name;
            ASTNode *value;
        } assignment;
        
        // Binary operation
        struct {
            ASTNode *left;
            ASTNode *right;
            TokenType op;
        } binary_op;
        
        // Unary operation
        struct {
            ASTNode *operand;
            TokenType op;
        } unary_op;
        
        // Literal values
        struct {
            double value;
        } number_literal;
        
        struct {
            char *value;
        } string_literal;
        
        struct {
            bool value;
        } bool_literal;
        
        struct {
            char *name;
        } identifier;
        
        // Interpolated string
        struct {
            char **parts;
            ASTNode **expressions;
            int part_count;
        } interpolated_string;
        
        // If statement
        struct {
            ASTNode *condition;
            ASTNode **then_body;
            int then_count;
            ASTNode **else_body;
            int else_count;
        } if_stmt;
        
        // For loop
        struct {
            char *var_name;
            ASTNode *start;
            ASTNode *end;
            ASTNode **body;
            int body_count;
        } for_loop;
        
        // While loop
        struct {
            ASTNode *condition;
            ASTNode **body;
            int body_count;
        } while_loop;
        
        // Function definition
        struct {
            char *name;
            char **params;
            int param_count;
            ASTNode **body;
            int body_count;
        } func_def;
        
        // Function call
        struct {
            char *name;
            ASTNode **args;
            int arg_count;
        } func_call;
        
        // Method call
        struct {
            char *object;
            char *method;
            ASTNode **args;
            int arg_count;
        } method_call;
        
        // Struct definition
        struct {
            char *name;
            char **fields;
            int field_count;
        } struct_def;
        
        // Struct access
        struct {
            char *struct_name;
            char *field_name;
        } struct_access;
        
        // Import
        struct {
            char *module_name;
        } import;
        
        // Block
        struct {
            ASTNode **statements;
            int count;
        } block;
        
        // Write statement
        struct {
            ASTNode *value;
        } write_stmt;
        
        // Return statement
        struct {
            ASTNode *value;
        } return_stmt;
        
        // AI Mind creation
        struct {
            char *name;
        } create_mind;
        
        // AI Ecosystem creation
        struct {
            char *name;
        } create_ecosystem;
        
        // Neural Network creation
        struct {
            char *name;
        } create_neural;
        
        // AI Learn
        struct {
            char *mind_name;
            char **patterns;
            char **responses;
            int count;
        } ai_learn;
        
        // AI Ask
        struct {
            char *mind_name;
            ASTNode *question;
        } ai_ask;
        
        // AI Add mind to ecosystem
        struct {
            char *eco_name;
            char *mind_type;
            char *mind_name;
        } ai_add_mind;
        
        // AI Create memory
        struct {
            char *eco_name;
            char *memory_type;
        } ai_create_memory;
        
        // AI Self learn
        struct {
            char *eco_name;
        } ai_self_learn;
        
        // AI Respond
        struct {
            char *eco_name;
            ASTNode *input;
        } ai_respond;
        
        // AI Train neural network
        struct {
            char *net_name;
            char *data_file;
        } ai_train;
        
        // AI Use GPU
        struct {
            char *net_name;
        } ai_use_gpu;
        
        // AI Run app
        struct {
            char *eco_name;
        } ai_run_app;
        
        // AI Save
        struct {
            char *ai_name;
            char *filename;
        } ai_save;
        
        // Meta access
        struct {
            char *property;
        } meta_access;
        
    } data;
};

typedef struct {
    Lexer *lexer;
    Token *current;
    Token *peek;
} Parser;

// Parser functions
Parser *parser_create(Lexer *lexer);
void parser_destroy(Parser *parser);
ASTNode *parser_parse(Parser *parser);
void ast_destroy(ASTNode *node);
void ast_print(ASTNode *node, int indent);

// Parsing helpers
void parser_advance(Parser *parser);
bool parser_match(Parser *parser, TokenType type);
bool parser_check(Parser *parser, TokenType type);
Token *parser_consume(Parser *parser, TokenType type, const char *message);

// Grammar parsing functions
ASTNode *parse_statement(Parser *parser);
ASTNode *parse_expression(Parser *parser);
ASTNode *parse_primary(Parser *parser);
ASTNode *parse_var_decl(Parser *parser);
ASTNode *parse_const_decl(Parser *parser);
ASTNode *parse_if_stmt(Parser *parser);
ASTNode *parse_for_loop(Parser *parser);
ASTNode *parse_while_loop(Parser *parser);
ASTNode *parse_func_def(Parser *parser);
ASTNode *parse_func_call(Parser *parser, char *name);
ASTNode *parse_struct_def(Parser *parser);
ASTNode *parse_import(Parser *parser);
ASTNode *parse_write(Parser *parser);
ASTNode *parse_return(Parser *parser);
ASTNode *parse_assignment_or_call(Parser *parser);
ASTNode *parse_block(Parser *parser);
ASTNode *parse_ai_stmt(Parser *parser);
ASTNode *parse_interpolated_string(Parser *parser, char *start);

#endif // PARSER_H
