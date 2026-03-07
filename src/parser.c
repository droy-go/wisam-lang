#include "parser.h"
#include <stdarg.h>

// Forward declarations
static ASTNode *parse_comparison(Parser *parser);
static ASTNode *parse_term(Parser *parser);
static ASTNode *parse_factor(Parser *parser);
static ASTNode *parse_unary(Parser *parser);

Parser *parser_create(Lexer *lexer) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->current = lexer_next_token(lexer);
    parser->peek = lexer_next_token(lexer);
    
    return parser;
}

void parser_destroy(Parser *parser) {
    if (parser) {
        token_destroy(parser->current);
        token_destroy(parser->peek);
        free(parser);
    }
}

void parser_advance(Parser *parser) {
    token_destroy(parser->current);
    parser->current = parser->peek;
    parser->peek = lexer_next_token(parser->lexer);
}

bool parser_match(Parser *parser, TokenType type) {
    if (parser->current->type == type) {
        parser_advance(parser);
        return true;
    }
    return false;
}

bool parser_check(Parser *parser, TokenType type) {
    return parser->current->type == type;
}

Token *parser_consume(Parser *parser, TokenType type, const char *message) {
    if (parser->current->type == type) {
        Token *token = parser->current;
        parser->current = NULL; // Prevent double free
        parser_advance(parser);
        return token;
    }
    
    fprintf(stderr, "Parse error at line %d, column %d: %s\n", 
            parser->current->line, parser->current->column, message);
    fprintf(stderr, "Expected %s, got %s\n", 
            token_type_to_string(type), 
            token_type_to_string(parser->current->type));
    return NULL;
}

static void skip_newlines(Parser *parser) {
    while (parser->current->type == TOKEN_NEWLINE) {
        parser_advance(parser);
    }
}

ASTNode *ast_create_node(ASTNodeType type) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    if (!node) return NULL;
    
    node->type = type;
    return node;
}

