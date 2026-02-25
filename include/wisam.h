#ifndef WISAM_H
#define WISAM_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

#define WISAM_VERSION "2.0"
#define WISAM_VERSION_NAME "الإصدار الذهبي"
#define MAX_TOKEN_LENGTH 512
#define MAX_VARIABLES 5000
#define MAX_FUNCTIONS 500
#define MAX_STRUCTS 200
#define MAX_ARRAY_SIZE 10000
#define MAX_CLASSES 100
#define MAX_MODULES 50

// أنواع الرموز (Token Types)
typedef enum {
    // كلمات مفتاحية أساسية
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
    TOKEN_CLASS,        // صنف
    TOKEN_IMPORT,       // استورد
    TOKEN_CREATE,       // أنشئ
    TOKEN_AS,           // باسم
    TOKEN_WHILE,        // طالما
    TOKEN_BREAK,        // توقف
    TOKEN_CONTINUE,     // استمر
    TOKEN_AND,          // و
    TOKEN_OR,           // أو
    TOKEN_NOT,          // ليس
    TOKEN_IN,           // في
    TOKEN_OF,           // من
    TOKEN_TRY,          // حاول
    TOKEN_CATCH,        // امسك
    TOKEN_FINALLY,      // أخيراً
    TOKEN_THROW,        // ألقِ
    TOKEN_SWITCH,       // حسب
    TOKEN_CASE,         // حالة
    TOKEN_DEFAULT,      // افتراضي
    TOKEN_DO,           // نفذ
    TOKEN_UNTIL,        // حتى
    TOKEN_FOREACH,      // لكل_عنصر
    TOKEN_YIELD,        // أنتج
    TOKEN_ASYNC,        // غير_متزامن
    TOKEN_AWAIT,        // انتظر
    TOKEN_PUBLIC,       // عام
    TOKEN_PRIVATE,      // خاص
    TOKEN_PROTECTED,    // محمي
    TOKEN_STATIC,       // ثابت_الصنف
    TOKEN_EXTENDS,      // يرث
    TOKEN_IMPLEMENTS,   // ينفذ
    TOKEN_NEW,          // جديد
    TOKEN_THIS,         // هذا
    TOKEN_SUPER,        // أب
    TOKEN_NULL,         // فارغ
    TOKEN_TRUE,         // صحيح
    TOKEN_FALSE,        // خطأ
    
    // أنواع البيانات
    TOKEN_NUMBER,       // رقم
    TOKEN_STRING,       // نص
    TOKEN_BOOLEAN,      // منطقي
    TOKEN_ARRAY,        // مصفوفة
    TOKEN_OBJECT,       // كائن
    
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
    TOKEN_TYPEOF,       // نوع
    TOKEN_SIZEOF,       // حجم
    
    // كلمات مفتاحية للذكاء الاصطناعي
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
    TOKEN_LEARNING_RATE,// معدل_تعلم
    TOKEN_MODEL,        // نموذج
    TOKEN_PREDICT,      // تنبأ
    TOKEN_DATASET,      // مجموعة_بيانات
    
    // معاملات إضافية
    TOKEN_INCREMENT,    // ++
    TOKEN_DECREMENT,    // --
    TOKEN_PLUS_ASSIGN,  // +=
    TOKEN_MINUS_ASSIGN, // -=
    TOKEN_MUL_ASSIGN,   // *=
    TOKEN_DIV_ASSIGN,   // /=
    TOKEN_MOD_ASSIGN,   // %=
    TOKEN_BIT_AND,      // &
    TOKEN_BIT_OR,       // |
    TOKEN_BIT_XOR,      // ^
    TOKEN_BIT_NOT,      // ~
    TOKEN_SHIFT_LEFT,   // <<
    TOKEN_SHIFT_RIGHT,  // >>
} TokenType;

