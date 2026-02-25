// ============================================
// Wisam Code Editor - Lexer
// محرر وسام - محلل الرموز
// ============================================

// Token Types
const TOKEN_TYPES = {
    // Keywords
    LET: 'LET',           // ليكن
    CONST: 'CONST',       // ثابت
    IF: 'IF',             // إذا
    THEN: 'THEN',         // إذن
    ELSE: 'ELSE',         // وإلا
    END: 'END',           // انتهى
    FOR: 'FOR',           // لكل
    FROM: 'FROM',         // من
    TO: 'TO',             // إلى
    FUNCTION: 'FUNCTION', // دالة
    RETURN: 'RETURN',     // أعد
    STRUCT: 'STRUCT',     // هيكل
    CLASS: 'CLASS',       // صنف
    IMPORT: 'IMPORT',     // استورد
    CREATE: 'CREATE',     // أنشئ
    AS: 'AS',             // باسم
    WHILE: 'WHILE',       // طالما
    BREAK: 'BREAK',       // توقف
    CONTINUE: 'CONTINUE', // استمر
    AND: 'AND',           // و
    OR: 'OR',             // أو
    NOT: 'NOT',           // ليس
    IN: 'IN',             // في
    OF: 'OF',             // من
    TRY: 'TRY',           // حاول
    CATCH: 'CATCH',       // امسك
    FINALLY: 'FINALLY',   // أخيراً
    THROW: 'THROW',       // ألقِ
    SWITCH: 'SWITCH',     // حسب
    CASE: 'CASE',         // حالة
    DEFAULT: 'DEFAULT',   // افتراضي
    DO: 'DO',             // نفذ
    UNTIL: 'UNTIL',       // حتى
    FOREACH: 'FOREACH',   // لكل_عنصر
    YIELD: 'YIELD',       // أنتج
    ASYNC: 'ASYNC',       // غير_متزامن
    AWAIT: 'AWAIT',       // انتظر
    PUBLIC: 'PUBLIC',     // عام
    PRIVATE: 'PRIVATE',   // خاص
    PROTECTED: 'PROTECTED', // محمي
    STATIC: 'STATIC',     // ثابت_الصنف
    EXTENDS: 'EXTENDS',   // يرث
    IMPLEMENTS: 'IMPLEMENTS', // ينفذ
    NEW: 'NEW',           // جديد
    THIS: 'THIS',         // هذا
    SUPER: 'SUPER',       // أب
    NULL: 'NULL',         // فارغ
    TRUE: 'TRUE',         // صحيح
    FALSE: 'FALSE',       // خطأ
    
    // Data Types
    NUMBER: 'NUMBER',
    STRING: 'STRING',
    BOOLEAN: 'BOOLEAN',
    ARRAY: 'ARRAY',
    OBJECT: 'OBJECT',
    
    // Identifiers and Symbols
    IDENTIFIER: 'IDENTIFIER',
    ASSIGN: 'ASSIGN',           // =
    PLUS: 'PLUS',               // +
    MINUS: 'MINUS',             // -
    MULTIPLY: 'MULTIPLY',       // *
    DIVIDE: 'DIVIDE',           // /
    MODULO: 'MODULO',           // %
    POWER: 'POWER',             // ^
    EQUAL: 'EQUAL',             // ==
    NOT_EQUAL: 'NOT_EQUAL',     // !=
    GREATER: 'GREATER',         // >
    LESS: 'LESS',               // <
    GREATER_EQ: 'GREATER_EQ',   // >=
    LESS_EQ: 'LESS_EQ',         // <=
    LPAREN: 'LPAREN',           // (
    RPAREN: 'RPAREN',           // )
    LBRACE: 'LBRACE',           // {
    RBRACE: 'RBRACE',           // }
    LBRACKET: 'LBRACKET',       // [
    RBRACKET: 'RBRACKET',       // ]
    COMMA: 'COMMA',             // ،
    DOT: 'DOT',                 // .
    COLON: 'COLON',             // :
    SEMICOLON: 'SEMICOLON',     // ؛
    NEWLINE: 'NEWLINE',
    EOF: 'EOF',
    PRINT: 'PRINT',             // اكتب
    INPUT: 'INPUT',             // ادخل
    TYPEOF: 'TYPEOF',           // نوع
    SIZEOF: 'SIZEOF',           // حجم
    
    // AI Keywords
    MIND: 'MIND',               // عقل
    SYSTEM: 'SYSTEM',           // منظومة
    LEARN: 'LEARN',             // تعلّم
    ASK: 'ASK',                 // اسأل
    RESPONSE: 'RESPONSE',       // استجب
    SAVE: 'SAVE',               // احفظ
    LOAD: 'LOAD',               // حمّل
    GPU: 'GPU',
    TRAIN: 'TRAIN',             // درّب
    NEURAL: 'NEURAL',           // شبكة_عصبية
    LAYERS: 'LAYERS',           // طبقات
    LEARNING_RATE: 'LEARNING_RATE', // معدل_تعلم
    MODEL: 'MODEL',             // نموذج
    PREDICT: 'PREDICT',         // تنبأ
    DATASET: 'DATASET',         // مجموعة_بيانات
    
    // Additional Operators
    INCREMENT: 'INCREMENT',     // ++
    DECREMENT: 'DECREMENT',     // --
    PLUS_ASSIGN: 'PLUS_ASSIGN', // +=
    MINUS_ASSIGN: 'MINUS_ASSIGN', // -=
    MUL_ASSIGN: 'MUL_ASSIGN',   // *=
    DIV_ASSIGN: 'DIV_ASSIGN',   // /=
    MOD_ASSIGN: 'MOD_ASSIGN',   // %=
    BIT_AND: 'BIT_AND',         // &
    BIT_OR: 'BIT_OR',           // |
    BIT_XOR: 'BIT_XOR',         // ^
    BIT_NOT: 'BIT_NOT',         // ~
    SHIFT_LEFT: 'SHIFT_LEFT',   // <<
    SHIFT_RIGHT: 'SHIFT_RIGHT'  // >>
};

