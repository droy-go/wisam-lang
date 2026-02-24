#ifndef WISAM_H
#define WISAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define WISAM_VERSION "1.0"
#define MAX_TOKEN_LENGTH 256
#define MAX_VARIABLES 1000
#define MAX_FUNCTIONS 100
#define MAX_STRUCTS 50
#define MAX_ARRAY_SIZE 1000

// أنواع الرموز (Token Types)
typedef enum {
    // كلمات مفتاحية
    TOKEN_LET,          // ليكن
    TOKEN_CONST,        // ثابت
    TOKEN_IF,           // إذا
    TOKEN_THEN,         // إذن
    TOKEN_ELSE,         // وإلا
    TOKEN_END,          // انتهى
    TOKEN_FOR,          // لكل
    TOKEN_FROM,         // من
    TOKEN_TO,           // إلى
    TOKEN_FUNCTION,     // دالة
    TOKEN_RETURN,       // أعد
    TOKEN_STRUCT,       // هيكل
    TOKEN_IMPORT,       // استورد
    TOKEN_CREATE,       // أنشئ
    TOKEN_AS,           // باسم
    TOKEN_WHILE,        // طالما
    TOKEN_BREAK,        // توقف
    TOKEN_CONTINUE,     // استمر
    TOKEN_AND,          // و
    TOKEN_OR,           // أو
    TOKEN_NOT,          // ليس
    
    // أنواع البيانات
    TOKEN_NUMBER,       // رقم
    TOKEN_STRING,       // نص
    TOKEN_BOOLEAN,      // منطقي
    TOKEN_NULL,         // فارغ
    
    // المعرفات والرموز
    TOKEN_IDENTIFIER,   // معرف
    TOKEN_ASSIGN,       // =
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_MODULO,       // %
    TOKEN_POWER,        // ^
    TOKEN_EQUAL,        // ==
    TOKEN_NOT_EQUAL,    // !=
    TOKEN_GREATER,      // >
    TOKEN_LESS,         // <
    TOKEN_GREATER_EQ,   // >=
    TOKEN_LESS_EQ,      // <=
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_LBRACKET,     // [
    TOKEN_RBRACKET,     // ]
    TOKEN_COMMA,        // ،
    TOKEN_DOT,          // .
    TOKEN_COLON,        // :
    TOKEN_SEMICOLON,    // ؛
    TOKEN_NEWLINE,      // سطر جديد
    TOKEN_EOF,          // نهاية الملف
    TOKEN_PRINT,        // اكتب
    TOKEN_INPUT,        // ادخل
    
    // كلمات مفتاحية إضافية للذكاء الاصطناعي
    TOKEN_MIND,         // عقل
    TOKEN_SYSTEM,       // منظومة
    TOKEN_LEARN,        // تعلّم
    TOKEN_ASK,          // اسأل
    TOKEN_RESPONSE,     // استجب
    TOKEN_SAVE,         // احفظ
    TOKEN_LOAD,         // حمّل
    TOKEN_GPU,          // GPU
    TOKEN_TRAIN,        // درّب
    TOKEN_NEURAL,       // شبكة_عصبية
    TOKEN_LAYERS,       // طبقات
    TOKEN_LEARNING_RATE // معدل_تعلم
} TokenType;

// هيكل الرمز (Token)
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
    int line;
    int column;
} Token;

// أنواع القيم
typedef enum {
    VAL_NUMBER,
    VAL_STRING,
    VAL_BOOLEAN,
    VAL_NULL,
    VAL_ARRAY,
    VAL_OBJECT,
    VAL_FUNCTION,
    VAL_STRUCT_DEF,
    VAL_MIND,       // عقل ذكاء اصطناعي
    VAL_SYSTEM,     // منظومة ذكاء
    VAL_NEURAL      // شبكة عصبية
} ValueType;

// هيكل القيمة (Value)
struct Value;
typedef struct Value Value;