// هيكل الرمز (Token)
typedef struct {
    TokenType type;
    char value[MAX_TOKEN_LENGTH];
    int line;
    int column;
    char *filename;
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
    VAL_CLASS_DEF,
    VAL_INSTANCE,
    VAL_MIND,
    VAL_SYSTEM,
    VAL_NEURAL,
    VAL_MODULE,
    VAL_PROMISE,
    VAL_ITERATOR,
    VAL_EXCEPTION
} ValueType;

// عقدة شجرة النحو (AST Node) - تعريف مسبق
typedef struct ASTNode ASTNode;

// البيئة (Environment) - تعريف مسبق
typedef struct Environment Environment;

// هيكل الصنف (Class) - تعريف مسبق
typedef struct Class Class;

// هيكل الكائن (Object) - تعريف مسبق
typedef struct Object Object;

// هيكل الوعد (Promise) - تعريف مسبق
typedef struct Promise Promise;

// هيكل القيمة (Value)
typedef struct Value {
    ValueType type;
    union {
        double number;
        char *string;
        bool boolean;
        struct {
            struct Value **items;
            int count;
            int capacity;
        } array;
        struct {
            char **keys;
            struct Value **values;
            int count;
            int capacity;
        } object;
        struct {
            char *name;
            char **params;
            int param_count;
            ASTNode *body;
            Environment *closure;
            bool is_native;
            struct Value (*native_fn)(struct Value *args, int arg_count);
        } function;
        struct {
            char *name;
            char **fields;
            int field_count;
        } struct_def;
        struct {
            Class *class_def;
            Object *instance;
        } instance;
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
            void *model_data;
        } neural;
        struct {
            char *name;
            struct Value **exports;
            int export_count;
        } module;
        struct {
            bool resolved;
            struct Value *value;
            struct Promise *next;
        } promise;
        struct {
            struct Value *collection;
            int index;
        } iterator;
        struct {
            char *message;
            int code;
            char *stack_trace;
        } exception;
    } as;
} Value;

// هيكل الدالة - تعريف مسبق
typedef struct Function Function;

// هيكل الصنف
struct Class {
    char *name;
    char **fields;
    int field_count;
    struct Function **methods;
    int method_count;
    Class *parent;
    bool is_abstract;
};

