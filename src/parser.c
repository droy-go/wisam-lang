#include "wisam.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// إنشاء البارسر
Parser *parser_create(Token *tokens, int token_count) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->tokens = tokens;
    parser->position = 0;
    parser->count = token_count;
    parser->error_message = NULL;
    parser->error_line = 0;
    parser->error_column = 0;
    
    return parser;
}

// تدمير البارسر
void parser_destroy(Parser *parser) {
    if (parser) {
        free(parser->error_message);
        free(parser);
    }
}

// الحصول على رسالة الخطأ
char *parser_get_error(Parser *parser) {
    return parser ? parser->error_message : NULL;
}

// الحصول على رقم السطر للخطأ
int parser_get_error_line(Parser *parser) {
    return parser ? parser->error_line : 0;
}

// الحصول على رقم العمود للخطأ
int parser_get_error_column(Parser *parser) {
    return parser ? parser->error_column : 0;
}

// تعيين رسالة خطأ
static void set_error(Parser *parser, const char *message) {
    if (parser->error_message) free(parser->error_message);
    parser->error_message = strdup(message);
    parser->error_line = parser->tokens[parser->position].line;
    parser->error_column = parser->tokens[parser->position].column;
}

// النظر إلى الرمز الحالي
static Token parser_peek(Parser *parser) {
    if (parser->position >= parser->count) {
        return parser->tokens[parser->count - 1];
    }
    return parser->tokens[parser->position];
}

// النظر إلى الرمز التالي
static Token parser_peek_next(Parser *parser) {
    if (parser->position + 1 >= parser->count) {
        return parser->tokens[parser->count - 1];
    }
    return parser->tokens[parser->position + 1];
}

// التقدم إلى الرمز التالي
static Token parser_advance(Parser *parser) {
    if (parser->position >= parser->count) {
        return parser->tokens[parser->count - 1];
    }
    return parser->tokens[parser->position++];
}

// التحقق من نوع الرمز الحالي
static bool parser_check(Parser *parser, TokenType type) {
    return parser_peek(parser).type == type;
}

