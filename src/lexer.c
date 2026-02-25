#include "wisam.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// كلمات مفتاحية عربية محسّنة
static struct {
    const char *word;
    TokenType type;
} arabic_keywords[] = {
    // الكلمات الأساسية
    {"ليكن", TOKEN_LET},
    {"ثابت", TOKEN_CONST},
    {"إذا", TOKEN_IF},
    {"اذا", TOKEN_IF},
    {"إذن", TOKEN_THEN},
    {"اذن", TOKEN_THEN},
    {"وإلا", TOKEN_ELSE},
    {"وانلا", TOKEN_ELSE},
    {"انتهى", TOKEN_END},
    {"انتهي", TOKEN_END},
    {"لكل", TOKEN_FOR},
    {"من", TOKEN_FROM},
    {"إلى", TOKEN_TO},
    {"الى", TOKEN_TO},
    {"دالة", TOKEN_FUNCTION},
    {"أعد", TOKEN_RETURN},
    {"اعد", TOKEN_RETURN},
    {"هيكل", TOKEN_STRUCT},
    {"صنف", TOKEN_CLASS},
    {"استورد", TOKEN_IMPORT},
    {"أنشئ", TOKEN_CREATE},
    {"انشئ", TOKEN_CREATE},
    {"باسم", TOKEN_AS},
    {"طالما", TOKEN_WHILE},
    {"توقف", TOKEN_BREAK},
    {"استمر", TOKEN_CONTINUE},
    {"و", TOKEN_AND},
    {"أو", TOKEN_OR},
    {"او", TOKEN_OR},
    {"ليس", TOKEN_NOT},
    {"في", TOKEN_IN},
    {"من", TOKEN_OF},
    {"حاول", TOKEN_TRY},
    {"امسك", TOKEN_CATCH},
    {"أخيراً", TOKEN_FINALLY},
    {"اخيراً", TOKEN_FINALLY},
    {"ألقِ", TOKEN_THROW},
    {"القي", TOKEN_THROW},
    {"حسب", TOKEN_SWITCH},
    {"حالة", TOKEN_CASE},
    {"افتراضي", TOKEN_DEFAULT},
    {"نفذ", TOKEN_DO},
    {"حتى", TOKEN_UNTIL},
    {"لكل_عنصر", TOKEN_FOREACH},
    {"أنتج", TOKEN_YIELD},
    {"انتج", TOKEN_YIELD},
    {"غير_متزامن", TOKEN_ASYNC},
    {"انتظر", TOKEN_AWAIT},
    {"عام", TOKEN_PUBLIC},
    {"خاص", TOKEN_PRIVATE},
    {"محمي", TOKEN_PROTECTED},
    {"ثابت_الصنف", TOKEN_STATIC},
    {"يرث", TOKEN_EXTENDS},
    {"ينفذ", TOKEN_IMPLEMENTS},
    {"جديد", TOKEN_NEW},
    {"هذا", TOKEN_THIS},
    {"أب", TOKEN_SUPER},
    {"اب", TOKEN_SUPER},
    {"فارغ", TOKEN_NULL},
    {"صحيح", TOKEN_TRUE},
    {"خطأ", TOKEN_FALSE},
    
    // دوال الإخراج والإدخال
    {"اكتب", TOKEN_PRINT},
    {"أكتب", TOKEN_PRINT},
    {"ادخل", TOKEN_INPUT},
    {"أدخل", TOKEN_INPUT},
    {"نوع", TOKEN_TYPEOF},
    {"حجم", TOKEN_SIZEOF},
    
    // كلمات الذكاء الاصطناعي
    {"عقل", TOKEN_MIND},
    {"منظومة", TOKEN_SYSTEM},
    {"تعلّم", TOKEN_LEARN},
    {"تعلم", TOKEN_LEARN},
    {"اسأل", TOKEN_ASK},
    {"استجب", TOKEN_RESPONSE},
    {"احفظ", TOKEN_SAVE},
    {"حمّل", TOKEN_LOAD},
    {"حمل", TOKEN_LOAD},
    {"GPU", TOKEN_GPU},
    {"درّب", TOKEN_TRAIN},
    {"درب", TOKEN_TRAIN},
    {"شبكة_عصبية", TOKEN_NEURAL},
    {"طبقات", TOKEN_LAYERS},
    {"معدل_تعلم", TOKEN_LEARNING_RATE},
    {"نموذج", TOKEN_MODEL},
    {"تنبأ", TOKEN_PREDICT},
    {"مجموعة_بيانات", TOKEN_DATASET},
    
    {NULL, 0}
};