// هيكل الكائن
struct Object {
    Class *class_def;
    Value **field_values;
    int field_count;
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
    AST_DO_WHILE,
    AST_FOREACH,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_RETURN,
    AST_STRUCT_DEF,
    AST_STRUCT_ACCESS,
    AST_STRUCT_CREATE,
    AST_CLASS_DEF,
    AST_CLASS_CREATE,
    AST_METHOD_CALL,
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
    AST_LOAD,
    AST_TRY_CATCH,
    AST_THROW,
    AST_SWITCH,
    AST_CASE,
    AST_DEFAULT,
    AST_BREAK,
    AST_CONTINUE,
    AST_YIELD,
    AST_AWAIT,
    AST_NEW,
    AST_THIS,
    AST_SUPER,
    AST_TYPEOF,
    AST_SIZEOF,
    AST_TERNARY,
    AST_FOR_IN,
    AST_PROPERTY_ACCESS,
    AST_STATIC_CALL,
    AST_INCREMENT,
    AST_DECREMENT
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    int line;
    int column;
    union {
        struct {
            struct ASTNode **statements;
            int count;
        } program;
        struct {
            char *name;
            struct ASTNode *value;
            bool is_constant;
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
            struct ASTNode *step;
            struct ASTNode *body;
        } for_loop;
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_loop;
        struct {
            struct ASTNode *body;
            struct ASTNode *condition;
        } do_while;
        struct {
            char *var_name;
            struct ASTNode *collection;
            struct ASTNode *body;
        } for_each;
        struct {
            char *name;
            char **params;
            int param_count;
            struct ASTNode *body;
            bool is_async;
            bool is_static;
            char *return_type;
        } function_def;
        struct {
            char *name;
            struct ASTNode **args;
            int arg_count;
            struct ASTNode *object;
            bool is_method;
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
            char *name;
            char **fields;
            int field_count;
            Function **methods;
            int method_count;
            char *parent_name;
            bool is_abstract;
            char **interfaces;
            int interface_count;
        } class_def;
        struct {
            char *class_name;
            struct ASTNode **args;
            int arg_count;
        } class_create;
        struct {
            struct ASTNode *object;
            char *method_name;
            struct ASTNode **args;
            int arg_count;
        } method_call;
        struct {
            char *module_name;
            char **imports;
            int import_count;
            bool is_wildcard;
        } import;
        struct {
            struct ASTNode *expression;
        } print;
        struct {
            char *prompt;
            char *var_type;
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
            bool use_gpu;
        } neural_create;
        struct {
            char *name;
            char *data_file;
            int epochs;
        } neural_train;
        struct {
            char *filename;
        } save;
        struct {
            char *filename;
        } load;
        struct {
            struct ASTNode *try_block;
            struct ASTNode *catch_block;
            char *exception_var;
            struct ASTNode *finally_block;
        } try_catch;
        struct {
            struct ASTNode *exception;
        } throw_stmt;
        struct {
            struct ASTNode *expression;
            struct ASTNode **cases;
            int case_count;
            struct ASTNode *default_case;
        } switch_stmt;
        struct {
            struct ASTNode *value;
            struct ASTNode **statements;
            int statement_count;
        } case_stmt;
        struct {
            struct ASTNode **statements;
            int statement_count;
        } default_stmt;
        struct {
            char *label;
        } break_stmt;
        struct {
            char *label;
        } continue_stmt;
        struct {
            struct ASTNode *value;
        } yield_stmt;
        struct {
            struct ASTNode *promise;
        } await_stmt;
        struct {
            char *class_name;
            struct ASTNode **args;
            int arg_count;
        } new_stmt;
        struct {
            char *property;
        } this_expr;
        struct {
            char *method_name;
            struct ASTNode **args;
            int arg_count;
        } super_call;
        struct {
            struct ASTNode *expression;
        } typeof_expr;
        struct {
            struct ASTNode *expression;
        } sizeof_expr;
        struct {
            struct ASTNode *condition;
            struct ASTNode *true_expr;
            struct ASTNode *false_expr;
        } ternary;
        struct {
            char *var_name;
            char *collection;
            struct ASTNode *body;
        } for_in;
        struct {
            struct ASTNode *object;
            char *property;
        } property_access;
        struct {
            char *class_name;
            char *method_name;
            struct ASTNode **args;
            int arg_count;
        } static_call;
        struct {
            struct ASTNode *operand;
            bool is_prefix;
        } increment;
        struct {
            struct ASTNode *operand;
            bool is_prefix;
        } decrement;
    } as;
} ASTNode;

// المتغيرات
typedef struct {
    char *name;
    Value value;
    bool is_constant;
    char *type_hint;
} Variable;

// البيئة (Environment)
typedef struct Environment {
    Variable variables[MAX_VARIABLES];
    int var_count;
    struct Environment *parent;
    char *name;
    bool is_class_scope;
    bool is_function_scope;
} Environment;

// الليكسر
typedef struct {
    char *source;
    int position;
    int line;
    int column;
    int length;
    char *filename;
    char *error_message;
} Lexer;

// البارسر
typedef struct {
    Token *tokens;
    int position;
    int count;
    char *error_message;
    int error_line;
    int error_column;
} Parser;

// المفسر
typedef struct {
    Environment *global_env;
    Environment *current_env;
    Value *return_value;
    bool is_returning;
    bool is_breaking;
    bool is_continuing;
    Value *exception;
    bool is_try_block;
} Interpreter;

// دوال الليكسر
Lexer *lexer_create(const char *source, const char *filename);
void lexer_destroy(Lexer *lexer);
Token *lexer_tokenize(Lexer *lexer, int *token_count);
char *lexer_get_error(Lexer *lexer);

// دوال البارسر
Parser *parser_create(Token *tokens, int token_count);
void parser_destroy(Parser *parser);
ASTNode *parser_parse(Parser *parser);
char *parser_get_error(Parser *parser);
int parser_get_error_line(Parser *parser);
int parser_get_error_column(Parser *parser);