struct Value {
    ValueType type;
    union {
        double number;
        char *string;
        bool boolean;
        struct {
            Value **items;
            int count;
            int capacity;
        } array;
        struct {
            char **keys;
            Value **values;
            int count;
            int capacity;
        } object;
        struct {
            char *name;
            char **params;
            int param_count;
            struct ASTNode *body;
        } function;
        struct {
            char *name;
            struct Value **fields;
            int field_count;
        } struct_def;
        struct {
            char *name;
            struct Value **memories;
            int memory_count;
        } mind;
        struct {
            char *name;
            struct Value **components;
            int component_count;
        } system;
        struct {
            char *name;
            int layers;
            double learning_rate;
            bool use_gpu;
        } neural;
    } as;
};

// عقدة شجرة النحو (AST Node)
typedef enum {
    AST_PROGRAM,
    AST_LET,
    AST_CONST,
    AST_ASSIGN,
    AST_IF,
    AST_FOR,
    AST_WHILE,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_RETURN,
    AST_STRUCT_DEF,
    AST_STRUCT_ACCESS,
    AST_STRUCT_CREATE,
    AST_IMPORT,
    AST_PRINT,
    AST_INPUT,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_LITERAL,
    AST_IDENTIFIER,
    AST_ARRAY,
    AST_ARRAY_ACCESS,
    AST_MIND_CREATE,
    AST_MIND_LEARN,
    AST_MIND_ASK,
    AST_SYSTEM_CREATE,
    AST_NEURAL_CREATE,
    AST_NEURAL_TRAIN,
    AST_SAVE,
    AST_LOAD
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        struct {
            struct ASTNode **statements;
            int count;
        } program;
        struct {
            char *name;
            struct ASTNode *value;
        } let;
        struct {
            char *name;
            struct ASTNode *value;
        } constant;
        struct {
            char *name;
            struct ASTNode *value;
        } assign;
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;
        struct {
            char *var_name;
            struct ASTNode *start;
            struct ASTNode *end;
            struct ASTNode *body;
        } for_loop;
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_loop;
        struct {
            char *name;
            char **params;
            int param_count;
            struct ASTNode *body;
        } function_def;
        struct {
            char *name;
            struct ASTNode **args;
            int arg_count;
        } function_call;
        struct {
            struct ASTNode *value;
        } return_stmt;
        struct {
            char *name;
            char **fields;
            int field_count;
        } struct_def;
        struct {
            struct ASTNode *object;
            char *field;
        } struct_access;
        struct {
            char *struct_name;
            char *var_name;
        } struct_create;
        struct {
            char *module_name;
        } import;
        struct {
            struct ASTNode *expression;
        } print;
        struct {
            char *prompt;
        } input;
        struct {
            TokenType op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_op;
        struct {
            TokenType op;
            struct ASTNode *operand;
        } unary_op;
        struct {
            Value value;
        } literal;
        struct {
            char *name;
        } identifier;
        struct {
            struct ASTNode **elements;
            int count;
        } array;
        struct {
            struct ASTNode *array;
            struct ASTNode *index;
        } array_access;
        struct {
            char *name;
        } mind_create;
        struct {
            char *name;
            struct ASTNode **pairs;
            int pair_count;
        } mind_learn;
        struct {
            char *name;
            struct ASTNode *question;
        } mind_ask;
        struct {
            char *name;
        } system_create;
        struct {
            char *name;
            int layers;
            double learning_rate;
        } neural_create;
        struct {
            char *name;
            char *data_file;
        } neural_train;
        struct {
            char *filename;
        } save;
        struct {
            char *filename;
        } load;
    } as;
} ASTNode;

// المتغيرات
typedef struct {
    char *name;
    Value value;
    bool is_constant;
} Variable;

// البيئة (Environment)
typedef struct Environment {
    Variable variables[MAX_VARIABLES];
    int var_count;
    struct Environment *parent;
} Environment;

// الليكسر
typedef struct {
    char *source;
    int position;
    int line;
    int column;
    int length;
} Lexer;

// البارسر
typedef struct {
    Token *tokens;
    int position;
    int count;
} Parser;