// إنشاء الليكسر
Lexer *lexer_create(const char *source, const char *filename) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = strdup(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->length = strlen(source);
    lexer->filename = filename ? strdup(filename) : strdup("<unknown>");
    lexer->error_message = NULL;
    
    return lexer;
}

// تدمير الليكسر
void lexer_destroy(Lexer *lexer) {
    if (lexer) {
        free(lexer->source);
        free(lexer->filename);
        free(lexer->error_message);
        free(lexer);
    }
}

// الحصول على رسالة الخطأ
char *lexer_get_error(Lexer *lexer) {
    return lexer ? lexer->error_message : NULL;
}

// النظر إلى الحرف الحالي
static char lexer_peek(Lexer *lexer) {
    if (lexer->position >= lexer->length) return '\0';
    return lexer->source[lexer->position];
}

// النظر إلى الحرف التالي
static char lexer_peek_next(Lexer *lexer) {
    if (lexer->position + 1 >= lexer->length) return '\0';
    return lexer->source[lexer->position + 1];
}

// التقدم إلى الحرف التالي
static char lexer_advance(Lexer *lexer) {
    if (lexer->position >= lexer->length) return '\0';
    char c = lexer->source[lexer->position];
    lexer->position++;
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    return c;
}

// تخطي المسافات البيضاء
static void lexer_skip_whitespace(Lexer *lexer) {
    while (lexer_peek(lexer) == ' ' || lexer_peek(lexer) == '\t' || 
           lexer_peek(lexer) == '\r') {
        lexer_advance(lexer);
    }
}

// تخطي التعليقات
static void lexer_skip_comment(Lexer *lexer) {
    if (lexer_peek(lexer) == '#') {
        while (lexer_peek(lexer) != '\n' && lexer_peek(lexer) != '\0') {
            lexer_advance(lexer);
        }
    }
    // تعليقات متعددة الأسطر /* */
    else if (lexer_peek(lexer) == '/' && lexer_peek_next(lexer) == '*') {
        lexer_advance(lexer); // /
        lexer_advance(lexer); // *
        while (!(lexer_peek(lexer) == '*' && lexer_peek_next(lexer) == '/') && 
               lexer_peek(lexer) != '\0') {
            lexer_advance(lexer);
        }
        if (lexer_peek(lexer) != '\0') {
            lexer_advance(lexer); // *
            lexer_advance(lexer); // /
        }
    }
}

// إنشاء رمز
static Token create_token(TokenType type, const char *value, int line, int column, const char *filename) {
    Token token;
    token.type = type;
    strncpy(token.value, value, MAX_TOKEN_LENGTH - 1);
    token.value[MAX_TOKEN_LENGTH - 1] = '\0';
    token.line = line;
    token.column = column;
    token.filename = strdup(filename);
    return token;
}