// دوال المفسر
Interpreter *interpreter_create(void);
void interpreter_destroy(Interpreter *interpreter);
Value interpreter_evaluate(Interpreter *interpreter, ASTNode *node);
void interpreter_run(Interpreter *interpreter, ASTNode *program);
void interpreter_set_variable(Interpreter *interpreter, const char *name, Value value);
Value *interpreter_get_variable(Interpreter *interpreter, const char *name);

// دوال القيم
Value value_create_number(double num);
Value value_create_string(const char *str);
Value value_create_boolean(bool boolean);
Value value_create_null(void);
Value value_create_array(void);
Value value_create_object(void);
Value value_create_function(const char *name, char **params, int param_count, ASTNode *body);
Value value_create_class(const char *name, char **fields, int field_count);
Value value_create_instance(Class *class_def);
Value value_create_exception(const char *message, int code);
void value_free(Value *value);
char *value_to_string(Value *value);
bool value_is_truthy(Value *value);
bool value_equals(Value *a, Value *b);
Value value_copy(Value *value);

// دوال البيئة
Environment *environment_create(Environment *parent, const char *name);
void environment_destroy(Environment *env);
void environment_define(Environment *env, const char *name, Value value, bool is_constant);
Value *environment_get(Environment *env, const char *name);
void environment_set(Environment *env, const char *name, Value value);
bool environment_exists(Environment *env, const char *name);
bool environment_is_constant(Environment *env, const char *name);
void environment_set_type_hint(Environment *env, const char *name, const char *type_hint);
char *environment_get_type_hint(Environment *env, const char *name);

// مكتبة النصوص
Value lib_text_upper(Value *args, int arg_count);
Value lib_text_lower(Value *args, int arg_count);
Value lib_text_length(Value *args, int arg_count);
Value lib_text_substring(Value *args, int arg_count);
Value lib_text_replace(Value *args, int arg_count);
Value lib_text_split(Value *args, int arg_count);
Value lib_text_trim(Value *args, int arg_count);
Value lib_text_translate(Value *args, int arg_count);
Value lib_text_starts_with(Value *args, int arg_count);
Value lib_text_ends_with(Value *args, int arg_count);
Value lib_text_contains(Value *args, int arg_count);
Value lib_text_index_of(Value *args, int arg_count);
Value lib_text_last_index_of(Value *args, int arg_count);
Value lib_text_repeat(Value *args, int arg_count);
Value lib_text_pad_start(Value *args, int arg_count);
Value lib_text_pad_end(Value *args, int arg_count);
Value lib_text_reverse(Value *args, int arg_count);
Value lib_text_format(Value *args, int arg_count);

// مكتبة الزمن
Value lib_time_now(Value *args, int arg_count);
Value lib_time_format(Value *args, int arg_count);
Value lib_time_sleep(Value *args, int arg_count);
Value lib_time_year(Value *args, int arg_count);
Value lib_time_month(Value *args, int arg_count);
Value lib_time_day(Value *args, int arg_count);
Value lib_time_hour(Value *args, int arg_count);
Value lib_time_minute(Value *args, int arg_count);
Value lib_time_second(Value *args, int arg_count);
Value lib_time_timestamp(Value *args, int arg_count);
Value lib_time_parse(Value *args, int arg_count);
Value lib_time_add_days(Value *args, int arg_count);
Value lib_time_diff(Value *args, int arg_count);