// Arabic Keywords Mapping
const ARABIC_KEYWORDS = {
    'ليكن': TOKEN_TYPES.LET,
    'ثابت': TOKEN_TYPES.CONST,
    'إذا': TOKEN_TYPES.IF,
    'اذا': TOKEN_TYPES.IF,
    'إذن': TOKEN_TYPES.THEN,
    'اذن': TOKEN_TYPES.THEN,
    'وإلا': TOKEN_TYPES.ELSE,
    'وانلا': TOKEN_TYPES.ELSE,
    'انتهى': TOKEN_TYPES.END,
    'انتهي': TOKEN_TYPES.END,
    'لكل': TOKEN_TYPES.FOR,
    'من': TOKEN_TYPES.FROM,
    'إلى': TOKEN_TYPES.TO,
    'الى': TOKEN_TYPES.TO,
    'دالة': TOKEN_TYPES.FUNCTION,
    'أعد': TOKEN_TYPES.RETURN,
    'اعد': TOKEN_TYPES.RETURN,
    'هيكل': TOKEN_TYPES.STRUCT,
    'صنف': TOKEN_TYPES.CLASS,
    'استورد': TOKEN_TYPES.IMPORT,
    'أنشئ': TOKEN_TYPES.CREATE,
    'انشئ': TOKEN_TYPES.CREATE,
    'باسم': TOKEN_TYPES.AS,
    'طالما': TOKEN_TYPES.WHILE,
    'توقف': TOKEN_TYPES.BREAK,
    'استمر': TOKEN_TYPES.CONTINUE,
    'و': TOKEN_TYPES.AND,
    'أو': TOKEN_TYPES.OR,
    'او': TOKEN_TYPES.OR,
    'ليس': TOKEN_TYPES.NOT,
    'في': TOKEN_TYPES.IN,
    'حاول': TOKEN_TYPES.TRY,
    'امسك': TOKEN_TYPES.CATCH,
    'أخيراً': TOKEN_TYPES.FINALLY,
    'اخيراً': TOKEN_TYPES.FINALLY,
    'ألقِ': TOKEN_TYPES.THROW,
    'القي': TOKEN_TYPES.THROW,
    'حسب': TOKEN_TYPES.SWITCH,
    'حالة': TOKEN_TYPES.CASE,
    'افتراضي': TOKEN_TYPES.DEFAULT,
    'نفذ': TOKEN_TYPES.DO,
    'حتى': TOKEN_TYPES.UNTIL,
    'لكل_عنصر': TOKEN_TYPES.FOREACH,
    'أنتج': TOKEN_TYPES.YIELD,
    'انتج': TOKEN_TYPES.YIELD,
    'غير_متزامن': TOKEN_TYPES.ASYNC,
    'انتظر': TOKEN_TYPES.AWAIT,
    'عام': TOKEN_TYPES.PUBLIC,
    'خاص': TOKEN_TYPES.PRIVATE,
    'محمي': TOKEN_TYPES.PROTECTED,
    'ثابت_الصنف': TOKEN_TYPES.STATIC,
    'يرث': TOKEN_TYPES.EXTENDS,
    'ينفذ': TOKEN_TYPES.IMPLEMENTS,
    'جديد': TOKEN_TYPES.NEW,
    'هذا': TOKEN_TYPES.THIS,
    'أب': TOKEN_TYPES.SUPER,
    'اب': TOKEN_TYPES.SUPER,
    'فارغ': TOKEN_TYPES.NULL,
    'صحيح': TOKEN_TYPES.TRUE,
    'خطأ': TOKEN_TYPES.FALSE,
    'اكتب': TOKEN_TYPES.PRINT,
    'أكتب': TOKEN_TYPES.PRINT,
    'ادخل': TOKEN_TYPES.INPUT,
    'أدخل': TOKEN_TYPES.INPUT,
    'نوع': TOKEN_TYPES.TYPEOF,
    'حجم': TOKEN_TYPES.SIZEOF,
    'عقل': TOKEN_TYPES.MIND,
    'منظومة': TOKEN_TYPES.SYSTEM,
    'تعلّم': TOKEN_TYPES.LEARN,
    'تعلم': TOKEN_TYPES.LEARN,
    'اسأل': TOKEN_TYPES.ASK,
    'استجب': TOKEN_TYPES.RESPONSE,
    'احفظ': TOKEN_TYPES.SAVE,
    'حمّل': TOKEN_TYPES.LOAD,
    'حمل': TOKEN_TYPES.LOAD,
    'درّب': TOKEN_TYPES.TRAIN,
    'درب': TOKEN_TYPES.TRAIN,
    'شبكة_عصبية': TOKEN_TYPES.NEURAL,
    'طبقات': TOKEN_TYPES.LAYERS,
    'معدل_تعلم': TOKEN_TYPES.LEARNING_RATE,
    'نموذج': TOKEN_TYPES.MODEL,
    'تنبأ': TOKEN_TYPES.PREDICT,
    'مجموعة_بيانات': TOKEN_TYPES.DATASET
};