void ast_destroy(ASTNode *node) {
    if (!node) return;
    
    switch (node->type) {
        case AST_VAR_DECL:
            free(node->data.var_decl.name);
            ast_destroy(node->data.var_decl.value);
            break;
            
        case AST_CONST_DECL:
            free(node->data.const_decl.name);
            ast_destroy(node->data.const_decl.value);
            break;
            
        case AST_ASSIGNMENT:
            free(node->data.assignment.name);
            ast_destroy(node->data.assignment.value);
            break;
            
        case AST_BINARY_OP:
            ast_destroy(node->data.binary_op.left);
            ast_destroy(node->data.binary_op.right);
            break;
            
        case AST_UNARY_OP:
            ast_destroy(node->data.unary_op.operand);
            break;
            
        case AST_STRING_LITERAL:
            free(node->data.string_literal.value);
            break;
            
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
            
        case AST_IF:
            ast_destroy(node->data.if_stmt.condition);
            for (int i = 0; i < node->data.if_stmt.then_count; i++) {
                ast_destroy(node->data.if_stmt.then_body[i]);
            }
            free(node->data.if_stmt.then_body);
            for (int i = 0; i < node->data.if_stmt.else_count; i++) {
                ast_destroy(node->data.if_stmt.else_body[i]);
            }
            free(node->data.if_stmt.else_body);
            break;
            
        case AST_FOR:
            free(node->data.for_loop.var_name);
            ast_destroy(node->data.for_loop.start);
            ast_destroy(node->data.for_loop.end);
            for (int i = 0; i < node->data.for_loop.body_count; i++) {
                ast_destroy(node->data.for_loop.body[i]);
            }
            free(node->data.for_loop.body);
            break;
            
        case AST_WHILE:
            ast_destroy(node->data.while_loop.condition);
            for (int i = 0; i < node->data.while_loop.body_count; i++) {
                ast_destroy(node->data.while_loop.body[i]);
            }
            free(node->data.while_loop.body);
            break;
            
        case AST_FUNCTION_DEF:
            free(node->data.func_def.name);
            for (int i = 0; i < node->data.func_def.param_count; i++) {
                free(node->data.func_def.params[i]);
            }
            free(node->data.func_def.params);
            for (int i = 0; i < node->data.func_def.body_count; i++) {
                ast_destroy(node->data.func_def.body[i]);
            }
            free(node->data.func_def.body);
            break;
            
        case AST_FUNCTION_CALL:
            free(node->data.func_call.name);
            for (int i = 0; i < node->data.func_call.arg_count; i++) {
                ast_destroy(node->data.func_call.args[i]);
            }
            free(node->data.func_call.args);
            break;
            
        case AST_METHOD_CALL:
            free(node->data.method_call.object);
            free(node->data.method_call.method);
            for (int i = 0; i < node->data.method_call.arg_count; i++) {
                ast_destroy(node->data.method_call.args[i]);
            }
            free(node->data.method_call.args);
            break;
            
        case AST_STRUCT_DEF:
            free(node->data.struct_def.name);
            for (int i = 0; i < node->data.struct_def.field_count; i++) {
                free(node->data.struct_def.fields[i]);
            }
            free(node->data.struct_def.fields);
            break;
            
        case AST_STRUCT_ACCESS:
            free(node->data.struct_access.struct_name);
            free(node->data.struct_access.field_name);
            break;
            
        case AST_IMPORT:
            free(node->data.import.module_name);
            break;
            
        case AST_BLOCK:
            for (int i = 0; i < node->data.block.count; i++) {
                ast_destroy(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
            
        case AST_WRITE:
            ast_destroy(node->data.write_stmt.value);
            break;
            
        case AST_RETURN:
            ast_destroy(node->data.return_stmt.value);
            break;
            
        case AST_INTERPOLATED_STRING:
            for (int i = 0; i < node->data.interpolated_string.part_count; i++) {
                free(node->data.interpolated_string.parts[i]);
                ast_destroy(node->data.interpolated_string.expressions[i]);
            }
            free(node->data.interpolated_string.parts);
            free(node->data.interpolated_string.expressions);
            break;
            
        case AST_PROGRAM:
            for (int i = 0; i < node->data.block.count; i++) {
                ast_destroy(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
            
        default:
            break;
    }
    
    free(node);
}

ASTNode *parser_parse(Parser *parser) {
    ASTNode *program = ast_create_node(AST_PROGRAM);
    if (!program) return NULL;
    
    program->data.block.statements = NULL;
    program->data.block.count = 0;
    
    skip_newlines(parser);
    
    while (parser->current->type != TOKEN_EOF) {
        skip_newlines(parser);
        if (parser->current->type == TOKEN_EOF) break;
        
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            program->data.block.count++;
            program->data.block.statements = realloc(
                program->data.block.statements,
                sizeof(ASTNode *) * program->data.block.count
            );
            program->data.block.statements[program->data.block.count - 1] = stmt;
        } else {
            // Error recovery - skip to next newline
            while (parser->current->type != TOKEN_NEWLINE && 
                   parser->current->type != TOKEN_EOF) {
                parser_advance(parser);
            }
        }
        
        skip_newlines(parser);
    }
    
    return program;
}

ASTNode *parse_statement(Parser *parser) {
    skip_newlines(parser);
    
    switch (parser->current->type) {
        case TOKEN_LET:
            return parse_var_decl(parser);
            
        case TOKEN_CONST:
            return parse_const_decl(parser);
            
        case TOKEN_IF:
            return parse_if_stmt(parser);
            
        case TOKEN_FOR:
            return parse_for_loop(parser);
            
        case TOKEN_WHILE:
            return parse_while_loop(parser);
            
        case TOKEN_FUNCTION:
            return parse_func_def(parser);
            
        case TOKEN_STRUCT:
            return parse_struct_def(parser);
            
        case TOKEN_IMPORT:
            return parse_import(parser);
            
        case TOKEN_WRITE:
            return parse_write(parser);
            
        case TOKEN_RETURN:
            return parse_return(parser);
            
        case TOKEN_CREATE_MIND:
        case TOKEN_CREATE_ECOSYSTEM:
        case TOKEN_CREATE_NEURAL:
            return parse_ai_stmt(parser);
            
        case TOKEN_IDENTIFIER:
            return parse_assignment_or_call(parser);
            
        default:
            fprintf(stderr, "Unexpected token: %s\n", 
                    token_type_to_string(parser->current->type));
            return NULL;
    }
}

ASTNode *parse_var_decl(Parser *parser) {
    parser_advance(parser); // consume 'ليكن'
    
    Token *name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected variable name");
    if (!name) return NULL;
    
    parser_consume(parser, TOKEN_IS, "Expected 'هو' after variable name");
    
    ASTNode *value = parse_expression(parser);
    
    ASTNode *node = ast_create_node(AST_VAR_DECL);
    node->data.var_decl.name = strdup(name->value);
    node->data.var_decl.value = value;
    
    token_destroy(name);
    return node;
}

ASTNode *parse_const_decl(Parser *parser) {
    parser_advance(parser); // consume 'ثابت'
    
    Token *name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected constant name");
    if (!name) return NULL;
    
    parser_consume(parser, TOKEN_ASSIGN, "Expected '=' after constant name");
    
    ASTNode *value = parse_expression(parser);
    
    ASTNode *node = ast_create_node(AST_CONST_DECL);
    node->data.const_decl.name = strdup(name->value);
    node->data.const_decl.value = value;
    
    token_destroy(name);
    return node;
}

ASTNode *parse_if_stmt(Parser *parser) {
    parser_advance(parser); // consume 'إذا'
    
    ASTNode *condition = parse_expression(parser);
    
    parser_consume(parser, TOKEN_THEN, "Expected 'إذن' after condition");
    
    ASTNode *node = ast_create_node(AST_IF);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_body = NULL;
    node->data.if_stmt.then_count = 0;
    node->data.if_stmt.else_body = NULL;
    node->data.if_stmt.else_count = 0;
    
    skip_newlines(parser);
    
    // Parse then block
    while (parser->current->type != TOKEN_ELSE && 
           parser->current->type != TOKEN_END &&
           parser->current->type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            node->data.if_stmt.then_count++;
            node->data.if_stmt.then_body = realloc(
                node->data.if_stmt.then_body,
                sizeof(ASTNode *) * node->data.if_stmt.then_count
            );
            node->data.if_stmt.then_body[node->data.if_stmt.then_count - 1] = stmt;
        }
        skip_newlines(parser);
    }
    
    // Parse else block if present
    if (parser_match(parser, TOKEN_ELSE)) {
        skip_newlines(parser);
        while (parser->current->type != TOKEN_END &&
               parser->current->type != TOKEN_EOF) {
            ASTNode *stmt = parse_statement(parser);
            if (stmt) {
                node->data.if_stmt.else_count++;
                node->data.if_stmt.else_body = realloc(
                    node->data.if_stmt.else_body,
                    sizeof(ASTNode *) * node->data.if_stmt.else_count
                );
                node->data.if_stmt.else_body[node->data.if_stmt.else_count - 1] = stmt;
            }
            skip_newlines(parser);
        }
    }
    
    parser_consume(parser, TOKEN_END, "Expected 'انتهى' to close if statement");
    
    return node;
}

ASTNode *parse_for_loop(Parser *parser) {
    parser_advance(parser); // consume 'لكل'
    
    Token *var = parser_consume(parser, TOKEN_IDENTIFIER, "Expected loop variable");
    if (!var) return NULL;
    
    parser_consume(parser, TOKEN_FROM, "Expected 'من' after variable");
    
    ASTNode *start = parse_expression(parser);
    
    parser_consume(parser, TOKEN_TO, "Expected 'إلى' after start value");
    
    ASTNode *end = parse_expression(parser);
    
    ASTNode *node = ast_create_node(AST_FOR);
    node->data.for_loop.var_name = strdup(var->value);
    node->data.for_loop.start = start;
    node->data.for_loop.end = end;
    node->data.for_loop.body = NULL;
    node->data.for_loop.body_count = 0;
    
    token_destroy(var);
    
    skip_newlines(parser);
    
    // Parse body
    while (parser->current->type != TOKEN_END &&
           parser->current->type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            node->data.for_loop.body_count++;
            node->data.for_loop.body = realloc(
                node->data.for_loop.body,
                sizeof(ASTNode *) * node->data.for_loop.body_count
            );
            node->data.for_loop.body[node->data.for_loop.body_count - 1] = stmt;
        }
        skip_newlines(parser);
    }
    
    parser_consume(parser, TOKEN_END, "Expected 'انتهى' to close for loop");
    
    return node;
}

ASTNode *parse_while_loop(Parser *parser) {
    parser_advance(parser); // consume 'طالما'
    
    ASTNode *condition = parse_expression(parser);
    
    ASTNode *node = ast_create_node(AST_WHILE);
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = NULL;
    node->data.while_loop.body_count = 0;
    
    skip_newlines(parser);
    
    // Parse body
    while (parser->current->type != TOKEN_END &&
           parser->current->type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            node->data.while_loop.body_count++;
            node->data.while_loop.body = realloc(
                node->data.while_loop.body,
                sizeof(ASTNode *) * node->data.while_loop.body_count
            );
            node->data.while_loop.body[node->data.while_loop.body_count - 1] = stmt;
        }
        skip_newlines(parser);
    }
    
    parser_consume(parser, TOKEN_END, "Expected 'انتهى' to close while loop");
    
    return node;
}

