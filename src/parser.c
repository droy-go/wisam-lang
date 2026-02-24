#include "wisam.h"
#include <stdlib.h>
#include <string.h>

Parser *parser_create(Token *tokens, int token_count) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->tokens = tokens;
    parser->position = 0;
    parser->count = token_count;
    
    return parser;
}

void parser_destroy(Parser *parser) {
    free(parser);
}

static Token parser_peek(Parser *parser) {
    if (parser->position >= parser->count) {
        return parser->tokens[parser->count - 1];
    }
    return parser->tokens[parser->position];
}

static Token parser_advance(Parser *parser) {
    if (parser->position >= parser->count) {
        return parser->tokens[parser->count - 1];
    }
    return parser->tokens[parser->position++];
}

static bool parser_check(Parser *parser, TokenType type) {
    return parser_peek(parser).type == type;
}

static bool parser_match(Parser *parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

static Token parser_consume(Parser *parser, TokenType type, const char *message) {
    if (parser_check(parser, type)) {
        return parser_advance(parser);
    }
    Token current = parser_peek(parser);
    fprintf(stderr, "خطأ نحوي عند السطر %d: %s\n", current.line, message);
    exit(1);
}

static void skip_newlines(Parser *parser) {
    while (parser_match(parser, TOKEN_NEWLINE));
}

// Forward declarations
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_primary(Parser *parser);

static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    return node;
}

static ASTNode *parse_print(Parser *parser) {
    ASTNode *node = create_node(AST_PRINT);
    skip_newlines(parser);
    node->as.print.expression = parse_expression(parser);
    return node;
}

static ASTNode *parse_let(Parser *parser) {
    ASTNode *node = create_node(AST_LET);
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم المتغير بعد 'ليكن'");
    node->as.let.name = strdup(name.value);
    
    parser_consume(parser, TOKEN_ASSIGN, "متوقع '=' بعد اسم المتغير");
    skip_newlines(parser);
    
    node->as.let.value = parse_expression(parser);
    return node;
}

static ASTNode *parse_const(Parser *parser) {
    ASTNode *node = create_node(AST_CONST);
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الثابت بعد 'ثابت'");
    node->as.constant.name = strdup(name.value);
    
    parser_consume(parser, TOKEN_ASSIGN, "متوقع '=' بعد اسم الثابت");
    skip_newlines(parser);
    
    node->as.constant.value = parse_expression(parser);
    return node;
}

static ASTNode *parse_if(Parser *parser) {
    ASTNode *node = create_node(AST_IF);
    
    node->as.if_stmt.condition = parse_expression(parser);
    parser_consume(parser, TOKEN_THEN, "متوقع 'إذن' بعد الشرط");
    skip_newlines(parser);
    
    // جمع الأوامر في فرع then
    ASTNode **then_stmts = malloc(sizeof(ASTNode*) * 100);
    int then_count = 0;
    
    while (!parser_check(parser, TOKEN_ELSE) && !parser_check(parser, TOKEN_END) && 
           !parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (parser_check(parser, TOKEN_ELSE) || parser_check(parser, TOKEN_END)) break;
        then_stmts[then_count++] = parse_statement(parser);
        skip_newlines(parser);
    }
    
    // إنشاء عقدة برنامج فرعية لـ then
    ASTNode *then_node = create_node(AST_PROGRAM);
    then_node->as.program.statements = then_stmts;
    then_node->as.program.count = then_count;
    node->as.if_stmt.then_branch = then_node;
    
    // فرع else
    if (parser_match(parser, TOKEN_ELSE)) {
        skip_newlines(parser);
        ASTNode **else_stmts = malloc(sizeof(ASTNode*) * 100);
        int else_count = 0;
        
        while (!parser_check(parser, TOKEN_END) && !parser_check(parser, TOKEN_EOF)) {
            skip_newlines(parser);
            if (parser_check(parser, TOKEN_END)) break;
            else_stmts[else_count++] = parse_statement(parser);
            skip_newlines(parser);
        }
        
        ASTNode *else_node = create_node(AST_PROGRAM);
        else_node->as.program.statements = else_stmts;
        else_node->as.program.count = else_count;
        node->as.if_stmt.else_branch = else_node;
    } else {
        node->as.if_stmt.else_branch = NULL;
    }
    
    parser_consume(parser, TOKEN_END, "متوقع 'انتهى' في نهاية جملة 'إذا'");
    return node;
}

