#ifndef LEXER_H
#define LEXER_H

#include "wisam.h"

// Arabic Keywords (Token Types)
typedef enum {
    // Special tokens
    TOKEN_EOF,
    TOKEN_NEWLINE,
    TOKEN_ERROR,
    
    // Literals
    TOKEN_NUMBER,
    TOKEN_STRING,
    TOKEN_IDENTIFIER,
    
    // Keywords - Variables
    TOKEN_LET,          // ليكن
    TOKEN_CONST,        // ثابت
    TOKEN_IS,           // هو
    
    // Keywords - I/O
    TOKEN_WRITE,        // اكتب
    TOKEN_READ,         // اقرأ
    
    // Keywords - Control Flow
    TOKEN_IF,           // إذا
    TOKEN_THEN,         // إذن
    TOKEN_ELSE,         // وإلا
    TOKEN_END,          // انتهى
    TOKEN_FOR,          // لكل
    TOKEN_FROM,         // من
    TOKEN_TO,           // إلى
    TOKEN_WHILE,        // طالما
    
    // Keywords - Functions
    TOKEN_FUNCTION,     // دالة
    TOKEN_TAKES,        // تأخذ
    TOKEN_AND,          // و
    TOKEN_RETURN,       // أعد
    
    // Keywords - Structs
    TOKEN_STRUCT,       // هيكل
    TOKEN_DOT,          // .
    
    // Keywords - Modules
    TOKEN_IMPORT,       // استورد
    
    // Keywords - AI
    TOKEN_CREATE_MIND,  // أنشئ عقل
    TOKEN_CREATE_ECOSYSTEM, // أنشئ منظومة
    TOKEN_CREATE_NEURAL, // أنشئ شبكة عصبية
    TOKEN_WITH_NAME,    // باسم
    TOKEN_LEARN,        // تعلّم
    TOKEN_ASK,          // اسأل
    TOKEN_ADD,          // أضف
    TOKEN_TEXT_MIND,    // عقل نصي
    TOKEN_LONG_MEMORY,  // ذاكرة طويلة
    TOKEN_SELF_LEARN,   // تعلّم_ذاتي
    TOKEN_RESPOND,      // استجب
    TOKEN_LAYERS,       // طبقات
    TOKEN_LEARNING_RATE, // معدل_تعلم
    TOKEN_TRAIN,        // درّب
    TOKEN_USE_GPU,      // استخدم GPU
    TOKEN_RUN_APP,      // أدر_التطبيق
    TOKEN_SAVE,         // احفظ
    
    // Keywords - Meta
    TOKEN_META,         // ميتا
    
    // Operators
    TOKEN_PLUS,         // +
    TOKEN_MINUS,        // -
    TOKEN_MULTIPLY,     // *
    TOKEN_DIVIDE,       // /
    TOKEN_MODULO,       // %
    TOKEN_ASSIGN,       // =
    TOKEN_EQUAL,        // ==
    TOKEN_NOT_EQUAL,    // !=
    TOKEN_LESS,         // <
    TOKEN_GREATER,      // >
    TOKEN_LESS_EQUAL,   // <=
    TOKEN_GREATER_EQUAL,// >=
    
    // Delimiters
    TOKEN_LPAREN,       // (
    TOKEN_RPAREN,       // )
    TOKEN_LBRACE,       // {
    TOKEN_RBRACE,       // }
    TOKEN_LBRACKET,     // [
    TOKEN_RBRACKET,     // ]
    TOKEN_COMMA,        // ،
    TOKEN_COLON,        // :
    TOKEN_QUOTE,        // "
    
    // Special for string interpolation
    TOKEN_INTERP_START, // {
    TOKEN_INTERP_END,   // }
    
    // Boolean literals
    TOKEN_TRUE,         // صحيح
    TOKEN_FALSE,        // خطأ
    
    // Null
    TOKEN_NULL          // فارغ
} TokenType;

typedef struct {
    TokenType type;
    char *value;
    int line;
    int column;
} Token;

typedef struct {
    char *source;
    int pos;
    int line;
    int column;
    int length;
} Lexer;

// Lexer functions
Lexer *lexer_create(const char *source);
void lexer_destroy(Lexer *lexer);
Token *lexer_next_token(Lexer *lexer);
Token *token_create(TokenType type, const char *value, int line, int column);
void token_destroy(Token *token);
const char *token_type_to_string(TokenType type);

// Helper to check if a string is an Arabic keyword
TokenType check_arabic_keyword(const char *word);

#endif // LEXER_H