ASTNode *parse_func_def(Parser *parser) {
    parser_advance(parser); // consume 'دالة'
    
    Token *name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected function name");
    if (!name) return NULL;
    
    parser_consume(parser, TOKEN_TAKES, "Expected 'تأخذ' after function name");
    
    ASTNode *node = ast_create_node(AST_FUNCTION_DEF);
    node->data.func_def.name = strdup(name->value);
    node->data.func_def.params = NULL;
    node->data.func_def.param_count = 0;
    node->data.func_def.body = NULL;
    node->data.func_def.body_count = 0;
    
    token_destroy(name);
    
    // Parse parameters
    while (parser->current->type == TOKEN_IDENTIFIER) {
        Token *param = parser_consume(parser, TOKEN_IDENTIFIER, "Expected parameter name");
        node->data.func_def.param_count++;
        node->data.func_def.params = realloc(
            node->data.func_def.params,
            sizeof(char *) * node->data.func_def.param_count
        );
        node->data.func_def.params[node->data.func_def.param_count - 1] = strdup(param->value);
        token_destroy(param);
        
        if (parser->current->type == TOKEN_AND) {
            parser_advance(parser); // consume 'و'
        }
    }
    
    skip_newlines(parser);
    
    // Parse body
    while (parser->current->type != TOKEN_END &&
           parser->current->type != TOKEN_EOF) {
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            node->data.func_def.body_count++;
            node->data.func_def.body = realloc(
                node->data.func_def.body,
                sizeof(ASTNode *) * node->data.func_def.body_count
            );
            node->data.func_def.body[node->data.func_def.body_count - 1] = stmt;
        }
        skip_newlines(parser);
    }
    
    parser_consume(parser, TOKEN_END, "Expected 'انتهى' to close function");
    
    return node;
}