// مكتبة الرياضيات
Value lib_math_abs(Value *args, int arg_count);
Value lib_math_sqrt(Value *args, int arg_count);
Value lib_math_pow(Value *args, int arg_count);
Value lib_math_sin(Value *args, int arg_count);
Value lib_math_cos(Value *args, int arg_count);
Value lib_math_tan(Value *args, int arg_count);
Value lib_math_log(Value *args, int arg_count);
Value lib_math_log10(Value *args, int arg_count);
Value lib_math_exp(Value *args, int arg_count);
Value lib_math_floor(Value *args, int arg_count);
Value lib_math_ceil(Value *args, int arg_count);
Value lib_math_round(Value *args, int arg_count);
Value lib_math_min(Value *args, int arg_count);
Value lib_math_max(Value *args, int arg_count);
Value lib_math_random(Value *args, int arg_count);
Value lib_math_pi(Value *args, int arg_count);
Value lib_math_e(Value *args, int arg_count);
Value lib_math_sum(Value *args, int arg_count);
Value lib_math_average(Value *args, int arg_count);
Value lib_math_median(Value *args, int arg_count);
Value lib_math_mode(Value *args, int arg_count);
Value lib_math_stddev(Value *args, int arg_count);
Value lib_math_variance(Value *args, int arg_count);
Value lib_math_factorial(Value *args, int arg_count);
Value lib_math_gcd(Value *args, int arg_count);
Value lib_math_lcm(Value *args, int arg_count);
Value lib_math_is_prime(Value *args, int arg_count);
Value lib_math_fibonacci(Value *args, int arg_count);

// مكتبة القوائم
Value lib_list_create(Value *args, int arg_count);
Value lib_list_push(Value *args, int arg_count);
Value lib_list_pop(Value *args, int arg_count);
Value lib_list_shift(Value *args, int arg_count);
Value lib_list_unshift(Value *args, int arg_count);
Value lib_list_length(Value *args, int arg_count);
Value lib_list_get(Value *args, int arg_count);
Value lib_list_set(Value *args, int arg_count);
Value lib_list_remove(Value *args, int arg_count);
Value lib_list_index_of(Value *args, int arg_count);
Value lib_list_contains(Value *args, int arg_count);
Value lib_list_sort(Value *args, int arg_count);
Value lib_list_reverse(Value *args, int arg_count);
Value lib_list_slice(Value *args, int arg_count);
Value lib_list_splice(Value *args, int arg_count);
Value lib_list_join(Value *args, int arg_count);
Value lib_list_map(Value *args, int arg_count);
Value lib_list_filter(Value *args, int arg_count);
Value lib_list_reduce(Value *args, int arg_count);
Value lib_list_find(Value *args, int arg_count);
Value lib_list_find_index(Value *args, int arg_count);
Value lib_list_every(Value *args, int arg_count);
Value lib_list_some(Value *args, int arg_count);
Value lib_list_flatten(Value *args, int arg_count);
Value lib_list_unique(Value *args, int arg_count);
Value lib_list_chunk(Value *args, int arg_count);

// مكتبة الملفات
Value lib_file_read(Value *args, int arg_count);
Value lib_file_write(Value *args, int arg_count);
Value lib_file_append(Value *args, int arg_count);
Value lib_file_exists(Value *args, int arg_count);
Value lib_file_delete(Value *args, int arg_count);
Value lib_file_copy(Value *args, int arg_count);
Value lib_file_move(Value *args, int arg_count);
Value lib_file_rename(Value *args, int arg_count);
Value lib_file_size(Value *args, int arg_count);
Value lib_file_create_dir(Value *args, int arg_count);
Value lib_file_delete_dir(Value *args, int arg_count);
Value lib_file_list_dir(Value *args, int arg_count);
Value lib_file_is_dir(Value *args, int arg_count);
Value lib_file_is_file(Value *args, int arg_count);
Value lib_file_get_extension(Value *args, int arg_count);
Value lib_file_join_path(Value *args, int arg_count);
Value lib_file_get_absolute_path(Value *args, int arg_count);
Value lib_file_get_relative_path(Value *args, int arg_count);
Value lib_file_read_json(Value *args, int arg_count);
Value lib_file_write_json(Value *args, int arg_count);

// مكتبة JSON
Value lib_json_parse(Value *args, int arg_count);
Value lib_json_stringify(Value *args, int arg_count);
Value lib_json_is_valid(Value *args, int arg_count);
Value lib_json_get(Value *args, int arg_count);
Value lib_json_set(Value *args, int arg_count);
Value lib_json_remove(Value *args, int arg_count);
Value lib_json_keys(Value *args, int arg_count);
Value lib_json_values(Value *args, int arg_count);
Value lib_json_merge(Value *args, int arg_count);