// مطابقة نوع الرمز والتقدم
static bool parser_match(Parser *parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

// استهلاك رمز متوقع
static Token parser_consume(Parser *parser, TokenType type, const char *message) {
    if (parser_check(parser, type)) {
        return parser_advance(parser);
    }
    set_error(parser, message);
    Token error_token = {TOKEN_EOF, "", 0, 0, NULL};
    return error_token;
}

// تخطي أسطر جديدة
static void skip_newlines(Parser *parser) {
    while (parser_match(parser, TOKEN_NEWLINE));
}

// إنشاء عقدة جديدة
static ASTNode *create_node(ASTNodeType type) {
    ASTNode *node = calloc(1, sizeof(ASTNode));
    node->type = type;
    return node;
}

// التصريحات المسبقة
static ASTNode *parse_statement(Parser *parser);
static ASTNode *parse_expression(Parser *parser);
static ASTNode *parse_primary(Parser *parser);
static ASTNode *parse_unary(Parser *parser);
static ASTNode *parse_multiplicative(Parser *parser);
static ASTNode *parse_additive(Parser *parser);
static ASTNode *parse_comparison(Parser *parser);
static ASTNode *parse_equality(Parser *parser);
static ASTNode *parse_logical_and(Parser *parser);
static ASTNode *parse_logical_or(Parser *parser);

// تحليل جملة اكتب
static ASTNode *parse_print(Parser *parser) {
    ASTNode *node = create_node(AST_PRINT);
    skip_newlines(parser);
    node->as.print.expression = parse_expression(parser);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    return node;
}

// تحليل جملة ادخل
static ASTNode *parse_input(Parser *parser) {
    ASTNode *node = create_node(AST_INPUT);
    skip_newlines(parser);
    
    if (parser_check(parser, TOKEN_STRING)) {
        Token prompt = parser_advance(parser);
        node->as.input.prompt = strdup(prompt.value);
    } else {
        node->as.input.prompt = NULL;
    }
    
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    return node;
}

// تحليل تعريف متغير
static ASTNode *parse_let(Parser *parser) {
    ASTNode *node = create_node(AST_LET);
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم المتغير بعد 'ليكن'");
    node->as.let.name = strdup(name.value);
    node->line = name.line;
    node->column = name.column;
    
    parser_consume(parser, TOKEN_ASSIGN, "متوقع '=' بعد اسم المتغير");
    skip_newlines(parser);
    
    node->as.let.value = parse_expression(parser);
    return node;
}

// تحليل تعريف ثابت
static ASTNode *parse_const(Parser *parser) {
    ASTNode *node = create_node(AST_CONST);
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الثابت بعد 'ثابت'");
    node->as.constant.name = strdup(name.value);
    node->line = name.line;
    node->column = name.column;
    
    parser_consume(parser, TOKEN_ASSIGN, "متوقع '=' بعد اسم الثابت");
    skip_newlines(parser);
    
    node->as.constant.value = parse_expression(parser);
    return node;
}

// تحليل جملة إذا
static ASTNode *parse_if(Parser *parser) {
    ASTNode *node = create_node(AST_IF);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
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

// تحليل حلقة لكل
static ASTNode *parse_for(Parser *parser) {
    ASTNode *node = create_node(AST_FOR);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
    Token var = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم المتغير بعد 'لكل'");
    node->as.for_loop.var_name = strdup(var.value);
    
    parser_consume(parser, TOKEN_FROM, "متوقع 'من' بعد اسم المتغير");
    node->as.for_loop.start = parse_expression(parser);
    
    parser_consume(parser, TOKEN_TO, "متوقع 'إلى' بعد قيمة البداية");
    node->as.for_loop.end = parse_expression(parser);
    
    // معالجة الخطوة الاختيارية
    if (parser_match(parser, TOKEN_IDENTIFIER)) {
        // قد تكون "خطوة" - نتخطاها للآن
        node->as.for_loop.step = parse_expression(parser);
    } else {
        node->as.for_loop.step = NULL;
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
    node->as.for_loop.body = body_node;
    
    parser_consume(parser, TOKEN_END, "متوقع 'انتهى' في نهاية حلقة 'لكل'");
    return node;
}

// تحليل حلقة طالما
static ASTNode *parse_while(Parser *parser) {
    ASTNode *node = create_node(AST_WHILE);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
    node->as.while_loop.condition = parse_expression(parser);
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
    node->as.while_loop.body = body_node;
    
    parser_consume(parser, TOKEN_END, "متوقع 'انتهى' في نهاية حلقة 'طالما'");
    return node;
}

// تحليل تعريف دالة
static ASTNode *parse_function_def(Parser *parser) {
    ASTNode *node = create_node(AST_FUNCTION_DEF);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
    Token name = parser_consume(parser, TOKEN_IDENTIFIER, "متوقع اسم الدالة بعد 'دالة'");
    node->as.function_def.name = strdup(name.value);
    
    // قراءة المعاملات
    node->as.function_def.params = malloc(sizeof(char*) * 20);
    node->as.function_def.param_count = 0;
    node->as.function_def.is_async = false;
    node->as.function_def.is_static = false;
    node->as.function_def.return_type = NULL;
    
    // تخطي "تأخذ" إذا وجدت
    if (parser_check(parser, TOKEN_IDENTIFIER) && 
        strcmp(parser_peek(parser).value, "تأخذ") == 0) {
        parser_advance(parser);
    }
    
    // قراءة المعاملات
    while (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_EOF) &&
           parser_check(parser, TOKEN_IDENTIFIER)) {
        Token param = parser_advance(parser);
        node->as.function_def.params[node->as.function_def.param_count++] = strdup(param.value);
        
        // تخطي الفاصلة إذا وجدت
        if (parser_check(parser, TOKEN_COMMA) || parser_check(parser, TOKEN_IDENTIFIER)) {
            parser_advance(parser);
        }
    }
    
    skip_newlines(parser);
    
    // جمع جسم الدالة
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

// تحليل استدعاء دالة
static ASTNode *parse_function_call(Parser *parser, const char *name) {
    ASTNode *node = create_node(AST_FUNCTION_CALL);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
    node->as.function_call.name = strdup(name);
    node->as.function_call.args = malloc(sizeof(ASTNode*) * 20);
    node->as.function_call.arg_count = 0;
    node->as.function_call.is_method = false;
    node->as.function_call.object = NULL;
    
    // قراءة المعاملات
    while (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_EOF) &&
           !parser_check(parser, TOKEN_END)) {
        node->as.function_call.args[node->as.function_call.arg_count++] = parse_expression(parser);
        
        // تخطي الفاصلة إذا وجدت
        if (parser_check(parser, TOKEN_COMMA) || parser_check(parser, TOKEN_IDENTIFIER)) {
            parser_advance(parser);
        } else {
            break;
        }
    }
    
    return node;
}

// تحليل المصفوفة
static ASTNode *parse_array(Parser *parser) {
    ASTNode *node = create_node(AST_ARRAY);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
    parser_consume(parser, TOKEN_LBRACKET, "متوقع '['");
    
    node->as.array.elements = malloc(sizeof(ASTNode*) * 100);
    node->as.array.count = 0;
    
    while (!parser_check(parser, TOKEN_RBRACKET) && !parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        node->as.array.elements[node->as.array.count++] = parse_expression(parser);
        skip_newlines(parser);
        
        if (parser_match(parser, TOKEN_COMMA)) {
            continue;
        } else {
            break;
        }
    }
    
    parser_consume(parser, TOKEN_RBRACKET, "متوقع ']' في نهاية المصفوفة");
    return node;
}

// تحليل الوصول للمصفوفة
static ASTNode *parse_array_access(Parser *parser, const char *name) {
    ASTNode *node = create_node(AST_ARRAY_ACCESS);
    node->line = parser->tokens[parser->position].line;
    node->column = parser->tokens[parser->position].column;
    
    ASTNode *id_node = create_node(AST_IDENTIFIER);
    id_node->as.identifier.name = strdup(name);
    node->as.array_access.array = id_node;
    
    parser_consume(parser, TOKEN_LBRACKET, "متوقع '['");
    node->as.array_access.index = parse_expression(parser);
    parser_consume(parser, TOKEN_RBRACKET, "متوقع ']'");
    
    return node;
}

// تحليل التعبير الأساسي
static ASTNode *parse_primary(Parser *parser) {
    skip_newlines(parser);
    
    Token token = parser_peek(parser);
    
    switch (token.type) {
        case TOKEN_NUMBER:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_LITERAL);
                node->as.literal.value = value_create_number(atof(token.value));
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_STRING:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_LITERAL);
                node->as.literal.value = value_create_string(token.value);
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_TRUE:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_LITERAL);
                node->as.literal.value = value_create_boolean(true);
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_FALSE:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_LITERAL);
                node->as.literal.value = value_create_boolean(false);
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_NULL:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_LITERAL);
                node->as.literal.value = value_create_null();
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_IDENTIFIER:
            parser_advance(parser);
            // التحقق من استدعاء دالة أو الوصول للمصفوفة
            if (parser_check(parser, TOKEN_LBRACKET)) {
                return parse_array_access(parser, token.value);
            }
            // يمكن إضافة المزيد من الحالات هنا
            {
                ASTNode *node = create_node(AST_IDENTIFIER);
                node->as.identifier.name = strdup(token.value);
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_LPAREN:
            parser_advance(parser);
            {
                ASTNode *expr = parse_expression(parser);
                parser_consume(parser, TOKEN_RPAREN, "متوقع ')'");
                return expr;
            }
            
        case TOKEN_LBRACKET:
            return parse_array(parser);
            
        default:
            set_error(parser, "تعبير غير متوقع");
            return create_node(AST_LITERAL);
    }
}