// المفسر
typedef struct {
    Environment *global_env;
    Environment *current_env;
} Interpreter;

// دوال الليكسر
Lexer *lexer_create(const char *source);
void lexer_destroy(Lexer *lexer);
Token *lexer_tokenize(Lexer *lexer, int *token_count);

// دوال البارسر
Parser *parser_create(Token *tokens, int token_count);
void parser_destroy(Parser *parser);
ASTNode *parser_parse(Parser *parser);

// دوال المفسر
Interpreter *interpreter_create(void);
void interpreter_destroy(Interpreter *interpreter);
Value interpreter_evaluate(Interpreter *interpreter, ASTNode *node);
void interpreter_run(Interpreter *interpreter, ASTNode *program);

// دوال القيم
Value value_create_number(double num);
Value value_create_string(const char *str);
Value value_create_boolean(bool boolean);
Value value_create_null(void);
Value value_create_array(void);
Value value_create_object(void);
void value_free(Value *value);
char *value_to_string(Value *value);

// دوال البيئة
Environment *environment_create(Environment *parent);
void environment_destroy(Environment *env);
void environment_define(Environment *env, const char *name, Value value, bool is_constant);
Value *environment_get(Environment *env, const char *name);
void environment_set(Environment *env, const char *name, Value value);
bool environment_exists(Environment *env, const char *name);

// مكتبة النصوص
Value lib_text_upper(Value *args, int arg_count);
Value lib_text_lower(Value *args, int arg_count);
Value lib_text_length(Value *args, int arg_count);
Value lib_text_substring(Value *args, int arg_count);
Value lib_text_replace(Value *args, int arg_count);
Value lib_text_split(Value *args, int arg_count);
Value lib_text_trim(Value *args, int arg_count);
Value lib_text_translate(Value *args, int arg_count);

// مكتبة الزمن
Value lib_time_now(Value *args, int arg_count);
Value lib_time_format(Value *args, int arg_count);
Value lib_time_sleep(Value *args, int arg_count);

// مكتبة المخزن
void lib_store_init(void);
Value lib_store_create(Value *args, int arg_count);
Value lib_store_add(Value *args, int arg_count);
Value lib_store_read(Value *args, int arg_count);
Value lib_store_delete(Value *args, int arg_count);
Value lib_store_list(Value *args, int arg_count);

// مكتبة الذكاء
void lib_ai_init(void);
Value lib_mind_create(Value *args, int arg_count);
Value lib_mind_learn(Value *args, int arg_count);
Value lib_mind_ask(Value *args, int arg_count);
Value lib_system_create(Value *args, int arg_count);
Value lib_system_add(Value *args, int arg_count);
Value lib_system_respond(Value *args, int arg_count);
Value lib_neural_create(Value *args, int arg_count);
Value lib_neural_train(Value *args, int arg_count);
Value lib_neural_use_gpu(Value *args, int arg_count);

// مكتبة الوسائط
Value lib_media_show_image(Value *args, int arg_count);
Value lib_media_play_video(Value *args, int arg_count);
Value lib_media_play_audio(Value *args, int arg_count);

// مكتبة الواجهات
Value lib_gui_create_window(Value *args, int arg_count);
Value lib_gui_add_button(Value *args, int arg_count);
Value lib_gui_add_label(Value *args, int arg_count);
Value lib_gui_add_input(Value *args, int arg_count);

// مكتبة الشبكة
Value lib_net_http_get(Value *args, int arg_count);
Value lib_net_http_post(Value *args, int arg_count);
Value lib_net_create_server(Value *args, int arg_count);

// معلومات الميتا
Value lib_meta_program_name(Value *args, int arg_count);
Value lib_meta_os(Value *args, int arg_count);
Value lib_meta_memory_usage(Value *args, int arg_count);

// دوال مساعدة
char *utf8_strdup(const char *src);
int utf8_strlen(const char *str);
char *utf8_substring(const char *str, int start, int length);
bool is_arabic_letter(char c);
bool is_arabic_digit(char c);

#endif // WISAM_H