// Token class
class Token {
    constructor(type, value, line = 1, column = 1) {
        this.type = type;
        this.value = value;
        this.line = line;
        this.column = column;
    }
    
    toString() {
        return `Token(${this.type}, "${this.value}", ${this.line}:${this.column})`;
    }
}

// Lexer class
class Lexer {
    constructor(source) {
        this.source = source;
        this.position = 0;
        this.line = 1;
        this.column = 1;
        this.length = source.length;
        this.errors = [];
    }
    
    // Get current character
    peek() {
        if (this.position >= this.length) return '\0';
        return this.source[this.position];
    }
    
    // Get next character
    peekNext() {
        if (this.position + 1 >= this.length) return '\0';
        return this.source[this.position + 1];
    }
    
    // Advance to next character
    advance() {
        if (this.position >= this.length) return '\0';
        const char = this.source[this.position];
        this.position++;
        if (char === '\n') {
            this.line++;
            this.column = 1;
        } else {
            this.column++;
        }
        return char;
    }
    
    // Skip whitespace
    skipWhitespace() {
        while (this.peek() === ' ' || this.peek() === '\t' || this.peek() === '\r') {
            this.advance();
        }
    }
    
    // Skip comments
    skipComment() {
        if (this.peek() === '#') {
            while (this.peek() !== '\n' && this.peek() !== '\0') {
                this.advance();
            }
        }
    }
    