// مكتبة التشفير
Value lib_crypto_md5(Value *args, int arg_count);
Value lib_crypto_sha1(Value *args, int arg_count);
Value lib_crypto_sha256(Value *args, int arg_count);
Value lib_crypto_sha512(Value *args, int arg_count);
Value lib_crypto_base64_encode(Value *args, int arg_count);
Value lib_crypto_base64_decode(Value *args, int arg_count);
Value lib_crypto_uuid(Value *args, int arg_count);
Value lib_crypto_random_bytes(Value *args, int arg_count);
Value lib_crypto_hash(Value *args, int arg_count);
Value lib_crypto_hmac(Value *args, int arg_count);

// مكتبة التعابير النمطية
Value lib_regex_match(Value *args, int arg_count);
Value lib_regex_search(Value *args, int arg_count);
Value lib_regex_replace(Value *args, int arg_count);
Value lib_regex_split(Value *args, int arg_count);
Value lib_regex_test(Value *args, int arg_count);

// مكتبة النظام
Value lib_system_execute(Value *args, int arg_count);
Value lib_system_get_env(Value *args, int arg_count);
Value lib_system_set_env(Value *args, int arg_count);
Value lib_system_get_args(Value *args, int arg_count);
Value lib_system_exit(Value *args, int arg_count);
Value lib_system_get_pid(Value *args, int arg_count);
Value lib_system_get_cwd(Value *args, int arg_count);
Value lib_system_chdir(Value *args, int arg_count);
Value lib_system_platform(Value *args, int arg_count);
Value lib_system_arch(Value *args, int arg_count);
Value lib_system_cpu_count(Value *args, int arg_count);
Value lib_system_memory(Value *args, int arg_count);
Value lib_system_free_memory(Value *args, int arg_count);
Value lib_system_hostname(Value *args, int arg_count);
Value lib_system_username(Value *args, int arg_count);
Value lib_system_temp_dir(Value *args, int arg_count);
Value lib_system_home_dir(Value *args, int arg_count);

// مكتبة المخزن
void lib_store_init(void);
Value lib_store_create(Value *args, int arg_count);
Value lib_store_add(Value *args, int arg_count);
Value lib_store_read(Value *args, int arg_count);
Value lib_store_delete(Value *args, int arg_count);
Value lib_store_list(Value *args, int arg_count);
Value lib_store_clear(Value *args, int arg_count);
Value lib_store_save(Value *args, int arg_count);
Value lib_store_load(Value *args, int arg_count);

// مكتبة الذكاء
void lib_ai_init(void);
Value lib_mind_create(Value *args, int arg_count);
Value lib_mind_learn(Value *args, int arg_count);
Value lib_mind_ask(Value *args, int arg_count);
Value lib_mind_save(Value *args, int arg_count);
Value lib_mind_load(Value *args, int arg_count);
Value lib_mind_clear(Value *args, int arg_count);
Value lib_system_create(Value *args, int arg_count);
Value lib_system_add(Value *args, int arg_count);
Value lib_system_respond(Value *args, int arg_count);
Value lib_neural_create(Value *args, int arg_count);
Value lib_neural_train(Value *args, int arg_count);
Value lib_neural_predict(Value *args, int arg_count);
Value lib_neural_save(Value *args, int arg_count);
Value lib_neural_load(Value *args, int arg_count);
Value lib_neural_use_gpu(Value *args, int arg_count);
Value lib_neural_get_accuracy(Value *args, int arg_count);
Value lib_dataset_load(Value *args, int arg_count);
Value lib_dataset_split(Value *args, int arg_count);

