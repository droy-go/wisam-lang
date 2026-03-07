#include "lexer.h"
#include <wctype.h>

// Arabic keywords mapping
static struct {
    const char *arabic;
    TokenType type;
} arabic_keywords[] = {
    // Variables
    {"ليكن", TOKEN_LET},
    {"ثابت", TOKEN_CONST},
    {"هو", TOKEN_IS},
    
    // I/O
    {"اكتب", TOKEN_WRITE},
    {"اقرأ", TOKEN_READ},
    
    // Control flow
    {"إذا", TOKEN_IF},
    {"اذا", TOKEN_IF},
    {"إذن", TOKEN_THEN},
    {"اذن", TOKEN_THEN},
    {"وإلا", TOKEN_ELSE},
    {"وإلا", TOKEN_ELSE},
    {"انتهى", TOKEN_END},
    {"لكل", TOKEN_FOR},
    {"من", TOKEN_FROM},
    {"إلى", TOKEN_TO},
    {"الى", TOKEN_TO},
    {"طالما", TOKEN_WHILE},
    
    // Functions
    {"دالة", TOKEN_FUNCTION},
    {"تأخذ", TOKEN_TAKES},
    {"و", TOKEN_AND},
    {"أعد", TOKEN_RETURN},
    
    // Structs
    {"هيكل", TOKEN_STRUCT},
    
    // Modules
    {"استورد", TOKEN_IMPORT},
    
    // AI Keywords
    {"أنشئ", TOKEN_CREATE_MIND},
    {"انشئ", TOKEN_CREATE_MIND},
    {"عقل", TOKEN_CREATE_MIND},
    {"منظومة", TOKEN_CREATE_ECOSYSTEM},
    {"شبكة", TOKEN_CREATE_NEURAL},
    {"عصبية", TOKEN_CREATE_NEURAL},
    {"باسم", TOKEN_WITH_NAME},
    {"تعلّم", TOKEN_LEARN},
    {"تعلم", TOKEN_LEARN},
    {"اسأل", TOKEN_ASK},
    {"اسال", TOKEN_ASK},
    {"أضف", TOKEN_ADD},
    {"اضف", TOKEN_ADD},
    {"نصي", TOKEN_TEXT_MIND},
    {"ذاكرة", TOKEN_LONG_MEMORY},
    {"طويلة", TOKEN_LONG_MEMORY},
    {"تعلّم_ذاتي", TOKEN_SELF_LEARN},
    {"تعلم_ذاتي", TOKEN_SELF_LEARN},
    {"استجب", TOKEN_RESPOND},
    {"طبقات", TOKEN_LAYERS},
    {"معدل_تعلم", TOKEN_LEARNING_RATE},
    {"درّب", TOKEN_TRAIN},
    {"درب", TOKEN_TRAIN},
    {"استخدم", TOKEN_USE_GPU},
    {"أدر_التطبيق", TOKEN_RUN_APP},
    {"ادر_التطبيق", TOKEN_RUN_APP},
    {"احفظ", TOKEN_SAVE},
    
    // Meta
    {"ميتا", TOKEN_META},
    
    // Boolean
    {"صحيح", TOKEN_TRUE},
    {"خطأ", TOKEN_FALSE},
    {"خطا", TOKEN_FALSE},
    
    // Null
    {"فارغ", TOKEN_NULL},
    
    {NULL, TOKEN_IDENTIFIER}
};

Lexer *lexer_create(const char *source) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = strdup(source);
    lexer->pos = 0;
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

Token *token_create(TokenType type, const char *value, int line, int column) {
    Token *token = malloc(sizeof(Token));
    if (!token) return NULL;
    
    token->type = type;
    token->value = value ? strdup(value) : NULL;
    token->line = line;
    token->column = column;
    
    return token;
}

