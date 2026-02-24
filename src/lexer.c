#include "wisam.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

// كلمات مفتاحية عربية
static struct {
    const char *word;
    TokenType type;
} arabic_keywords[] = {
    {"ليكن", TOKEN_LET},
    {"ثابت", TOKEN_CONST},
    {"إذا", TOKEN_IF},
    {"اذا", TOKEN_IF},
    {"إذن", TOKEN_THEN},
    {"اذن", TOKEN_THEN},
    {"وإلا", TOKEN_ELSE},
    {"وإلا", TOKEN_ELSE},
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
    {"اكتب", TOKEN_PRINT},
    {"أكتب", TOKEN_PRINT},
    {"ادخل", TOKEN_INPUT},
    {"أدخل", TOKEN_INPUT},
    {"عقل", TOKEN_MIND},
    {"منظومة", TOKEN_SYSTEM},
    {"تعلّم", TOKEN_LEARN},
    {"تعلم", TOKEN_LEARN},
    {"اسأل", TOKEN_ASK},
    {"استجب", TOKEN_RESPONSE},
    {"احفظ", TOKEN_SAVE},
    {"حمّل", TOKEN_LOAD},
    {"حمل", TOKEN_LOAD},
    {"شبكة_عصبية", TOKEN_NEURAL},
    {"طبقات", TOKEN_LAYERS},
    {"معدل_تعلم", TOKEN_LEARNING_RATE},
    {"درّب", TOKEN_TRAIN},
    {"درب", TOKEN_TRAIN},
    {NULL, 0}
};

Lexer *lexer_create(const char *source) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = strdup(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->length = strlen(source);
    
    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer) {
        free(lexer->source);
        free(lexer);
    }
}

static char lexer_peek(Lexer *lexer) {
    if (lexer->position >= lexer->length) return '\0';
    return lexer->source[lexer->position];
}

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

static void lexer_skip_whitespace(Lexer *lexer) {
    while (lexer_peek(lexer) == ' ' || lexer_peek(lexer) == '\t' || 
           lexer_peek(lexer) == '\r') {
        lexer_advance(lexer);
    }
}

static void lexer_skip_comment(Lexer *lexer) {
    if (lexer_peek(lexer) == '#') {
        while (lexer_peek(lexer) != '\n' && lexer_peek(lexer) != '\0') {
            lexer_advance(lexer);
        }
    }
}

static Token create_token(TokenType type, const char *value, int line, int column) {
    Token token;
    token.type = type;
    strncpy(token.value, value, MAX_TOKEN_LENGTH - 1);
    token.value[MAX_TOKEN_LENGTH - 1] = '\0';
    token.line = line;
    token.column = column;
    return token;
}

static TokenType get_keyword_type(const char *word) {
    for (int i = 0; arabic_keywords[i].word != NULL; i++) {
        if (strcmp(word, arabic_keywords[i].word) == 0) {
            return arabic_keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

static bool is_arabic_char(unsigned char c) {
    // نطاق الحروف العربية في UTF-8
    return (c >= 0x80);
}

static Token read_string(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    
    lexer_advance(lexer); // تخطي "
    
    while (lexer_peek(lexer) != '"' && lexer_peek(lexer) != '\0' && i < MAX_TOKEN_LENGTH - 1) {
        buffer[i++] = lexer_advance(lexer);
    }
    
    buffer[i] = '\0';
    lexer_advance(lexer); // تخطي "
    
    return create_token(TOKEN_STRING, buffer, start_line, start_col);
}

static Token read_number(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    char buffer[MAX_TOKEN_LENGTH];
    int i = 0;
    bool has_dot = false;
    
    while ((isdigit(lexer_peek(lexer)) || lexer_peek(lexer) == '.') && i < MAX_TOKEN_LENGTH - 1) {
        if (lexer_peek(lexer) == '.') {
            if (has_dot) break;
            has_dot = true;
        }
        buffer[i++] = lexer_advance(lexer);
    }
    
    buffer[i] = '\0';
    return create_token(TOKEN_NUMBER, buffer, start_line, start_col);
}

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
    return create_token(type, buffer, start_line, start_col);
}

Token *lexer_tokenize(Lexer *lexer, int *token_count) {
    Token *tokens = malloc(sizeof(Token) * 10000);
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
            tokens[count++] = create_token(TOKEN_NEWLINE, "\n", line, col);
            lexer_advance(lexer);
            continue;
        }
        
        // سلسلة نصية
        if (c == '"') {
            tokens[count++] = read_string(lexer);
            continue;
        }
        
        // رقم
        if (isdigit(c)) {
            tokens[count++] = read_number(lexer);
            continue;
        }
        
        // معرف أو كلمة مفتاحية
        if (is_arabic_char((unsigned char)c) || isalpha(c) || c == '_') {
            tokens[count++] = read_identifier(lexer);
            continue;
        }
        
        // رموز العمليات
        switch (c) {
            case '=':
                lexer_advance(lexer);
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_EQUAL, "==", line, col);
                } else {
                    tokens[count++] = create_token(TOKEN_ASSIGN, "=", line, col);
                }
                break;
            case '+':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_PLUS, "+", line, col);
                break;
            case '-':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_MINUS, "-", line, col);
                break;
            case '*':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_MULTIPLY, "*", line, col);
                break;
            case '/':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_DIVIDE, "/", line, col);
                break;
            case '%':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_MODULO, "%", line, col);
                break;
            case '^':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_POWER, "^", line, col);
                break;
            case '>':
                lexer_advance(lexer);
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_GREATER_EQ, ">=", line, col);
                } else {
                    tokens[count++] = create_token(TOKEN_GREATER, ">", line, col);
                }
                break;
            case '<':
                lexer_advance(lexer);
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_LESS_EQ, "<=", line, col);
                } else {
                    tokens[count++] = create_token(TOKEN_LESS, "<", line, col);
                }
                break;
            case '!':
                lexer_advance(lexer);
                if (lexer_peek(lexer) == '=') {
                    lexer_advance(lexer);
                    tokens[count++] = create_token(TOKEN_NOT_EQUAL, "!=", line, col);
                } else {
                    tokens[count++] = create_token(TOKEN_NOT, "!", line, col);
                }
                break;
            case '(':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_LPAREN, "(", line, col);
                break;
            case ')':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_RPAREN, ")", line, col);
                break;
            case '{':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_LBRACE, "{", line, col);
                break;
            case '}':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_RBRACE, "}", line, col);
                break;
            case '[':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_LBRACKET, "[", line, col);
                break;
            case ']':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_RBRACKET, "]", line, col);
                break;
            case '.':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_DOT, ".", line, col);
                break;
            case ',':
            case '،':  // الفاصلة العربية
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_COMMA, ",", line, col);
                break;
            case ':':
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_COLON, ":", line, col);
                break;
            case ';':
            case '؛':  // الفاصلة المنقوطة العربية
                lexer_advance(lexer);
                tokens[count++] = create_token(TOKEN_SEMICOLON, ";", line, col);
                break;
            default:
                // تخطي أي حرف غير معروف
                lexer_advance(lexer);
                break;
        }
    }
    
    tokens[count++] = create_token(TOKEN_EOF, "EOF", lexer->line, lexer->column);
    *token_count = count;
    return tokens;
}