// تحليل التعبير الأحادي
static ASTNode *parse_unary(Parser *parser) {
    if (parser_match(parser, TOKEN_MINUS) || parser_match(parser, TOKEN_NOT)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_UNARY_OP);
        node->as.unary_op.op = op;
        node->as.unary_op.operand = parse_unary(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        return node;
    }
    
    return parse_primary(parser);
}

// تحليل الضرب والقسمة
static ASTNode *parse_multiplicative(Parser *parser) {
    ASTNode *left = parse_unary(parser);
    
    while (parser_match(parser, TOKEN_MULTIPLY) || 
           parser_match(parser, TOKEN_DIVIDE) || 
           parser_match(parser, TOKEN_MODULO)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_unary(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        left = node;
    }
    
    return left;
}

// تحليل الجمع والطرح
static ASTNode *parse_additive(Parser *parser) {
    ASTNode *left = parse_multiplicative(parser);
    
    while (parser_match(parser, TOKEN_PLUS) || parser_match(parser, TOKEN_MINUS)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_multiplicative(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        left = node;
    }
    
    return left;
}

// تحليل المقارنات
static ASTNode *parse_comparison(Parser *parser) {
    ASTNode *left = parse_additive(parser);
    
    while (parser_match(parser, TOKEN_GREATER) || 
           parser_match(parser, TOKEN_LESS) || 
           parser_match(parser, TOKEN_GREATER_EQ) || 
           parser_match(parser, TOKEN_LESS_EQ)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_additive(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        left = node;
    }
    
    return left;
}

// تحليل المساواة
static ASTNode *parse_equality(Parser *parser) {
    ASTNode *left = parse_comparison(parser);
    
    while (parser_match(parser, TOKEN_EQUAL) || parser_match(parser, TOKEN_NOT_EQUAL)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_comparison(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        left = node;
    }
    
    return left;
}

// تحليل AND المنطقي
static ASTNode *parse_logical_and(Parser *parser) {
    ASTNode *left = parse_equality(parser);
    
    while (parser_match(parser, TOKEN_AND)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_equality(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        left = node;
    }
    
    return left;
}

// تحليل OR المنطقي
static ASTNode *parse_logical_or(Parser *parser) {
    ASTNode *left = parse_logical_and(parser);
    
    while (parser_match(parser, TOKEN_OR)) {
        TokenType op = parser->tokens[parser->position - 1].type;
        ASTNode *node = create_node(AST_BINARY_OP);
        node->as.binary_op.op = op;
        node->as.binary_op.left = left;
        node->as.binary_op.right = parse_logical_and(parser);
        node->line = parser->tokens[parser->position].line;
        node->column = parser->tokens[parser->position].column;
        left = node;
    }
    
    return left;
}

// تحليل التعبير
static ASTNode *parse_expression(Parser *parser) {
    return parse_logical_or(parser);
}

// تحليل جملة
static ASTNode *parse_statement(Parser *parser) {
    skip_newlines(parser);
    
    Token token = parser_peek(parser);
    
    switch (token.type) {
        case TOKEN_PRINT:
            parser_advance(parser);
            return parse_print(parser);
            
        case TOKEN_INPUT:
            parser_advance(parser);
            return parse_input(parser);
            
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
            
        case TOKEN_WHILE:
            parser_advance(parser);
            return parse_while(parser);
            
        case TOKEN_FUNCTION:
            parser_advance(parser);
            return parse_function_def(parser);
            
        case TOKEN_RETURN:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_RETURN);
                node->line = token.line;
                node->column = token.column;
                if (!parser_check(parser, TOKEN_NEWLINE) && !parser_check(parser, TOKEN_END)) {
                    node->as.return_stmt.value = parse_expression(parser);
                } else {
                    node->as.return_stmt.value = NULL;
                }
                return node;
            }
            
        case TOKEN_BREAK:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_BREAK);
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_CONTINUE:
            parser_advance(parser);
            {
                ASTNode *node = create_node(AST_CONTINUE);
                node->line = token.line;
                node->column = token.column;
                return node;
            }
            
        case TOKEN_IDENTIFIER:
            // التحقق من وجود تعيين
            {
                Token next = parser_peek_next(parser);
                if (next.type == TOKEN_ASSIGN) {
                    ASTNode *node = create_node(AST_ASSIGN);
                    node->as.assign.name = strdup(token.value);
                    parser_advance(parser); // اسم المتغير
                    parser_advance(parser); // =
                    node->as.assign.value = parse_expression(parser);
                    node->line = token.line;
                    node->column = token.column;
                    return node;
                }
            }
            // استدعاء دالة
            return parse_function_call(parser, token.value);
            
        default:
            return parse_expression(parser);
    }
}

// تحليل البرنامج
ASTNode *parser_parse(Parser *parser) {
    ASTNode *program = create_node(AST_PROGRAM);
    program->line = 1;
    program->column = 1;
    
    program->as.program.statements = malloc(sizeof(ASTNode*) * 10000);
    program->as.program.count = 0;
    
    while (!parser_check(parser, TOKEN_EOF)) {
        skip_newlines(parser);
        if (parser_check(parser, TOKEN_EOF)) break;
        
        ASTNode *stmt = parse_statement(parser);
        if (stmt) {
            program->as.program.statements[program->as.program.count++] = stmt;
        }
        
        skip_newlines(parser);
    }
    
    return program;
}