static ASTNode *parse_for(Parser *parser) {
    ASTNode *node = create_node(AST_FOR);
    
    Token var = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم المتغير بعد 'لكل'");
    node->as.for_loop.var_name = strdup(var.value);
    
    parser_consume(parser, TOKEN_FROM, "متوقع 'من' بعد اسم المتغير");
    node->as.for_loop.start = parse_expression(parser);
    
    parser_consume(parser, TOKEN_TO, "متوقع 'إلى' بعد قيمة البداية");
    node->as.for_loop.end = parse_expression(parser);
    skip_newlines(parser);
    
    // جمع الأوامر
    ASTNode **body_stmts = malloc(sizeof(ASTNode*) * 100);
    int body_count = 0;
    
    while (!parser_check(parser, TOKEN_END) && !parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (parser_check(parser, TOKEN_END)) break;
        body_stmts[body_count++] = parse_statement(parser);
        skip_newlines(parser);
    }
    
    ASTNode *body_node = create_node(AST_PROGRAM);
    body_node->as.program.statements = body_stmts;
    body_node->as.program.count = body_count;
    node->as.for_loop.body = body_node;
    
    parser_consume(parser, TOKEN_END, "متوقع 'انتهى' في نهاية حلقة 'لكل'");
    return node;
}

static ASTNode *parse_function_def(Parser *parser) {
    ASTNode *node = create_node(AST_FUNCTION_DEF);
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الدالة بعد 'دالة'");
    node->as.function_def.name = strdup(name.value);
    
    // قراءة المعاملات
    node->as.function_def.params = malloc(sizeof(char*) * 20);
    node->as.function_def.param_count = 0;
    
    // تخطي "تأخذ" إذا وجدت
    if (parser_check(parser, TOKEN_IDENTIFIER) && 
        strcmp(parser_peek(parser).value, "تأخذ") == 0) {
        parser_advance(parser);
    }
    
    // قراءة المعاملات
    while (parser_check(parser, TOKEN_IDENTIFIER)) {
        Token param = parser_advance(parser);
        node->as.function_def.params[node->as.function_def.param_count++] = strdup(param.value);
        
        if (parser_check(parser, TOKEN_AND) || parser_check(parser, TOKEN_COMMA)) {
            parser_advance(parser);
        }
    }
    
    skip_newlines(parser);
    
    // جمع الأوامر
    ASTNode **body_stmts = malloc(sizeof(ASTNode*) * 100);
    int body_count = 0;
    
    while (!parser_check(parser, TOKEN_END) && !parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (parser_check(parser, TOKEN_END)) break;
        body_stmts[body_count++] = parse_statement(parser);
        skip_newlines(parser);
    }
    
    ASTNode *body_node = create_node(AST_PROGRAM);
    body_node->as.program.statements = body_stmts;
    body_node->as.program.count = body_count;
    node->as.function_def.body = body_node;
    
    parser_consume(parser, TOKEN_END, "متوقع 'انتهى' في نهاية تعريف الدالة");
    return node;
}

static ASTNode *parse_return(Parser *parser) {
    ASTNode *node = create_node(AST_RETURN);
    skip_newlines(parser);
    node->as.return_stmt.value = parse_expression(parser);
    return node;
}

static ASTNode *parse_struct_def(Parser *parser) {
    ASTNode *node = create_node(AST_STRUCT_DEF);
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الهيكل بعد 'هيكل'");
    node->as.struct_def.name = strdup(name.value);
    
    node->as.struct_def.fields = malloc(sizeof(char*) * 20);
    node->as.struct_def.field_count = 0;
    skip_newlines(parser);
    
    // قراءة الحقول
    while (!parser_check(parser, TOKEN_END) && !parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (parser_check(parser, TOKEN_END)) break;
        
        Token field = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم حقل الهيكل");
        node->as.struct_def.fields[node->as.struct_def.field_count++] = strdup(field.value);
        skip_newlines(parser);
    }
    
    parser_consume(parser, TOKEN_END, "متوقع 'انتهى' في نهاية تعريف الهيكل");
    return node;
}