// الحصول على نوع الكلمة المفتاحية
static TokenType get_keyword_type(const char *word) {
    for (int i = 0; arabic_keywords[i].word != NULL; i++) {
        if (strcmp(word, arabic_keywords[i].word) == 0) {
            return arabic_keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

// التحقق من الحرف العربي
static bool is_arabic_char(unsigned char c) {
    return (c >= 0x80);
}

// التحقق من بداية حرف UTF-8
static bool is_utf8_start(unsigned char c) {
    return (c & 0xC0) != 0x80;
}

// قراءة السلسلة النصية
static Token read_string(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    
    char quote = lexer_advance(lexer); // " أو '
    
    while (lexer_peek(lexer) != quote && lexer_peek(lexer) != '\0' && i < MAX_TOKEN_LENGTH - 1) {
        char c = lexer_advance(lexer);
        // معالجة الأحرف الهروبية
        if (c == '\\' && lexer_peek(lexer) != '\0') {
            char next = lexer_advance(lexer);
            switch (next) {
                case 'n': c = '\n'; break;
                case 't': c = '\t'; break;
                case 'r': c = '\r'; break;
                case '\\': c = '\\'; break;
                case '"': c = '"'; break;
                case '\'': c = '\''; break;
                default: 
                    buffer[i++] = c;
                    c = next;
            }
        }
        buffer[i++] = c;
    }
    
    buffer[i] = '\0';
    lexer_advance(lexer); // تخطي " أو '
    
    return create_token(TOKEN_STRING, buffer, start_line, start_col, lexer->filename);
}

// قراءة الرقم
static Token read_number(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    bool has_dot = false;
    bool has_exp = false;
    
    while ((isdigit(lexer_peek(lexer)) || lexer_peek(lexer) == '.' || 
            lexer_peek(lexer) == 'e' || lexer_peek(lexer) == 'E' ||
            lexer_peek(lexer) == '+' || lexer_peek(lexer) == '-') && i < MAX_TOKEN_LENGTH - 1) {
        char c = lexer_peek(lexer);
        
        if (c == '.') {
            if (has_dot) break;
            has_dot = true;
        } else if (c == 'e' || c == 'E') {
            if (has_exp) break;
            has_exp = true;
        } else if (c == '+' || c == '-') {
            if (i == 0 || (buffer[i-1] != 'e' && buffer[i-1] != 'E')) {
                break;
            }
        }
        
        buffer[i++] = lexer_advance(lexer);
    }
    
    buffer[i] = '\0';
    return create_token(TOKEN_NUMBER, buffer, start_line, start_col, lexer->filename);
}

// قراءة المعرف
static Token read_identifier(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    
    // قراءة الحروف العربية والإنجليزية والأرقام والشرطة السفلية
    while ((is_arabic_char((unsigned char)lexer_peek(lexer)) || 
            isalnum((unsigned char)lexer_peek(lexer)) || 
            lexer_peek(lexer) == '_') && i < MAX_TOKEN_LENGTH - 1) {
        buffer[i++] = lexer_advance(lexer);
    }
    
    buffer[i] = '\0';
    
    TokenType type = get_keyword_type(buffer);
    return create_token(type, buffer, start_line, start_col, lexer->filename);
}

// قراءة معامل متعدد الأحرف
static Token read_multi_char_operator(Lexer *lexer, TokenType single_type, 
                                       char next_char, TokenType double_type) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    char buffer[3] = {0};
    
    buffer[0] = lexer_advance(lexer);
    
    if (lexer_peek(lexer) == next_char) {
        buffer[1] = lexer_advance(lexer);
        return create_token(double_type, buffer, start_line, start_col, lexer->filename);
    }
    
    buffer[1] = '\0';
    return create_token(single_type, buffer, start_line, start_col, lexer->filename);
}

// تحليل المصدر إلى رموز
Token *lexer_tokenize(Lexer *lexer, int *token_count) {
    Token *tokens = malloc(sizeof(Token) * 100000);
    if (!tokens) {
        lexer->error_message = strdup("فشل في تخصيص الذاكرة للرموز");
        return NULL;
    }
    
    int count = 0;
    
    while (lexer->position < lexer->length) {
        lexer_skip_whitespace(lexer);
        lexer_skip_comment(lexer);
        lexer_skip_whitespace(lexer);
        
        if (lexer->position >= lexer->length) break;
        
        char c = lexer_peek(lexer);
        int line = lexer->line;
        int col = lexer->column;
        
        // نهاية السطر
        if (c == '\n') {
            tokens[count++] = create_token(TOKEN_NEWLINE, "\n", line, col, lexer->filename);
            lexer_advance(lexer);
            continue;
        }
        
        // السلاسل النصية
        if (c == '"' || c == '\'') {
            tokens[count++] = read_string(lexer);
            continue;
        }
        
        // الأرقام
        if (isdigit(c)) {
            tokens[count++] = read_number(lexer);
            continue;
        }
        
        // المعرفات والكلمات المفتاحية
        if (is_arabic_char((unsigned char)c) || isalpha((unsigned char)c) || c == '_') {
            tokens[count++] = read_identifier(lexer);
            continue;
        }
        
        // المعاملات
        switch (c) {
            case '=':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_EQUAL, "==", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_ASSIGN, "=", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '!':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_NOT_EQUAL, "!=", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_NOT, "!", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '<':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_LESS_EQ, "<=", line, col, lexer->filename);
                } else if (lexer_peek_next(lexer) == '<') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_SHIFT_LEFT, "<<", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_LESS, "<", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '>':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_GREATER_EQ, ">=", line, col, lexer->filename);
                } else if (lexer_peek_next(lexer) == '>') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_SHIFT_RIGHT, ">>", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_GREATER, ">", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '+':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_PLUS_ASSIGN, "+=", line, col, lexer->filename);
                } else if (lexer_peek_next(lexer) == '+') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_INCREMENT, "++", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_PLUS, "+", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '-':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_MINUS_ASSIGN, "-=", line, col, lexer->filename);
                } else if (lexer_peek_next(lexer) == '-') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_DECREMENT, "--", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_MINUS, "-", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '*':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_MUL_ASSIGN, "*=", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_MULTIPLY, "*", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '/':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_DIV_ASSIGN, "/=", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_DIVIDE, "/", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '%':
                if (lexer_peek_next(lexer) == '=') {
                    lexer_advance(lexer);
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_MOD_ASSIGN, "%=", line, col, lexer->filename);
                } else {
                    tokens[count++] = create_token(TOKEN_MODULO, "%", line, col, lexer->filename);
                    lexer_advance(lexer);
                }
                break;
                
            case '^':
                tokens[count++] = create_token(TOKEN_POWER, "^", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '(':
                tokens[count++] = create_token(TOKEN_LPAREN, "(", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case ')':
                tokens[count++] = create_token(TOKEN_RPAREN, ")", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '{':
                tokens[count++] = create_token(TOKEN_LBRACE, "{", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '}':
                tokens[count++] = create_token(TOKEN_RBRACE, "}", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '[':
                tokens[count++] = create_token(TOKEN_LBRACKET, "[", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case ']':
                tokens[count++] = create_token(TOKEN_RBRACKET, "]", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case ',':
            case '،':
                tokens[count++] = create_token(TOKEN_COMMA, ",", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '.':
                tokens[count++] = create_token(TOKEN_DOT, ".", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case ':':
                tokens[count++] = create_token(TOKEN_COLON, ":", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case ';':
            case '؛':
                tokens[count++] = create_token(TOKEN_SEMICOLON, ";", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '&':
                tokens[count++] = create_token(TOKEN_BIT_AND, "&", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '|':
                tokens[count++] = create_token(TOKEN_BIT_OR, "|", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            case '~':
                tokens[count++] = create_token(TOKEN_BIT_NOT, "~", line, col, lexer->filename);
                lexer_advance(lexer);
                break;
                
            default:
                // حرف غير معروف
                {
                    char error_msg[256];
                    snprintf(error_msg, sizeof(error_msg), 
                             "حرف غير معروف: '%c' (السطر %d، العمود %d)", 
                             c, line, col);
                    lexer->error_message = strdup(error_msg);
                    
                    // تخطي الحرف ومواصلة التحليل
                    lexer_advance(lexer);
                }
                break;
        }
    }
    
    // إضافة رمز نهاية الملف
    tokens[count++] = create_token(TOKEN_EOF, "", lexer->line, lexer->column, lexer->filename);
    
    *token_count = count;
    return tokens;
}