ASTNode *parse_struct_def(Parser *parser) {
    parser_advance(parser); // consume 'هيكل'
    
    Token *name = parser_consume(parser, TOKEN_IDENTIFIER, "Expected struct name");
    if (!name) return NULL;
    
    ASTNode *node = ast_create_node(AST_STRUCT_DEF);
    node->data.struct_def.name = strdup(name->value);
    node->data.struct_def.fields = NULL;
    node->data.struct_def.field_count = 0;
    
    token_destroy(name);
    
    skip_newlines(parser);
    
    // Parse fields
    while (parser->current->type == TOKEN_IDENTIFIER) {
        Token *field = parser_consume(parser, TOKEN_IDENTIFIER, "Expected field name");
        node->data.struct_def.field_count++;
        node->data.struct_def.fields = realloc(
            node->data.struct_def.fields,
            sizeof(char *) * node->data.struct_def.field_count
        );
        node->data.struct_def.fields[node->data.struct_def.field_count - 1] = strdup(field->value);
        token_destroy(field);
        
        skip_newlines(parser);
    }
    
    parser_consume(parser, TOKEN_END, "Expected 'انتهى' to close struct definition");
    
    return node;
}

ASTNode *parse_import(Parser *parser) {
    parser_advance(parser); // consume 'استورد'
    
    Token *module = parser_consume(parser, TOKEN_IDENTIFIER, "Expected module name");
    if (!module) return NULL;
    
    ASTNode *node = ast_create_node(AST_IMPORT);
    node->data.import.module_name = strdup(module->value);
    
    token_destroy(module);
    return node;
}