static ASTNode *parse_import(Parser *parser) {
    ASTNode *node = create_node(AST_IMPORT);
    
    Token module = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم المكتبة بعد 'استورد'");
    node->as.import.module_name = strdup(module.value);
    
    return node;
}

static ASTNode *parse_create(Parser *parser) {
    // "أنشئ عقل باسم مساعد" أو "أنشئ منظومة باسم كيان"
    Token type = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع نوع الكائن بعد 'أنشئ'");
    
    if (strcmp(type.value, "عقل") == 0) {
        parser_consume(parser, TOKEN_AS, "متوقع 'باسم' بعد 'عقل'");
        Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم العقل");
        
        ASTNode *node = create_node(AST_MIND_CREATE);
        node->as.mind_create.name = strdup(name.value);
        return node;
    } else if (strcmp(type.value, "منظومة") == 0) {
        parser_consume(parser, TOKEN_AS, "متوقع 'باسم' بعد 'منظومة'");
        Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم المنظومة");
        
        ASTNode *node = create_node(AST_SYSTEM_CREATE);
        node->as.system_create.name = strdup(name.value);
        return node;
    } else if (strcmp(type.value, "شبكة") == 0) {
        // "أنشئ شبكة عصبية باسم شبكة_عميقة"
        Token neural_type = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع 'عصبية' بعد 'شبكة'");
        parser_consume(parser, TOKEN_AS, "متوقع 'باسم'");
        Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الشبكة");
        
        ASTNode *node = create_node(AST_NEURAL_CREATE);
        node->as.neural_create.name = strdup(name.value);
        node->as.neural_create.layers = 3;  // قيمة افتراضية
        node->as.neural_create.learning_rate = 0.01;
        return node;
    }
    
    fprintf(stderr, "خطأ: نوع غير معروف للإنشاء: %s\n", type.value);
    exit(1);
}

static ASTNode *parse_function_call(Parser *parser, char *name) {
    ASTNode *node = create_node(AST_FUNCTION_CALL);
    node->as.function_call.name = strdup(name);
    node->as.function_call.args = malloc(sizeof(ASTNode*) * 20);
    node->as.function_call.arg_count = 0;
    
    parser_consume(parser, TOKEN_LPAREN, "متوقع '(' بعد اسم الدالة");
    
    // قراءة المعاملات
    while (!parser_check(parser, TOKEN_RPAREN) && !parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        node->as.function_call.args[node->as.function_call.arg_count++] = parse_expression(parser);
        skip_newlines(parser);
        
        if (parser_check(parser, TOKEN_COMMA) || parser_check(parser, TOKEN_AND)) {
            parser_advance(parser);
        }
    }
    
    parser_consume(parser, TOKEN_RPAREN, "متوقع ')' في نهاية استدعاء الدالة");
    return node;
}

static ASTNode *parse_primary(Parser *parser) {
    Token token = parser_peek(parser);
    
    switch (token.type) {
        case TOKEN_NUMBER: {
            parser_advance(parser);
            ASTNode *node = create_node(AST_LITERAL);
            node->as.literal.value = value_create_number(atof(token.value));
            return node;
        }
        case TOKEN_STRING: {
            parser_advance(parser);
            ASTNode *node = create_node(AST_LITERAL);
            node->as.literal.value = value_create_string(token.value);
            return node;
        }
        case TOKEN_IDENTIFIER: {
            parser_advance(parser);
            
            // التحقق من وجود استدعاء دالة
            if (parser_check(parser, TOKEN_LPAREN)) {
                return parse_function_call(parser, token.value);
            }
            
            // التحقق من الوصول إلى حقل كائن
            if (parser_check(parser, TOKEN_DOT)) {
                parser_advance(parser);
                Token field = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الحقل بعد '.'");
                
                ASTNode *node = create_node(AST_STRUCT_ACCESS);
                node->as.struct_access.object = create_node(AST_IDENTIFIER);
                node->as.struct_access.object->as.identifier.name = strdup(token.value);
                node->as.struct_access.field = strdup(field.value);
                return node;
            }
            
            ASTNode *node = create_node(AST_IDENTIFIER);
            node->as.identifier.name = strdup(token.value);
            return node;
        }
        case TOKEN_LPAREN: {
            parser_advance(parser);
            ASTNode *expr = parse_expression(parser);
            parser_consume(parser, TOKEN_RPAREN, "متوقع ')'");
            return expr;
        }
        default:
            fprintf(stderr, "خطأ نحوي عند السطر %d: تعبير غير متوقع '%s'\n", 
                    token.line, token.value);
            exit(1);
    }
}

