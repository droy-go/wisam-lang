/*
 * ============================================
 * Wisam Programming Language - Test Suite
 * لغة برمجة وسام - مجموعة الاختبارات
 * ============================================
 * 
 * هذا الملف يحتوي على اختبارات شاملة للغة وسام
 * This file contains comprehensive tests for Wisam language
 */

#include "../include/wisam.h"
#include <assert.h>
#include <string.h>

/* ============================================
 * Test Framework
 * إطار الاختبار
 * ============================================ */

static int tests_run = 0;
static int tests_passed = 0;
static int tests_failed = 0;
static char current_test[256];

#define TEST(name) void test_##name()
#define RUN_TEST(name) do { \
    strcpy(current_test, #name); \
    tests_run++; \
    printf("  🧪 Testing: %s\n", #name); \
    test_##name(); \
    tests_passed++; \
    printf("  ✅ PASSED: %s\n\n", #name); \
} while(0)

#define ASSERT(condition) do { \
    if (!(condition)) { \
        tests_failed++; \
        tests_passed--; \
        printf("  ❌ FAILED: %s\n", current_test); \
        printf("     Assertion failed: %s\n", #condition); \
        printf("     at %s:%d\n", __FILE__, __LINE__); \
        return; \
    } \
} while(0)

#define ASSERT_EQ(a, b) ASSERT((a) == (b))
#define ASSERT_NE(a, b) ASSERT((a) != (b))
#define ASSERT_TRUE(x) ASSERT((x))
#define ASSERT_FALSE(x) ASSERT(!(x))
#define ASSERT_NULL(x) ASSERT((x) == NULL)
#define ASSERT_NOT_NULL(x) ASSERT((x) != NULL)

/* ============================================
 * Lexer Tests
 * اختبارات المحلل اللغوي
 * ============================================ */

TEST(lexer_create_destroy) {
    Lexer *lexer = lexer_create("ليكن س = 5", "test.wsm");
    ASSERT_NOT_NULL(lexer);
    lexer_destroy(lexer);
}

TEST(lexer_tokenize_let) {
    Lexer *lexer = lexer_create("ليكن س = 5", "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    ASSERT_NOT_NULL(tokens);
    ASSERT(token_count >= 4); // LET, IDENTIFIER, ASSIGN, NUMBER, EOF
    ASSERT_EQ(tokens[0].type, TOKEN_LET);
    
    free(tokens);
    lexer_destroy(lexer);
}

TEST(lexer_tokenize_if) {
    Lexer *lexer = lexer_create("إذا س > 5 إذن اكتب "مرحبا" انتهى", "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    ASSERT_NOT_NULL(tokens);
    ASSERT_EQ(tokens[0].type, TOKEN_IF);
    ASSERT_EQ(tokens[2].type, TOKEN_GREATER);
    ASSERT_EQ(tokens[4].type, TOKEN_THEN);
    
    free(tokens);
    lexer_destroy(lexer);
}

TEST(lexer_tokenize_string) {
    Lexer *lexer = lexer_create("\"مرحبا بالعالم\"", "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    ASSERT_NOT_NULL(tokens);
    ASSERT_EQ(tokens[0].type, TOKEN_STRING);
    ASSERT(strcmp(tokens[0].value, "مرحبا بالعالم") == 0);
    
    free(tokens);
    lexer_destroy(lexer);
}

TEST(lexer_tokenize_number) {
    Lexer *lexer = lexer_create("123 45.67 1e10", "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    ASSERT_NOT_NULL(tokens);
    ASSERT_EQ(tokens[0].type, TOKEN_NUMBER);
    ASSERT_EQ(tokens[2].type, TOKEN_NUMBER);
    
    free(tokens);
    lexer_destroy(lexer);
}

TEST(lexer_tokenize_comments) {
    Lexer *lexer = lexer_create("# هذا تعليق\nليكن س = 5", "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    ASSERT_NOT_NULL(tokens);
    ASSERT_EQ(tokens[0].type, TOKEN_LET);
    
    free(tokens);
    lexer_destroy(lexer);
}

TEST(lexer_arabic_keywords) {
    const char *code = "ليكن ثابت إذا وإلا انتهى لكل من إلى دالة أعد";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    ASSERT_NOT_NULL(tokens);
    ASSERT_EQ(tokens[0].type, TOKEN_LET);
    ASSERT_EQ(tokens[1].type, TOKEN_CONST);
    ASSERT_EQ(tokens[2].type, TOKEN_IF);
    ASSERT_EQ(tokens[3].type, TOKEN_ELSE);
    ASSERT_EQ(tokens[4].type, TOKEN_END);
    ASSERT_EQ(tokens[5].type, TOKEN_FOR);
    ASSERT_EQ(tokens[6].type, TOKEN_FROM);
    ASSERT_EQ(tokens[7].type, TOKEN_TO);
    ASSERT_EQ(tokens[8].type, TOKEN_FUNCTION);
    ASSERT_EQ(tokens[9].type, TOKEN_RETURN);
    
    free(tokens);
    lexer_destroy(lexer);
}

/* ============================================
 * Parser Tests
 * اختبارات المحلل النحوي
 * ============================================ */

TEST(parser_create_destroy) {
    Token tokens[] = {
        {TOKEN_LET, "ليكن", 1, 1, NULL},
        {TOKEN_EOF, "", 1, 1, NULL}
    };
    
    Parser *parser = parser_create(tokens, 2);
    ASSERT_NOT_NULL(parser);
    parser_destroy(parser);
}

TEST(parser_parse_let) {
    const char *code = "ليكن س = 5";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    ASSERT_NOT_NULL(ast);
    ASSERT_EQ(ast->type, AST_PROGRAM);
    ASSERT(ast->as.program.count >= 1);
    ASSERT_EQ(ast->as.program.statements[0]->type, AST_LET);
    
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(parser_parse_if) {
    const char *code = "إذا س > 5 إذن اكتب "مرحبا" انتهى";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    ASSERT_NOT_NULL(ast);
    ASSERT_EQ(ast->as.program.statements[0]->type, AST_IF);
    
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(parser_parse_for) {
    const char *code = "لكل رقم من 1 إلى 10\n    اكتب رقم\nانتهى";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    ASSERT_NOT_NULL(ast);
    ASSERT_EQ(ast->as.program.statements[0]->type, AST_FOR);
    
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(parser_parse_function) {
    const char *code = "دالة جمع تأخذ أ ب\n    أعد أ + ب\nانتهى";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    ASSERT_NOT_NULL(ast);
    ASSERT_EQ(ast->as.program.statements[0]->type, AST_FUNCTION_DEF);
    ASSERT(strcmp(ast->as.program.statements[0]->as.function_def.name, "جمع") == 0);
    
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(parser_parse_array) {
    const char *code = "ليكن أرقام = [1، 2، 3]";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    ASSERT_NOT_NULL(ast);
    ASSERT_EQ(ast->as.program.statements[0]->type, AST_LET);
    
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

/* ============================================
 * Interpreter Tests
 * اختبارات المفسر
 * ============================================ */

TEST(interpreter_create_destroy) {
    Interpreter *interp = interpreter_create();
    ASSERT_NOT_NULL(interp);
    interpreter_destroy(interp);
}

TEST(interpreter_number_literal) {
    const char *code = "ليكن س = 42";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *val = interpreter_get_variable(interp, "س");
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(val->type, VAL_NUMBER);
    ASSERT_EQ(val->as.number, 42);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_string_literal) {
    const char *code = "ليكن نص = "مرحبا"";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *val = interpreter_get_variable(interp, "نص");
    ASSERT_NOT_NULL(val);
    ASSERT_EQ(val->type, VAL_STRING);
    ASSERT(strcmp(val->as.string, "مرحبا") == 0);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_arithmetic) {
    const char *code = 
        "ليكن أ = 10\n"
        "ليكن ب = 5\n"
        "ليكن جمع = أ + ب\n"
        "ليكن طرح = أ - ب\n"
        "ليكن ضرب = أ * ب\n"
        "ليكن قسمة = أ / ب";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *جمع = interpreter_get_variable(interp, "جمع");
    Value *طرح = interpreter_get_variable(interp, "طرح");
    Value *ضرب = interpreter_get_variable(interp, "ضرب");
    Value *قسمة = interpreter_get_variable(interp, "قسمة");
    
    ASSERT_NOT_NULL(جمع);
    ASSERT_NOT_NULL(طرح);
    ASSERT_NOT_NULL(ضرب);
    ASSERT_NOT_NULL(قسمة);
    
    ASSERT_EQ(جمع->as.number, 15);
    ASSERT_EQ(طرح->as.number, 5);
    ASSERT_EQ(ضرب->as.number, 50);
    ASSERT_EQ(قسمة->as.number, 2);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_comparison) {
    const char *code = 
        "ليكن أكبر = 10 > 5\n"
        "ليكن أصغر = 5 < 10\n"
        "ليكن يساوي = 5 == 5\n"
        "ليكن لا_يساوي = 5 != 10";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *أكبر = interpreter_get_variable(interp, "أكبر");
    Value *أصغر = interpreter_get_variable(interp, "أصغر");
    Value *يساوي = interpreter_get_variable(interp, "يساوي");
    Value *لا_يساوي = interpreter_get_variable(interp, "لا_يساوي");
    
    ASSERT_TRUE(أكبر->as.boolean);
    ASSERT_TRUE(أصغر->as.boolean);
    ASSERT_TRUE(يساوي->as.boolean);
    ASSERT_TRUE(لا_يساوي->as.boolean);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_if_statement) {
    const char *code = 
        "ليكن س = 10\n"
        "إذا س > 5 إذن\n"
        "    ليكن ناتج = "كبير"\n"
        "انتهى";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *ناتج = interpreter_get_variable(interp, "ناتج");
    ASSERT_NOT_NULL(ناتج);
    ASSERT(strcmp(ناتج->as.string, "كبير") == 0);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_for_loop) {
    const char *code = 
        "ليكن مجموع = 0\n"
        "لكل رقم من 1 إلى 5\n"
        "    مجموع = مجموع + رقم\n"
        "انتهى";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *مجموع = interpreter_get_variable(interp, "مجموع");
    ASSERT_NOT_NULL(مجموع);
    ASSERT_EQ(مجموع->as.number, 15); // 1+2+3+4+5 = 15
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_function) {
    const char *code = 
        "دالة جمع تأخذ أ ب\n"
        "    أعد أ + ب\n"
        "انتهى\n"
        "ليكن ناتج = جمع 10 20";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *ناتج = interpreter_get_variable(interp, "ناتج");
    ASSERT_NOT_NULL(ناتج);
    ASSERT_EQ(ناتج->as.number, 30);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(interpreter_array) {
    const char *code = "ليكن أرقام = [1، 2، 3، 4، 5]";
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *أرقام = interpreter_get_variable(interp, "أرقام");
    ASSERT_NOT_NULL(أرقام);
    ASSERT_EQ(أرقام->type, VAL_ARRAY);
    ASSERT_EQ(أرقام->as.array.count, 5);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

/* ============================================
 * Value Tests
 * اختبارات القيم
 * ============================================ */

TEST(value_create_number) {
    Value val = value_create_number(42.5);
    ASSERT_EQ(val.type, VAL_NUMBER);
    ASSERT_EQ(val.as.number, 42.5);
}

TEST(value_create_string) {
    Value val = value_create_string("مرحبا");
    ASSERT_EQ(val.type, VAL_STRING);
    ASSERT(strcmp(val.as.string, "مرحبا") == 0);
    free(val.as.string);
}

TEST(value_create_boolean) {
    Value val = value_create_boolean(true);
    ASSERT_EQ(val.type, VAL_BOOLEAN);
    ASSERT_TRUE(val.as.boolean);
}

TEST(value_create_null) {
    Value val = value_create_null();
    ASSERT_EQ(val.type, VAL_NULL);
}

TEST(value_create_array) {
    Value val = value_create_array();
    ASSERT_EQ(val.type, VAL_ARRAY);
    ASSERT_EQ(val.as.array.count, 0);
    ASSERT_NOT_NULL(val.as.array.items);
    free(val.as.array.items);
}

TEST(value_is_truthy) {
    Value null_val = value_create_null();
    Value false_val = value_create_boolean(false);
    Value true_val = value_create_boolean(true);
    Value zero = value_create_number(0);
    Value non_zero = value_create_number(5);
    Value empty_str = value_create_string("");
    Value non_empty_str = value_create_string("hello");
    
    ASSERT_FALSE(value_is_truthy(&null_val));
    ASSERT_FALSE(value_is_truthy(&false_val));
    ASSERT_TRUE(value_is_truthy(&true_val));
    ASSERT_FALSE(value_is_truthy(&zero));
    ASSERT_TRUE(value_is_truthy(&non_zero));
    ASSERT_FALSE(value_is_truthy(&empty_str));
    ASSERT_TRUE(value_is_truthy(&non_empty_str));
    
    free(empty_str.as.string);
    free(non_empty_str.as.string);
}

TEST(value_equals) {
    Value a = value_create_number(5);
    Value b = value_create_number(5);
    Value c = value_create_number(10);
    
    ASSERT_TRUE(value_equals(&a, &b));
    ASSERT_FALSE(value_equals(&a, &c));
}

/* ============================================
 * Environment Tests
 * اختبارات البيئة
 * ============================================ */

TEST(environment_create_destroy) {
    Environment *env = environment_create(NULL, "test");
    ASSERT_NOT_NULL(env);
    environment_destroy(env);
}

TEST(environment_define_get) {
    Environment *env = environment_create(NULL, "test");
    Value val = value_create_number(42);
    
    environment_define(env, "س", val, false);
    
    Value *retrieved = environment_get(env, "س");
    ASSERT_NOT_NULL(retrieved);
    ASSERT_EQ(retrieved->as.number, 42);
    
    environment_destroy(env);
}

TEST(environment_set) {
    Environment *env = environment_create(NULL, "test");
    Value val1 = value_create_number(10);
    Value val2 = value_create_number(20);
    
    environment_define(env, "س", val1, false);
    environment_set(env, "س", val2);
    
    Value *retrieved = environment_get(env, "س");
    ASSERT_EQ(retrieved->as.number, 20);
    
    environment_destroy(env);
}

TEST(environment_constant) {
    Environment *env = environment_create(NULL, "test");
    Value val = value_create_number(10);
    
    environment_define(env, "PI", val, true);
    
    ASSERT_TRUE(environment_is_constant(env, "PI"));
    ASSERT_FALSE(environment_is_constant(env, "غير_موجود"));
    
    environment_destroy(env);
}

TEST(environment_nested) {
    Environment *parent = environment_create(NULL, "parent");
    Environment *child = environment_create(parent, "child");
    
    Value val = value_create_number(100);
    environment_define(parent, "س", val, false);
    
    Value *from_child = environment_get(child, "س");
    ASSERT_NOT_NULL(from_child);
    ASSERT_EQ(from_child->as.number, 100);
    
    environment_destroy(child);
    environment_destroy(parent);
}

/* ============================================
 * Library Tests
 * اختبارات المكتبات
 * ============================================ */

TEST(lib_text_upper) {
    Value arg = value_create_string("hello");
    Value result = lib_text_upper(&arg, 1);
    
    ASSERT(strcmp(result.as.string, "HELLO") == 0);
    
    free(arg.as.string);
    free(result.as.string);
}

TEST(lib_text_lower) {
    Value arg = value_create_string("HELLO");
    Value result = lib_text_lower(&arg, 1);
    
    ASSERT(strcmp(result.as.string, "hello") == 0);
    
    free(arg.as.string);
    free(result.as.string);
}

TEST(lib_text_length) {
    Value arg = value_create_string("مرحبا");
    Value result = lib_text_length(&arg, 1);
    
    ASSERT_EQ(result.as.number, 5);
    
    free(arg.as.string);
}

TEST(lib_math_abs) {
    Value arg = value_create_number(-10);
    Value result = lib_math_abs(&arg, 1);
    
    ASSERT_EQ(result.as.number, 10);
}

TEST(lib_math_sqrt) {
    Value arg = value_create_number(16);
    Value result = lib_math_sqrt(&arg, 1);
    
    ASSERT_EQ(result.as.number, 4);
}

TEST(lib_math_pow) {
    Value args[] = {
        value_create_number(2),
        value_create_number(3)
    };
    Value result = lib_math_pow(args, 2);
    
    ASSERT_EQ(result.as.number, 8);
}

/* ============================================
 * Integration Tests
 * اختبارات التكامل
 * ============================================ */

TEST(integration_hello_world) {
    const char *code = "اكتب \"مرحبا بالعالم\"";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    // Should not crash
    ASSERT_TRUE(1);
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(integration_fibonacci) {
    const char *code = 
        "دالة فيبوناتشي تأخذ ن\n"
        "    إذا ن <= 1 إذن\n"
        "        أعد ن\n"
        "    انتهى\n"
        "    أعد فيبوناتشي(ن - 1) + فيبوناتشي(ن - 2)\n"
        "انتهى\n"
        "ليكن ناتج = فيبوناتشي 10";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *ناتج = interpreter_get_variable(interp, "ناتج");
    ASSERT_NOT_NULL(ناتج);
    ASSERT_EQ(ناتج->as.number, 55); // F(10) = 55
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

TEST(integration_factorial) {
    const char *code = 
        "دالة مضروب تأخذ ن\n"
        "    إذا ن <= 1 إذن\n"
        "        أعد 1\n"
        "    انتهى\n"
        "    أعد ن * مضروب(ن - 1)\n"
        "انتهى\n"
        "ليكن ناتج = مضروب 5";
    
    Lexer *lexer = lexer_create(code, "test.wsm");
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    
    Value *ناتج = interpreter_get_variable(interp, "ناتج");
    ASSERT_NOT_NULL(ناتج);
    ASSERT_EQ(ناتج->as.number, 120); // 5! = 120
    
    interpreter_destroy(interp);
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
}

/* ============================================
 * Main Test Runner
 * المنفذ الرئيسي للاختبارات
 * ============================================ */

void print_header(const char *title) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║  %s\n", title);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
}

void print_summary() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                    نتائج الاختبارات                          ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  إجمالي الاختبارات: %-4d                                     ║\n", tests_run);
    printf("║  ✅ نجح: %-4d                                                ║\n", tests_passed);
    printf("║  ❌ فشل: %-4d                                                ║\n", tests_failed);
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    if (tests_failed == 0) {
        printf("\n🎉 جميع الاختبارات نجحت!\n\n");
    } else {
        printf("\n⚠️ بعض الاختبارات فشلت.\n\n");
    }
}

int main() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║                                                              ║\n");
    printf("║           🧪 مجموعة اختبارات لغة وسام 🧪                    ║\n");
    printf("║              Wisam Test Suite v2.0                          ║\n");
    printf("║                                                              ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    
    /* Lexer Tests */
    print_header("📋 اختبارات المحلل اللغوي (Lexer Tests)");
    RUN_TEST(lexer_create_destroy);
    RUN_TEST(lexer_tokenize_let);
    RUN_TEST(lexer_tokenize_if);
    RUN_TEST(lexer_tokenize_string);
    RUN_TEST(lexer_tokenize_number);
    RUN_TEST(lexer_tokenize_comments);
    RUN_TEST(lexer_arabic_keywords);
    
    /* Parser Tests */
    print_header("📋 اختبارات المحلل النحوي (Parser Tests)");
    RUN_TEST(parser_create_destroy);
    RUN_TEST(parser_parse_let);
    RUN_TEST(parser_parse_if);
    RUN_TEST(parser_parse_for);
    RUN_TEST(parser_parse_function);
    RUN_TEST(parser_parse_array);
    
    /* Interpreter Tests */
    print_header("📋 اختبارات المفسر (Interpreter Tests)");
    RUN_TEST(interpreter_create_destroy);
    RUN_TEST(interpreter_number_literal);
    RUN_TEST(interpreter_string_literal);
    RUN_TEST(interpreter_arithmetic);
    RUN_TEST(interpreter_comparison);
    RUN_TEST(interpreter_if_statement);
    RUN_TEST(interpreter_for_loop);
    RUN_TEST(interpreter_function);
    RUN_TEST(interpreter_array);
    
    /* Value Tests */
    print_header("📋 اختبارات القيم (Value Tests)");
    RUN_TEST(value_create_number);
    RUN_TEST(value_create_string);
    RUN_TEST(value_create_boolean);
    RUN_TEST(value_create_null);
    RUN_TEST(value_create_array);
    RUN_TEST(value_is_truthy);
    RUN_TEST(value_equals);
    
    /* Environment Tests */
    print_header("📋 اختبارات البيئة (Environment Tests)");
    RUN_TEST(environment_create_destroy);
    RUN_TEST(environment_define_get);
    RUN_TEST(environment_set);
    RUN_TEST(environment_constant);
    RUN_TEST(environment_nested);
    
    /* Library Tests */
    print_header("📋 اختبارات المكتبات (Library Tests)");
    RUN_TEST(lib_text_upper);
    RUN_TEST(lib_text_lower);
    RUN_TEST(lib_text_length);
    RUN_TEST(lib_math_abs);
    RUN_TEST(lib_math_sqrt);
    RUN_TEST(lib_math_pow);
    
    /* Integration Tests */
    print_header("📋 اختبارات التكامل (Integration Tests)");
    RUN_TEST(integration_hello_world);
    RUN_TEST(integration_fibonacci);
    RUN_TEST(integration_factorial);
    
    /* Print Summary */
    print_summary();
    
    return tests_failed > 0 ? 1 : 0;
}