ASTNode *parse_write(Parser *parser) {
    parser_advance(parser); // consume 'اكتب'
    
    ASTNode *node = ast_create_node(AST_WRITE);
    node->data.write_stmt.value = parse_expression(parser);
    
    return node;
}

ASTNode *parse_return(Parser *parser) {
    parser_advance(parser); // consume 'أعد'
    
    ASTNode *node = ast_create_node(AST_RETURN);
    node->data.return_stmt.value = parse_expression(parser);
    
    return node;
}

ASTNode *parse_assignment_or_call(Parser *parser) {
    Token *name = parser->current;
    char *name_str = strdup(name->value);
    parser_advance(parser);
    
    if (parser->current->type == TOKEN_ASSIGN) {
        // Assignment
        parser_advance(parser); // consume '='
        
        ASTNode *value = parse_expression(parser);
        
        ASTNode *node = ast_create_node(AST_ASSIGNMENT);
        node->data.assignment.name = name_str;
        node->data.assignment.value = value;
        
        return node;
    } else if (parser->current->type == TOKEN_DOT) {
        // Struct field assignment or method call
        parser_advance(parser); // consume '.'
        
        Token *field = parser_consume(parser, TOKEN_IDENTIFIER, "Expected field name");
        
        if (parser->current->type == TOKEN_ASSIGN) {
            parser_advance(parser);
            ASTNode *value = parse_expression(parser);
            
            // Create a method call for struct field assignment
            ASTNode *node = ast_create_node(AST_METHOD_CALL);
            node->data.method_call.object = name_str;
            node->data.method_call.method = strdup("__set_field__");
            node->data.method_call.args = malloc(sizeof(ASTNode *) * 2);
            node->data.method_call.args[0] = ast_create_node(AST_STRING_LITERAL);
            node->data.method_call.args[0]->data.string_literal.value = strdup(field->value);
            node->data.method_call.args[1] = value;
            node->data.method_call.arg_count = 2;
            
            token_destroy(field);
            return node;
        } else {
            // Struct field access
            ASTNode *node = ast_create_node(AST_STRUCT_ACCESS);
            node->data.struct_access.struct_name = name_str;
            node->data.struct_access.field_name = strdup(field->value);
            
            token_destroy(field);
            return node;
        }
    } else if (parser->current->type == TOKEN_LPAREN) {
        // Function call
        return parse_func_call(parser, name_str);
    }
    
    // Just an identifier expression
    ASTNode *node = ast_create_node(AST_IDENTIFIER);
    node->data.identifier.name = name_str;
    return node;
}

ASTNode *parse_func_call(Parser *parser, char *name) {
    parser_advance(parser); // consume '('
    
    ASTNode *node = ast_create_node(AST_FUNCTION_CALL);
    node->data.func_call.name = strdup(name);
    node->data.func_call.args = NULL;
    node->data.func_call.arg_count = 0;
    
    free(name);
    
    // Parse arguments
    while (parser->current->type != TOKEN_RPAREN &&
           parser->current->type != TOKEN_EOF) {
        ASTNode *arg = parse_expression(parser);
        if (arg) {
            node->data.func_call.arg_count++;
            node->data.func_call.args = realloc(
                node->data.func_call.args,
                sizeof(ASTNode *) * node->data.func_call.arg_count
            );
            node->data.func_call.args[node->data.func_call.arg_count - 1] = arg;
        }
        
        if (parser->current->type == TOKEN_COMMA) {
            parser_advance(parser);
        }
    }
    
    parser_consume(parser, TOKEN_RPAREN, "Expected ')' after arguments");
    
    return node;
}

ASTNode *parse_expression(Parser *parser) {
    return parse_comparison(parser);
}