    // Check if character is Arabic
    isArabic(char) {
        if (!char) return false;
        const code = char.charCodeAt(0);
        return (code >= 0x0600 && code <= 0x06FF) ||   // Arabic
               (code >= 0x0750 && code <= 0x077F) ||   // Arabic Supplement
               (code >= 0xFB50 && code <= 0xFDFF) ||   // Arabic Presentation Forms-A
               (code >= 0xFE70 && code <= 0xFEFF);     // Arabic Presentation Forms-B
    }
    
    // Check if character is a digit
    isDigit(char) {
        return char >= '0' && char <= '9';
    }
    
    // Check if character is a letter (English or Arabic)
    isLetter(char) {
        if (!char) return false;
        const code = char.charCodeAt(0);
        return (code >= 'a'.charCodeAt(0) && code <= 'z'.charCodeAt(0)) ||
               (code >= 'A'.charCodeAt(0) && code <= 'Z'.charCodeAt(0)) ||
               this.isArabic(char);
    }
    
    // Read string
    readString() {
        const startLine = this.line;
        const startColumn = this.column;
        let value = '';
        
        const quote = this.advance(); // " or '
        
        while (this.peek() !== quote && this.peek() !== '\0') {
            if (this.peek() === '\\') {
                this.advance();
                const escaped = this.advance();
                switch (escaped) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    case 'r': value += '\r'; break;
                    case '\\': value += '\\'; break;
                    case '"': value += '"'; break;
                    case "'": value += "'"; break;
                    default: value += escaped;
                }
            } else {
                value += this.advance();
            }
        }
        