// مكتبة الوسائط
Value lib_media_show_image(Value *args, int arg_count);
Value lib_media_play_video(Value *args, int arg_count);
Value lib_media_play_audio(Value *args, int arg_count);
Value lib_media_record_audio(Value *args, int arg_count);
Value lib_media_take_photo(Value *args, int arg_count);
Value lib_media_convert_format(Value *args, int arg_count);
Value lib_media_get_info(Value *args, int arg_count);
Value lib_media_resize_image(Value *args, int arg_count);

// مكتبة الواجهات
Value lib_gui_create_window(Value *args, int arg_count);
Value lib_gui_add_button(Value *args, int arg_count);
Value lib_gui_add_label(Value *args, int arg_count);
Value lib_gui_add_input(Value *args, int arg_count);
Value lib_gui_add_textarea(Value *args, int arg_count);
Value lib_gui_add_checkbox(Value *args, int arg_count);
Value lib_gui_add_radio(Value *args, int arg_count);
Value lib_gui_add_dropdown(Value *args, int arg_count);
Value lib_gui_add_slider(Value *args, int arg_count);
Value lib_gui_add_progress(Value *args, int arg_count);
Value lib_gui_add_menu(Value *args, int arg_count);
Value lib_gui_show_dialog(Value *args, int arg_count);
Value lib_gui_run_loop(Value *args, int arg_count);

// مكتبة الشبكة
Value lib_net_http_get(Value *args, int arg_count);
Value lib_net_http_post(Value *args, int arg_count);
Value lib_net_http_put(Value *args, int arg_count);
Value lib_net_http_delete(Value *args, int arg_count);
Value lib_net_http_download(Value *args, int arg_count);
Value lib_net_create_server(Value *args, int arg_count);
Value lib_net_create_websocket(Value *args, int arg_count);
Value lib_net_parse_url(Value *args, int arg_count);
Value lib_net_encode_url(Value *args, int arg_count);
Value lib_net_decode_url(Value *args, int arg_count);
Value lib_net_send_email(Value *args, int arg_count);
Value lib_net_ftp_upload(Value *args, int arg_count);
Value lib_net_ftp_download(Value *args, int arg_count);
Value lib_net_ping(Value *args, int arg_count);
Value lib_net_dns_lookup(Value *args, int arg_count);

// مكتبة قواعد البيانات
Value lib_db_connect(Value *args, int arg_count);
Value lib_db_disconnect(Value *args, int arg_count);
Value lib_db_query(Value *args, int arg_count);
Value lib_db_execute(Value *args, int arg_count);
Value lib_db_insert(Value *args, int arg_count);
Value lib_db_update(Value *args, int arg_count);
Value lib_db_delete(Value *args, int arg_count);
Value lib_db_select(Value *args, int arg_count);
Value lib_db_create_table(Value *args, int arg_count);
Value lib_db_drop_table(Value *args, int arg_count);
Value lib_db_begin_transaction(Value *args, int arg_count);
Value lib_db_commit(Value *args, int arg_count);
Value lib_db_rollback(Value *args, int arg_count);
Value lib_db_get_tables(Value *args, int arg_count);
Value lib_db_get_columns(Value *args, int arg_count);

// معلومات الميتا
Value lib_meta_program_name(Value *args, int arg_count);
Value lib_meta_version(Value *args, int arg_count);
Value lib_meta_os(Value *args, int arg_count);
Value lib_meta_arch(Value *args, int arg_count);
Value lib_meta_memory_usage(Value *args, int arg_count);
Value lib_meta_gc(Value *args, int arg_count);
Value lib_meta_call_stack(Value *args, int arg_count);
Value lib_meta_loaded_modules(Value *args, int arg_count);

// دوال مساعدة
char *utf8_strdup(const char *src);
int utf8_strlen(const char *str);
char *utf8_substring(const char *str, int start, int length);
bool is_arabic_letter(char c);
bool is_arabic_digit(char c);
bool is_arabic_string(const char *str);
char *arabic_number_to_words(int num);
int arabic_words_to_number(const char *words);
char *format_arabic_error(const char *message, int line, int column, const char *filename);
void print_arabic_error(const char *message, int line, int column, const char *filename);

#endif // WISAM_H