static ASTNode *parse_comparison(Parser *parser) {
    ASTNode *left = parse_term(parser);
    
    while (parser->current->type == TOKEN_EQUAL ||
           parser->current->type == TOKEN_NOT_EQUAL ||
           parser->current->type == TOKEN_LESS ||
           parser->current->type == TOKEN_GREATER ||
           parser->current->type == TOKEN_LESS_EQUAL ||
           parser->current->type == TOKEN_GREATER_EQUAL) {
        
        TokenType op = parser->current->type;
        parser_advance(parser);
        
        ASTNode *right = parse_term(parser);
        
        ASTNode *node = ast_create_node(AST_BINARY_OP);
        node->data.binary_op.left = left;
        node->data.binary_op.right = right;
        node->data.binary_op.op = op;
        
        left = node;
    }
    
    return left;
}

static ASTNode *parse_term(Parser *parser) {
    ASTNode *left = parse_factor(parser);
    
    while (parser->current->type == TOKEN_PLUS ||
           parser->current->type == TOKEN_MINUS) {
        
        TokenType op = parser->current->type;
        parser_advance(parser);
        
        ASTNode *right = parse_factor(parser);
        
        ASTNode *node = ast_create_node(AST_BINARY_OP);
        node->data.binary_op.left = left;
        node->data.binary_op.right = right;
        node->data.binary_op.op = op;
        
        left = node;
    }
    
    return left;
}

static ASTNode *parse_factor(Parser *parser) {
    ASTNode *left = parse_unary(parser);
    
    while (parser->current->type == TOKEN_MULTIPLY ||
           parser->current->type == TOKEN_DIVIDE ||
           parser->current->type == TOKEN_MODULO) {
        
        TokenType op = parser->current->type;
        parser_advance(parser);
        
        ASTNode *right = parse_unary(parser);
        
        ASTNode *node = ast_create_node(AST_BINARY_OP);
        node->data.binary_op.left = left;
        node->data.binary_op.right = right;
        node->data.binary_op.op = op;
        
        left = node;
    }
    
    return left;
}

static ASTNode *parse_unary(Parser *parser) {
    if (parser->current->type == TOKEN_MINUS) {
        parser_advance(parser);
        
        ASTNode *operand = parse_unary(parser);
        
        ASTNode *node = ast_create_node(AST_UNARY_OP);
        node->data.unary_op.operand = operand;
        node->data.unary_op.op = TOKEN_MINUS;
        
        return node;
    }
    
    return parse_primary(parser);
}

ASTNode *parse_primary(Parser *parser) {
    switch (parser->current->type) {
        case TOKEN_NUMBER: {
            Token *token = parser->current;
            parser_advance(parser);
            
            ASTNode *node = ast_create_node(AST_NUMBER_LITERAL);
            node->data.number_literal.value = atof(token->value);
            
            return node;
        }
        
        case TOKEN_STRING: {
            Token *token = parser->current;
            parser_advance(parser);
            
            // Check for string interpolation
            if (strchr(token->value, '{')) {
                return parse_interpolated_string(parser, token->value);
            }
            
            ASTNode *node = ast_create_node(AST_STRING_LITERAL);
            node->data.string_literal.value = strdup(token->value);
            
            return node;
        }
        
        case TOKEN_TRUE: {
            parser_advance(parser);
            ASTNode *node = ast_create_node(AST_BOOL_LITERAL);
            node->data.bool_literal.value = true;
            return node;
        }
        
        case TOKEN_FALSE: {
            parser_advance(parser);
            ASTNode *node = ast_create_node(AST_BOOL_LITERAL);
            node->data.bool_literal.value = false;
            return node;
        }
        
        case TOKEN_NULL: {
            parser_advance(parser);
            ASTNode *node = ast_create_node(AST_NUMBER_LITERAL);
            node->data.number_literal.value = 0;
            return node;
        }
        
        case TOKEN_IDENTIFIER: {
            Token *token = parser->current;
            char *name = strdup(token->value);
            parser_advance(parser);
            
            if (parser->current->type == TOKEN_LPAREN) {
                return parse_func_call(parser, name);
            } else if (parser->current->type == TOKEN_DOT) {
                parser_advance(parser);
                Token *field = parser_consume(parser, TOKEN_IDENTIFIER, "Expected field name");
                
                ASTNode *node = ast_create_node(AST_STRUCT_ACCESS);
                node->data.struct_access.struct_name = name;
                node->data.struct_access.field_name = strdup(field->value);
                
                token_destroy(field);
                return node;
            }
            
            ASTNode *node = ast_create_node(AST_IDENTIFIER);
            node->data.identifier.name = name;
            return node;
        }
        
        case TOKEN_LPAREN: {
            parser_advance(parser);
            ASTNode *expr = parse_expression(parser);
            parser_consume(parser, TOKEN_RPAREN, "Expected ')' after expression");
            return expr;
        }
        
        default:
            fprintf(stderr, "Unexpected token in expression: %s\n",
                    token_type_to_string(parser->current->type));
            return NULL;
    }
}