void token_destroy(Token *token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

// Check if character is Arabic
static int is_arabic(unsigned char c) {
    return (c >= 0xD8 && c <= 0xDB);
}

// Check if character is part of Arabic word
static int is_arabic_char(unsigned char c) {
    return is_arabic(c) || 
           (c >= 0x80 && c <= 0xBF) ||  // Arabic continuation bytes
           c == '_' ||
           (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9');
}

// Get current character
static char lexer_peek(Lexer *lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    return lexer->source[lexer->pos];
}

// Get next character
static char lexer_advance(Lexer *lexer) {
    if (lexer->pos >= lexer->length) return '\0';
    char c = lexer->source[lexer->pos];
    lexer->pos++;
    
    // Handle UTF-8 multi-byte characters
    if ((unsigned char)c >= 0xC0) {
        int bytes = 0;
        if ((unsigned char)c >= 0xFC) bytes = 5;
        else if ((unsigned char)c >= 0xF8) bytes = 4;
        else if ((unsigned char)c >= 0xF0) bytes = 3;
        else if ((unsigned char)c >= 0xE0) bytes = 2;
        else if ((unsigned char)c >= 0xC0) bytes = 1;
        
        for (int i = 0; i < bytes && lexer->pos < lexer->length; i++) {
            lexer->pos++;
        }
    }
    
    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    return c;
}

// Skip whitespace and comments
static void lexer_skip_whitespace(Lexer *lexer) {
    while (1) {
        char c = lexer_peek(lexer);
        
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            lexer_advance(lexer);
        } else if (c == '#') {
            // Skip comment until end of line
            while (lexer_peek(lexer) != '\n' && lexer_peek(lexer) != '\0') {
                lexer_advance(lexer);
            }
        } else {
            break;
        }
    }
}

// Read a string literal
static Token *lexer_read_string(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    lexer_advance(lexer); // consume opening quote
    
    char *value = malloc(MAX_TOKEN_LEN);
    int i = 0;
    
    while (lexer_peek(lexer) != '"' && lexer_peek(lexer) != '\0') {
        char c = lexer_advance(lexer);
        
        if (c == '\\') {
            char next = lexer_advance(lexer);
            switch (next) {
                case 'n': value[i++] = '\n'; break;
                case 't': value[i++] = '\t'; break;
                case 'r': value[i++] = '\r'; break;
                case '\\': value[i++] = '\\'; break;
                case '"': value[i++] = '"'; break;
                default: value[i++] = next; break;
            }
        } else {
            value[i++] = c;
        }
        
        if (i >= MAX_TOKEN_LEN - 1) break;
    }
    
    value[i] = '\0';
    
    if (lexer_peek(lexer) == '"') {
        lexer_advance(lexer); // consume closing quote
    }
    
    Token *token = token_create(TOKEN_STRING, value, start_line, start_col);
    free(value);
    return token;
}

// Read a number
static Token *lexer_read_number(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    char *value = malloc(MAX_TOKEN_LEN);
    int i = 0;
    bool has_dot = false;
    
    while (1) {
        char c = lexer_peek(lexer);
        
        if (c >= '0' && c <= '9') {
            value[i++] = lexer_advance(lexer);
        } else if (c == '.' && !has_dot) {
            has_dot = true;
            value[i++] = lexer_advance(lexer);
        } else {
            break;
        }
        
        if (i >= MAX_TOKEN_LEN - 1) break;
    }
    
    value[i] = '\0';
    
    Token *token = token_create(TOKEN_NUMBER, value, start_line, start_col);
    free(value);
    return token;
}

// Read an identifier or keyword
static Token *lexer_read_identifier(Lexer *lexer) {
    int start_line = lexer->line;
    int start_col = lexer->column;
    
    char *value = malloc(MAX_TOKEN_LEN);
    int i = 0;
    
    // Read UTF-8 characters
    while (lexer->pos < lexer->length) {
        unsigned char c = (unsigned char)lexer->source[lexer->pos];
        
        // Check if it's an Arabic character or continuation
        if (is_arabic_char(c) || 
            (c >= 0x80 && c <= 0xBF) ||
            (i > 0 && ((c >= '0' && c <= '9') || c == '_'))) {
            
            // Copy UTF-8 sequence
            int char_len = 1;
            if (c >= 0xC0) {
                if (c >= 0xFC) char_len = 6;
                else if (c >= 0xF8) char_len = 5;
                else if (c >= 0xF0) char_len = 4;
                else if (c >= 0xE0) char_len = 3;
                else if (c >= 0xC0) char_len = 2;
            }
            
            for (int j = 0; j < char_len && lexer->pos < lexer->length; j++) {
                value[i++] = lexer->source[lexer->pos++];
            }
            lexer->column++;
        } else {
            break;
        }
        
        if (i >= MAX_TOKEN_LEN - 4) break;
    }
    
    value[i] = '\0';
    
    // Check if it's a keyword
    TokenType type = check_arabic_keyword(value);
    
    Token *token = token_create(type, value, start_line, start_col);
    free(value);
    return token;
}

TokenType check_arabic_keyword(const char *word) {
    for (int i = 0; arabic_keywords[i].arabic != NULL; i++) {
        if (strcmp(word, arabic_keywords[i].arabic) == 0) {
            return arabic_keywords[i].type;
        }
    }
    return TOKEN_IDENTIFIER;
}

Token *lexer_next_token(Lexer *lexer) {
    lexer_skip_whitespace(lexer);
    
    int line = lexer->line;
    int col = lexer->column;
    char c = lexer_peek(lexer);
    
    if (c == '\0') {
        return token_create(TOKEN_EOF, NULL, line, col);
    }
    
    // String literal
    if (c == '"') {
        return lexer_read_string(lexer);
    }
    
    // Number
    if (c >= '0' && c <= '9') {
        return lexer_read_number(lexer);
    }
    
    // Identifier or keyword (Arabic or Latin)
    if (is_arabic((unsigned char)c) || 
        (c >= 'a' && c <= 'z') || 
        (c >= 'A' && c <= 'Z') || 
        c == '_') {
        return lexer_read_identifier(lexer);
    }
    
    // Operators and delimiters
    lexer_advance(lexer);
    
    switch (c) {
        case '+': return token_create(TOKEN_PLUS, "+", line, col);
        case '-': return token_create(TOKEN_MINUS, "-", line, col);
        case '*': return token_create(TOKEN_MULTIPLY, "*", line, col);
        case '/': return token_create(TOKEN_DIVIDE, "/", line, col);
        case '%': return token_create(TOKEN_MODULO, "%", line, col);
        case '(': return token_create(TOKEN_LPAREN, "(", line, col);
        case ')': return token_create(TOKEN_RPAREN, ")", line, col);
        case '{': return token_create(TOKEN_LBRACE, "{", line, col);
        case '}': return token_create(TOKEN_RBRACE, "}", line, col);
        case '[': return token_create(TOKEN_LBRACKET, "[", line, col);
        case ']': return token_create(TOKEN_RBRACKET, "]", line, col);
        case ':': return token_create(TOKEN_COLON, ":", line, col);
        case '.': return token_create(TOKEN_DOT, ".", line, col);
        
        case '=':
            if (lexer_peek(lexer) == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_EQUAL, "==", line, col);
            }
            return token_create(TOKEN_ASSIGN, "=", line, col);
            
        case '!':
            if (lexer_peek(lexer) == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_NOT_EQUAL, "!=", line, col);
            }
            break;
            
        case '<':
            if (lexer_peek(lexer) == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_LESS_EQUAL, "<=", line, col);
            }
            return token_create(TOKEN_LESS, "<", line, col);
            
        case '>':
            if (lexer_peek(lexer) == '=') {
                lexer_advance(lexer);
                return token_create(TOKEN_GREATER_EQUAL, ">=", line, col);
            }
            return token_create(TOKEN_GREATER, ">", line, col);
            
        case ',':
            return token_create(TOKEN_COMMA, ",", line, col);
            
        case '\n':
            return token_create(TOKEN_NEWLINE, "\n", line, col);
    }
    
    // Unknown character
    char unknown[2] = {c, '\0'};
    return token_create(TOKEN_ERROR, unknown, line, col);
}

const char *token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_NEWLINE: return "NEWLINE";
        case TOKEN_ERROR: return "ERROR";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_LET: return "LET";
        case TOKEN_CONST: return "CONST";
        case TOKEN_IS: return "IS";
        case TOKEN_WRITE: return "WRITE";
        case TOKEN_READ: return "READ";
        case TOKEN_IF: return "IF";
        case TOKEN_THEN: return "THEN";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_END: return "END";
        case TOKEN_FOR: return "FOR";
        case TOKEN_FROM: return "FROM";
        case TOKEN_TO: return "TO";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_FUNCTION: return "FUNCTION";
        case TOKEN_TAKES: return "TAKES";
        case TOKEN_AND: return "AND";
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_STRUCT: return "STRUCT";
        case TOKEN_DOT: return "DOT";
        case TOKEN_IMPORT: return "IMPORT";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_MODULO: return "MODULO";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_LBRACKET: return "LBRACKET";
        case TOKEN_RBRACKET: return "RBRACKET";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_COLON: return "COLON";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_NULL: return "NULL";
        default: return "UNKNOWN";
    }
}