static ASTNode *parse_unary(Parser *parser) {
    if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_NOT)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_UNARY_OP);
        node->as.unary_op.op = op;
        node->as.unary_op.operand = parse_unary(parser);
        return node;
    }
    return parse_primary(parser);
}

static ASTNode *parse_power(Parser *parser) {
    ASTNode *left = parse_unary(parser);
    
    while (parser_match(parser, TOKEN_POWER)) {
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = TOKEN_POWER;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_unary(parser);
        left = node;
    }
    
    return left;
}

static ASTNode *parse_term(Parser *parser) {
    ASTNode *left = parse_power(parser);
    
    while (parser_match(parser, TOKEN_MULTIPLY) || parser_match(parser, TOKEN_DIVIDE) || 
           parser_match(parser, TOKEN_MODULO)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_power(parser);
        left = node;
    }
    
    return left;
}

static ASTNode *parse_expression_arithmetic(Parser *parser) {
    ASTNode *left = parse_term(parser);
    
    while (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_term(parser);
        left = node;
    }
    
    return left;
}

static ASTNode *parse_comparison(Parser *parser) {
    ASTNode *left = parse_expression_arithmetic(parser);
    
    while (parser_match(parser, TOKEN_GREATER) || parser_match(parser, TOKEN_LESS) ||
           parser_match(parser, TOKEN_GREATER_EQ) || parser_match(parser, TOKEN_LESS_EQ) ||
           parser_match(parser, TOKEN_EQUAL) || parser_match(parser, TOKEN_NOT_EQUAL)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_expression_arithmetic(parser);
        left = node;
    }
    
    return left;
}

static ASTNode *parse_expression(Parser *parser) {
    return parse_comparison(parser);
}

static ASTNode *parse_statement(Parser *parser) {
    skip_newlines(parser);
    Token token = parser_peek(parser);
    
    switch (token.type) {
        case TOKEN_PRINT:
            parser_advance(parser);
            return parse_print(parser);
        case TOKEN_LET:
            parser_advance(parser);
            return parse_let(parser);
        case TOKEN_CONST:
            parser_advance(parser);
            return parse_const(parser);
        case TOKEN_IF:
            parser_advance(parser);
            return parse_if(parser);
        case TOKEN_FOR:
            parser_advance(parser);
            return parse_for(parser);
        case TOKEN_FUNCTION:
            parser_advance(parser);
            return parse_function_def(parser);
        case TOKEN_RETURN:
            parser_advance(parser);
            return parse_return(parser);
        case TOKEN_STRUCT:
            parser_advance(parser);
            return parse_struct_def(parser);
        case TOKEN_IMPORT:
            parser_advance(parser);
            return parse_import(parser);
        case TOKEN_CREATE:
            parser_advance(parser);
            return parse_create(parser);
        case TOKEN_IDENTIFIER: {
            // قد يكون إسناد أو استدعاء دالة
            Token next = parser->tokens[parser->position + 1];
            if (next.type == TOKEN_ASSIGN) {
                // إسناد
                ASTNode *node = create_node(AST_ASSIGN);
                node->as.assign.name = strdup(token.value);
                parser_advance(parser); // تخطي المعرف
                parser_advance(parser); // تخطي =
                node->as.assign.value = parse_expression(parser);
                return node;
            } else if (next.type == TOKEN_LPAREN) {
                // استدعاء دالة
                parser_advance(parser);
                return parse_function_call(parser, token.value);
            }
            // تعبير عادي
            return parse_expression(parser);
        }
        default:
            return parse_expression(parser);
    }
}

ASTNode *parser_parse(Parser *parser) {
    ASTNode *program = create_node(AST_PROGRAM);
    program->as.program.statements = malloc(sizeof(ASTNode*) * 1000);
    program->as.program.count = 0;
    
    while (!parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (parser_check(parser, TOKEN_EOF)) break;
        
        program->as.program.statements[program->as.program.count++] = parse_statement(parser);
        skip_newlines(parser);
    }
    
    return program;
}