ASTNode *parse_interpolated_string(Parser *parser, char *start) {
    // Simple implementation - just return as regular string for now
    ASTNode *node = ast_create_node(AST_STRING_LITERAL);
    node->data.string_literal.value = strdup(start);
    return node;
}

ASTNode *parse_ai_stmt(Parser *parser) {
    // Simplified AI statement parsing
    // This would need to be expanded based on specific AI syntax
    parser_advance(parser);
    
    // For now, just skip until end of statement
    while (parser->current->type != TOKEN_NEWLINE &&
           parser->current->type != TOKEN_EOF) {
        parser_advance(parser);
    }
    
    return ast_create_node(AST_BLOCK); // Return empty block as placeholder
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("Program (%d statements)\n", node->data.block.count);
            for (int i = 0; i < node->data.block.count; i++) {
                ast_print(node->data.block.statements[i], indent + 1);
            }
            break;
            
        case AST_VAR_DECL:
            printf("VarDecl: %s\n", node->data.var_decl.name);
            ast_print(node->data.var_decl.value, indent + 1);
            break;
            
        case AST_CONST_DECL:
            printf("ConstDecl: %s\n", node->data.const_decl.name);
            ast_print(node->data.const_decl.value, indent + 1);
            break;
            
        case AST_ASSIGNMENT:
            printf("Assignment: %s\n", node->data.assignment.name);
            ast_print(node->data.assignment.value, indent + 1);
            break;
            
        case AST_NUMBER_LITERAL:
            printf("Number: %f\n", node->data.number_literal.value);
            break;
            
        case AST_STRING_LITERAL:
            printf("String: \"%s\"\n", node->data.string_literal.value);
            break;
            
        case AST_BOOL_LITERAL:
            printf("Bool: %s\n", node->data.bool_literal.value ? "true" : "false");
            break;
            
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier.name);
            break;
            
        case AST_BINARY_OP:
            printf("BinaryOp: %s\n", token_type_to_string(node->data.binary_op.op));
            ast_print(node->data.binary_op.left, indent + 1);
            ast_print(node->data.binary_op.right, indent + 1);
            break;
            
        case AST_WRITE:
            printf("Write:\n");
            ast_print(node->data.write_stmt.value, indent + 1);
            break;
            
        case AST_IF:
            printf("If:\n");
            ast_print(node->data.if_stmt.condition, indent + 1);
            break;
            
        case AST_FOR:
            printf("For: %s\n", node->data.for_loop.var_name);
            break;
            
        case AST_FUNCTION_DEF:
            printf("Function: %s (%d params)\n", 
                   node->data.func_def.name, node->data.func_def.param_count);
            break;
            
        case AST_FUNCTION_CALL:
            printf("FunctionCall: %s (%d args)\n",
                   node->data.func_call.name, node->data.func_call.arg_count);
            break;
            
        case AST_STRUCT_DEF:
            printf("Struct: %s (%d fields)\n",
                   node->data.struct_def.name, node->data.struct_def.field_count);
            break;
            
        case AST_IMPORT:
            printf("Import: %s\n", node->data.import.module_name);
            break;
            
        default:
            printf("Unknown node type: %d\n", node->type);
            break;
    }
}