        this.advance(); // closing quote
        return new Token(TOKEN_TYPES.STRING, value, startLine, startColumn);
    }
    
    // Read number
    readNumber() {
        const startLine = this.line;
        const startColumn = this.column;
        let value = '';
        let hasDot = false;
        
        while (this.isDigit(this.peek()) || this.peek() === '.') {
            if (this.peek() === '.') {
                if (hasDot) break;
                hasDot = true;
            }
            value += this.advance();
        }
        
        return new Token(TOKEN_TYPES.NUMBER, value, startLine, startColumn);
    }
    
    // Read identifier or keyword
    readIdentifier() {
        const startLine = this.line;
        const startColumn = this.column;
        let value = '';
        
        while (this.isLetter(this.peek()) || this.isDigit(this.peek()) || this.peek() === '_') {
            value += this.advance();
        }
        
        // Check if it's a keyword
        const keywordType = ARABIC_KEYWORDS[value];
        if (keywordType) {
            return new Token(keywordType, value, startLine, startColumn);
        }
        
        return new Token(TOKEN_TYPES.IDENTIFIER, value, startLine, startColumn);
    }
    
    // Tokenize the source code
    tokenize() {
        const tokens = [];
        
        while (this.position < this.length) {
            this.skipWhitespace();
            this.skipComment();
            this.skipWhitespace();
            
            if (this.position >= this.length) break;
            
            const char = this.peek();
            const line = this.line;
            const column = this.column;
            
            // Newline
            if (char === '\n') {
                tokens.push(new Token(TOKEN_TYPES.NEWLINE, '\n', line, column));
                this.advance();
                continue;
            }
            
            // String
            if (char === '"' || char === "'") {
                tokens.push(this.readString());
                continue;
            }
            
            // Number
            if (this.isDigit(char)) {
                tokens.push(this.readNumber());
                continue;
            }
            
            // Identifier or keyword
            if (this.isLetter(char)) {
                tokens.push(this.readIdentifier());
                continue;
            }
            
            // Operators and symbols
            switch (char) {
                case '=':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.EQUAL, '==', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.ASSIGN, this.advance(), line, column));
                    }
                    break;
                    
                case '!':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.NOT_EQUAL, '!=', line, column));
                    } else {
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.NOT, '!', line, column));
                    }
                    break;
                    
                case '<':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.LESS_EQ, '<=', line, column));
                    } else if (this.peekNext() === '<') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.SHIFT_LEFT, '<<', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.LESS, this.advance(), line, column));
                    }
                    break;
                    
                case '>':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.GREATER_EQ, '>=', line, column));
                    } else if (this.peekNext() === '>') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.SHIFT_RIGHT, '>>', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.GREATER, this.advance(), line, column));
                    }
                    break;
                    
                case '+':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.PLUS_ASSIGN, '+=', line, column));
                    } else if (this.peekNext() === '+') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.INCREMENT, '++', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.PLUS, this.advance(), line, column));
                    }
                    break;
                    
                case '-':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.MINUS_ASSIGN, '-=', line, column));
                    } else if (this.peekNext() === '-') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.DECREMENT, '--', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.MINUS, this.advance(), line, column));
                    }
                    break;
                    
                case '*':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.MUL_ASSIGN, '*=', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.MULTIPLY, this.advance(), line, column));
                    }
                    break;
                    
                case '/':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.DIV_ASSIGN, '/=', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.DIVIDE, this.advance(), line, column));
                    }
                    break;
                    
                case '%':
                    if (this.peekNext() === '=') {
                        this.advance();
                        this.advance();
                        tokens.push(new Token(TOKEN_TYPES.MOD_ASSIGN, '%=', line, column));
                    } else {
                        tokens.push(new Token(TOKEN_TYPES.MODULO, this.advance(), line, column));
                    }
                    break;
                    
                case '^':
                    tokens.push(new Token(TOKEN_TYPES.POWER, this.advance(), line, column));
                    break;
                    
                case '(':
                    tokens.push(new Token(TOKEN_TYPES.LPAREN, this.advance(), line, column));
                    break;
                    
                case ')':
                    tokens.push(new Token(TOKEN_TYPES.RPAREN, this.advance(), line, column));
                    break;
                    
                case '{':
                    tokens.push(new Token(TOKEN_TYPES.LBRACE, this.advance(), line, column));
                    break;
                    
                case '}':
                    tokens.push(new Token(TOKEN_TYPES.RBRACE, this.advance(), line, column));
                    break;
                    
                case '[':
                    tokens.push(new Token(TOKEN_TYPES.LBRACKET, this.advance(), line, column));
                    break;
                    
                case ']':
                    tokens.push(new Token(TOKEN_TYPES.RBRACKET, this.advance(), line, column));
                    break;
                    
                case ',':
                case '،':
                    tokens.push(new Token(TOKEN_TYPES.COMMA, ',', line, column));
                    this.advance();
                    break;
                    
                case '.':
                    tokens.push(new Token(TOKEN_TYPES.DOT, this.advance(), line, column));
                    break;
                    
                case ':':
                    tokens.push(new Token(TOKEN_TYPES.COLON, this.advance(), line, column));
                    break;
                    
                case ';':
                case '؛':
                    tokens.push(new Token(TOKEN_TYPES.SEMICOLON, ';', line, column));
                    this.advance();
                    break;
                    
                case '&':
                    tokens.push(new Token(TOKEN_TYPES.BIT_AND, this.advance(), line, column));
                    break;
                    
                case '|':
                    tokens.push(new Token(TOKEN_TYPES.BIT_OR, this.advance(), line, column));
                    break;
                    
                case '~':
                    tokens.push(new Token(TOKEN_TYPES.BIT_NOT, this.advance(), line, column));
                    break;
                    
                default:
                    // Unknown character - skip it
                    this.errors.push({
                        message: `حرف غير معروف: "${char}"`,
                        line: line,
                        column: column
                    });
                    this.advance();
                    break;
            }
        }
        
        // Add EOF token
        tokens.push(new Token(TOKEN_TYPES.EOF, '', this.line, this.column));
        
        return tokens;
    }
}

// Export for use in other modules
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { Lexer, Token, TOKEN_TYPES, ARABIC_KEYWORDS };
}
