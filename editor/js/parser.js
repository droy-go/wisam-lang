// ============================================
// Wisam Code Editor - Parser
// محرر وسام - محلل النحو
// ============================================

// AST Node Types
const AST_NODE_TYPES = {
    PROGRAM: 'PROGRAM',
    LET: 'LET',
    CONST: 'CONST',
    ASSIGN: 'ASSIGN',
    IF: 'IF',
    FOR: 'FOR',
    WHILE: 'WHILE',
    DO_WHILE: 'DO_WHILE',
    FOREACH: 'FOREACH',
    FUNCTION_DEF: 'FUNCTION_DEF',
    FUNCTION_CALL: 'FUNCTION_CALL',
    RETURN: 'RETURN',
    STRUCT_DEF: 'STRUCT_DEF',
    STRUCT_ACCESS: 'STRUCT_ACCESS',
    CLASS_DEF: 'CLASS_DEF',
    METHOD_CALL: 'METHOD_CALL',
    IMPORT: 'IMPORT',
    PRINT: 'PRINT',
    INPUT: 'INPUT',
    BINARY_OP: 'BINARY_OP',
    UNARY_OP: 'UNARY_OP',
    LITERAL: 'LITERAL',
    IDENTIFIER: 'IDENTIFIER',
    ARRAY: 'ARRAY',
    ARRAY_ACCESS: 'ARRAY_ACCESS',
    BREAK: 'BREAK',
    CONTINUE: 'CONTINUE',
    TRY_CATCH: 'TRY_CATCH',
    THROW: 'THROW'
};

// AST Node class
class ASTNode {
    constructor(type, props = {}) {
        this.type = type;
        Object.assign(this, props);
    }
}

// Parser class
class Parser {
    constructor(tokens) {
        this.tokens = tokens;
        this.position = 0;
        this.errors = [];
    }
    
    // Get current token
    peek() {
        if (this.position >= this.tokens.length) {
            return this.tokens[this.tokens.length - 1];
        }
        return this.tokens[this.position];
    }
    
    // Get next token
    peekNext() {
        if (this.position + 1 >= this.tokens.length) {
            return this.tokens[this.tokens.length - 1];
        }
        return this.tokens[this.position + 1];
    }
    
    // Advance to next token
    advance() {
        if (this.position >= this.tokens.length) {
            return this.tokens[this.tokens.length - 1];
        }
        return this.tokens[this.position++];
    }
    
    // Check current token type
    check(type) {
        return this.peek().type === type;
    }
    
    // Match and advance
    match(type) {
        if (this.check(type)) {
            this.advance();
            return true;
        }
        return false;
    }
    
    // Consume expected token
    consume(type, message) {
        if (this.check(type)) {
            return this.advance();
        }
        this.errors.push({
            message: message,
            line: this.peek().line,
            column: this.peek().column
        });
        return this.peek();
    }
    
    // Skip newlines
    skipNewlines() {
        while (this.match(TOKEN_TYPES.NEWLINE));
    }
    
    // Parse the entire program
    parse() {
        const statements = [];
        
        while (!this.check(TOKEN_TYPES.EOF)) {
            this.skipNewlines();
            if (this.check(TOKEN_TYPES.EOF)) break;
            
            const stmt = this.parseStatement();
            if (stmt) {
                statements.push(stmt);
            }
            
            this.skipNewlines();
        }
        
        return new ASTNode(AST_NODE_TYPES.PROGRAM, { statements });
    }
    
    // Parse a statement
    parseStatement() {
        this.skipNewlines();
        
        const token = this.peek();
        
        switch (token.type) {
            case TOKEN_TYPES.PRINT:
                return this.parsePrint();
            case TOKEN_TYPES.INPUT:
                return this.parseInput();
            case TOKEN_TYPES.LET:
                return this.parseLet();
            case TOKEN_TYPES.CONST:
                return this.parseConst();
            case TOKEN_TYPES.IF:
                return this.parseIf();
            case TOKEN_TYPES.FOR:
                return this.parseFor();
            case TOKEN_TYPES.WHILE:
                return this.parseWhile();
            case TOKEN_TYPES.FUNCTION:
                return this.parseFunctionDef();
            case TOKEN_TYPES.RETURN:
                return this.parseReturn();
            case TOKEN_TYPES.BREAK:
                return this.parseBreak();
            case TOKEN_TYPES.CONTINUE:
                return this.parseContinue();
            case TOKEN_TYPES.IDENTIFIER:
                // Check for assignment or function call
                if (this.peekNext().type === TOKEN_TYPES.ASSIGN) {
                    return this.parseAssign();
                }
                return this.parseExpression();
            default:
                return this.parseExpression();
        }
    }
    
    // Parse print statement
    parsePrint() {
        this.advance(); // consume 'اكتب'
        this.skipNewlines();
        const expression = this.parseExpression();
        return new ASTNode(AST_NODE_TYPES.PRINT, { expression });
    }
    
    // Parse input statement
    parseInput() {
        this.advance(); // consume 'ادخل'
        this.skipNewlines();
        
        let prompt = null;
        if (this.check(TOKEN_TYPES.STRING)) {
            prompt = this.advance().value;
        }
        
        return new ASTNode(AST_NODE_TYPES.INPUT, { prompt });
    }
    
    // Parse let statement
    parseLet() {
        this.advance(); // consume 'ليكن'
        
        const name = this.consume(TOKEN_TYPES.IDENTIFIER, 'متوقع اسم المتغير بعد "ليكن"').value;
        this.consume(TOKEN_TYPES.ASSIGN, 'متوقع "=" بعد اسم المتغير');
        this.skipNewlines();
        
        const value = this.parseExpression();
        return new ASTNode(AST_NODE_TYPES.LET, { name, value });
    }
    
    // Parse const statement
    parseConst() {
        this.advance(); // consume 'ثابت'
        
        const name = this.consume(TOKEN_TYPES.IDENTIFIER, 'متوقع اسم الثابت بعد "ثابت"').value;
        this.consume(TOKEN_TYPES.ASSIGN, 'متوقع "=" بعد اسم الثابت');
        this.skipNewlines();
        
        const value = this.parseExpression();
        return new ASTNode(AST_NODE_TYPES.CONST, { name, value });
    }
    
    // Parse assignment
    parseAssign() {
        const name = this.advance().value;
        this.advance(); // consume '='
        this.skipNewlines();
        
        const value = this.parseExpression();
        return new ASTNode(AST_NODE_TYPES.ASSIGN, { name, value });
    }
    
    // Parse if statement
    parseIf() {
        this.advance(); // consume 'إذا'
        
        const condition = this.parseExpression();
        this.consume(TOKEN_TYPES.THEN, 'متوقع "إذن" بعد الشرط');
        this.skipNewlines();
        
        const thenBranch = [];
        while (!this.check(TOKEN_TYPES.ELSE) && 
               !this.check(TOKEN_TYPES.END) && 
               !this.check(TOKEN_TYPES.EOF)) {
            this.skipNewlines();
            if (this.check(TOKEN_TYPES.ELSE) || this.check(TOKEN_TYPES.END)) break;
            const stmt = this.parseStatement();
            if (stmt) thenBranch.push(stmt);
            this.skipNewlines();
        }
        
        let elseBranch = null;
        if (this.match(TOKEN_TYPES.ELSE)) {
            this.skipNewlines();
            elseBranch = [];
            while (!this.check(TOKEN_TYPES.END) && !this.check(TOKEN_TYPES.EOF)) {
                this.skipNewlines();
                if (this.check(TOKEN_TYPES.END)) break;
                const stmt = this.parseStatement();
                if (stmt) elseBranch.push(stmt);
                this.skipNewlines();
            }
        }
        
        this.consume(TOKEN_TYPES.END, 'متوقع "انتهى" في نهاية جملة "إذا"');
        
        return new ASTNode(AST_NODE_TYPES.IF, { 
            condition, 
            thenBranch: new ASTNode(AST_NODE_TYPES.PROGRAM, { statements: thenBranch }),
            elseBranch: elseBranch ? new ASTNode(AST_NODE_TYPES.PROGRAM, { statements: elseBranch }) : null
        });
    }
    
    // Parse for loop
    parseFor() {
        this.advance(); // consume 'لكل'
        
        const varName = this.consume(TOKEN_TYPES.IDENTIFIER, 'متوقع اسم المتغير بعد "لكل"').value;
        this.consume(TOKEN_TYPES.FROM, 'متوقع "من" بعد اسم المتغير');
        
        const start = this.parseExpression();
        this.consume(TOKEN_TYPES.TO, 'متوقع "إلى" بعد قيمة البداية');
        
        const end = this.parseExpression();
        this.skipNewlines();
        
        const body = [];
        while (!this.check(TOKEN_TYPES.END) && !this.check(TOKEN_TYPES.EOF)) {
            this.skipNewlines();
            if (this.check(TOKEN_TYPES.END)) break;
            const stmt = this.parseStatement();
            if (stmt) body.push(stmt);
            this.skipNewlines();
        }
        
        this.consume(TOKEN_TYPES.END, 'متوقع "انتهى" في نهاية حلقة "لكل"');
        
        return new ASTNode(AST_NODE_TYPES.FOR, { 
            varName, 
            start, 
            end, 
            body: new ASTNode(AST_NODE_TYPES.PROGRAM, { statements: body })
        });
    }
    
    // Parse while loop
    parseWhile() {
        this.advance(); // consume 'طالما'
        
        const condition = this.parseExpression();
        this.skipNewlines();
        
        const body = [];
        while (!this.check(TOKEN_TYPES.END) && !this.check(TOKEN_TYPES.EOF)) {
            this.skipNewlines();
            if (this.check(TOKEN_TYPES.END)) break;
            const stmt = this.parseStatement();
            if (stmt) body.push(stmt);
            this.skipNewlines();
        }
        
        this.consume(TOKEN_TYPES.END, 'متوقع "انتهى" في نهاية حلقة "طالما"');
        
        return new ASTNode(AST_NODE_TYPES.WHILE, { 
            condition, 
            body: new ASTNode(AST_NODE_TYPES.PROGRAM, { statements: body })
        });
    }
    
    // Parse function definition
    parseFunctionDef() {
        this.advance(); // consume 'دالة'
        
        const name = this.consume(TOKEN_TYPES.IDENTIFIER, 'متوقع اسم الدالة بعد "دالة"').value;
        
        const params = [];
        // Read parameters until newline or end
        while (!this.check(TOKEN_TYPES.NEWLINE) && 
               !this.check(TOKEN_TYPES.EOF) &&
               this.check(TOKEN_TYPES.IDENTIFIER)) {
            params.push(this.advance().value);
            
            if (this.check(TOKEN_TYPES.COMMA) || this.check(TOKEN_TYPES.IDENTIFIER)) {
                this.advance();
            } else {
                break;
            }
        }
        
        this.skipNewlines();
        
        const body = [];
        while (!this.check(TOKEN_TYPES.END) && !this.check(TOKEN_TYPES.EOF)) {
            this.skipNewlines();
            if (this.check(TOKEN_TYPES.END)) break;
            const stmt = this.parseStatement();
            if (stmt) body.push(stmt);
            this.skipNewlines();
        }
        
        this.consume(TOKEN_TYPES.END, 'متوقع "انتهى" في نهاية تعريف الدالة');
        
        return new ASTNode(AST_NODE_TYPES.FUNCTION_DEF, { 
            name, 
            params, 
            body: new ASTNode(AST_NODE_TYPES.PROGRAM, { statements: body })
        });
    }
    
    // Parse return statement
    parseReturn() {
        this.advance(); // consume 'أعد'
        
        let value = null;
        if (!this.check(TOKEN_TYPES.NEWLINE) && !this.check(TOKEN_TYPES.END)) {
            value = this.parseExpression();
        }
        
        return new ASTNode(AST_NODE_TYPES.RETURN, { value });
    }
    
    // Parse break statement
    parseBreak() {
        this.advance(); // consume 'توقف'
        return new ASTNode(AST_NODE_TYPES.BREAK);
    }
    
    // Parse continue statement
    parseContinue() {
        this.advance(); // consume 'استمر'
        return new ASTNode(AST_NODE_TYPES.CONTINUE);
    }
    
    // Parse expressions
    parseExpression() {
        return this.parseOr();
    }
    
    parseOr() {
        let left = this.parseAnd();
        
        while (this.match(TOKEN_TYPES.OR)) {
            const right = this.parseAnd();
            left = new ASTNode(AST_NODE_TYPES.BINARY_OP, { 
                op: TOKEN_TYPES.OR, 
                left, 
                right 
            });
        }
        
        return left;
    }
    
    parseAnd() {
        let left = this.parseEquality();
        
        while (this.match(TOKEN_TYPES.AND)) {
            const right = this.parseEquality();
            left = new ASTNode(AST_NODE_TYPES.BINARY_OP, { 
                op: TOKEN_TYPES.AND, 
                left, 
                right 
            });
        }
        
        return left;
    }
    
    parseEquality() {
        let left = this.parseComparison();
        
        while (this.match(TOKEN_TYPES.EQUAL) || this.match(TOKEN_TYPES.NOT_EQUAL)) {
            const op = this.tokens[this.position - 1].type;
            const right = this.parseComparison();
            left = new ASTNode(AST_NODE_TYPES.BINARY_OP, { op, left, right });
        }
        
        return left;
    }
    
    parseComparison() {
        let left = this.parseAdditive();
        
        while (this.match(TOKEN_TYPES.GREATER) || 
               this.match(TOKEN_TYPES.LESS) || 
               this.match(TOKEN_TYPES.GREATER_EQ) || 
               this.match(TOKEN_TYPES.LESS_EQ)) {
            const op = this.tokens[this.position - 1].type;
            const right = this.parseAdditive();
            left = new ASTNode(AST_NODE_TYPES.BINARY_OP, { op, left, right });
        }
        
        return left;
    }
    
    parseAdditive() {
        let left = this.parseMultiplicative();
        
        while (this.match(TOKEN_TYPES.PLUS) || this.match(TOKEN_TYPES.MINUS)) {
            const op = this.tokens[this.position - 1].type;
            const right = this.parseMultiplicative();
            left = new ASTNode(AST_NODE_TYPES.BINARY_OP, { op, left, right });
        }
        
        return left;
    }
    
    parseMultiplicative() {
        let left = this.parseUnary();
        
        while (this.match(TOKEN_TYPES.MULTIPLY) || 
               this.match(TOKEN_TYPES.DIVIDE) || 
               this.match(TOKEN_TYPES.MODULO) ||
               this.match(TOKEN_TYPES.POWER)) {
            const op = this.tokens[this.position - 1].type;
            const right = this.parseUnary();
            left = new ASTNode(AST_NODE_TYPES.BINARY_OP, { op, left, right });
        }
        
        return left;
    }
    
    parseUnary() {
        if (this.match(TOKEN_TYPES.MINUS) || this.match(TOKEN_TYPES.NOT)) {
            const op = this.tokens[this.position - 1].type;
            const operand = this.parseUnary();
            return new ASTNode(AST_NODE_TYPES.UNARY_OP, { op, operand });
        }
        
        return this.parsePrimary();
    }
    
    parsePrimary() {
        this.skipNewlines();
        
        const token = this.peek();
        
        switch (token.type) {
            case TOKEN_TYPES.NUMBER:
                this.advance();
                return new ASTNode(AST_NODE_TYPES.LITERAL, { 
                    value: parseFloat(token.value),
                    valueType: 'number'
                });
                
            case TOKEN_TYPES.STRING:
                this.advance();
                return new ASTNode(AST_NODE_TYPES.LITERAL, { 
                    value: token.value,
                    valueType: 'string'
                });
                
            case TOKEN_TYPES.TRUE:
                this.advance();
                return new ASTNode(AST_NODE_TYPES.LITERAL, { 
                    value: true,
                    valueType: 'boolean'
                });
                
            case TOKEN_TYPES.FALSE:
                this.advance();
                return new ASTNode(AST_NODE_TYPES.LITERAL, { 
                    value: false,
                    valueType: 'boolean'
                });
                
            case TOKEN_TYPES.NULL:
                this.advance();
                return new ASTNode(AST_NODE_TYPES.LITERAL, { 
                    value: null,
                    valueType: 'null'
                });
                
            case TOKEN_TYPES.IDENTIFIER:
                this.advance();
                // Check for function call
                if (this.check(TOKEN_TYPES.LPAREN) || 
                    (!this.check(TOKEN_TYPES.NEWLINE) && 
                     !this.check(TOKEN_TYPES.EOF) &&
                     !this.check(TOKEN_TYPES.END))) {
                    return this.parseFunctionCall(token.value);
                }
                return new ASTNode(AST_NODE_TYPES.IDENTIFIER, { name: token.value });
                
            case TOKEN_TYPES.LPAREN:
                this.advance();
                const expr = this.parseExpression();
                this.consume(TOKEN_TYPES.RPAREN, 'متوقع ")"');
                return expr;
                
            case TOKEN_TYPES.LBRACKET:
                return this.parseArray();
                
            default:
                this.errors.push({
                    message: `تعبير غير متوقع: ${token.value}`,
                    line: token.line,
                    column: token.column
                });
                this.advance();
                return new ASTNode(AST_NODE_TYPES.LITERAL, { value: null });
        }
    }
    
    // Parse function call
    parseFunctionCall(name) {
        const args = [];
        
        // Read arguments
        while (!this.check(TOKEN_TYPES.NEWLINE) && 
               !this.check(TOKEN_TYPES.EOF) &&
               !this.check(TOKEN_TYPES.END) &&
               !this.check(TOKEN_TYPES.RPAREN)) {
            args.push(this.parseExpression());
            
            if (this.check(TOKEN_TYPES.COMMA) || this.check(TOKEN_TYPES.IDENTIFIER)) {
                this.advance();
            } else {
                break;
            }
        }
        
        return new ASTNode(AST_NODE_TYPES.FUNCTION_CALL, { name, args });
    }
    
    // Parse array literal
    parseArray() {
        this.consume(TOKEN_TYPES.LBRACKET, 'متوقع "["');
        
        const elements = [];
        
        while (!this.check(TOKEN_TYPES.RBRACKET) && !this.check(TOKEN_TYPES.EOF)) {
            this.skipNewlines();
            elements.push(this.parseExpression());
            this.skipNewlines();
            
            if (this.match(TOKEN_TYPES.COMMA)) {
                continue;
            } else {
                break;
            }
        }
        
        this.consume(TOKEN_TYPES.RBRACKET, 'متوقع "]" في نهاية المصفوفة');
        
        return new ASTNode(AST_NODE_TYPES.ARRAY, { elements });
    }
    
    // Format AST for display
    static formatAST(node, indent = 0) {
        const spaces = '  '.repeat(indent);
        let result = '';
        
        switch (node.type) {
            case AST_NODE_TYPES.PROGRAM:
                result += `${spaces}📦 برنامج (${node.statements.length} عبارة)\n`;
                for (const stmt of node.statements) {
                    result += Parser.formatAST(stmt, indent + 1);
                }
                break;
                
            case AST_NODE_TYPES.LET:
                result += `${spaces}📌 تعريف: ${node.name}\n`;
                result += Parser.formatAST(node.value, indent + 1);
                break;
                
            case AST_NODE_TYPES.CONST:
                result += `${spaces}🔒 ثابت: ${node.name}\n`;
                result += Parser.formatAST(node.value, indent + 1);
                break;
                
            case AST_NODE_TYPES.ASSIGN:
                result += `${spaces}✏️ تعيين: ${node.name}\n`;
                result += Parser.formatAST(node.value, indent + 1);
                break;
                
            case AST_NODE_TYPES.IF:
                result += `${spaces}🔀 إذا\n`;
                result += Parser.formatAST(node.condition, indent + 1);
                result += `${spaces}  ⬇️ إذن\n`;
                result += Parser.formatAST(node.thenBranch, indent + 2);
                if (node.elseBranch) {
                    result += `${spaces}  ⬇️ وإلا\n`;
                    result += Parser.formatAST(node.elseBranch, indent + 2);
                }
                break;
                
            case AST_NODE_TYPES.FOR:
                result += `${spaces}🔁 لكل: ${node.varName}\n`;
                result += Parser.formatAST(node.start, indent + 1);
                result += Parser.formatAST(node.end, indent + 1);
                result += Parser.formatAST(node.body, indent + 1);
                break;
                
            case AST_NODE_TYPES.WHILE:
                result += `${spaces}🔄 طالما\n`;
                result += Parser.formatAST(node.condition, indent + 1);
                result += Parser.formatAST(node.body, indent + 1);
                break;
                
            case AST_NODE_TYPES.FUNCTION_DEF:
                result += `${spaces}⚙️ دالة: ${node.name} (${node.params.length} معامل)\n`;
                result += Parser.formatAST(node.body, indent + 1);
                break;
                
            case AST_NODE_TYPES.FUNCTION_CALL:
                result += `${spaces}📞 استدعاء: ${node.name} (${node.args.length} معامل)\n`;
                for (const arg of node.args) {
                    result += Parser.formatAST(arg, indent + 1);
                }
                break;
                
            case AST_NODE_TYPES.RETURN:
                result += `${spaces}⬅️ أعد\n`;
                if (node.value) {
                    result += Parser.formatAST(node.value, indent + 1);
                }
                break;
                
            case AST_NODE_TYPES.PRINT:
                result += `${spaces}🖨️ اكتب\n`;
                result += Parser.formatAST(node.expression, indent + 1);
                break;
                
            case AST_NODE_TYPES.BINARY_OP:
                result += `${spaces}➕ عملية: ${node.op}\n`;
                result += Parser.formatAST(node.left, indent + 1);
                result += Parser.formatAST(node.right, indent + 1);
                break;
                
            case AST_NODE_TYPES.UNARY_OP:
                result += `${spaces}➖ عملية أحادية: ${node.op}\n`;
                result += Parser.formatAST(node.operand, indent + 1);
                break;
                
            case AST_NODE_TYPES.LITERAL:
                result += `${spaces}💎 قيمة: ${node.value} (${node.valueType})\n`;
                break;
                
            case AST_NODE_TYPES.IDENTIFIER:
                result += `${spaces}🏷️ معرف: ${node.name}\n`;
                break;
                
            case AST_NODE_TYPES.ARRAY:
                result += `${spaces}📚 مصفوفة (${node.elements.length} عنصر)\n`;
                for (const elem of node.elements) {
                    result += Parser.formatAST(elem, indent + 1);
                }
                break;
                
            case AST_NODE_TYPES.BREAK:
                result += `${spaces}⏹️ توقف\n`;
                break;
                
            case AST_NODE_TYPES.CONTINUE:
                result += `${spaces}▶️ استمر\n`;
                break;
                
            default:
                result += `${spaces}❓ عقدة غير معروفة: ${node.type}\n`;
        }
        
        return result;
    }
}

// Export for use in other modules
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { Parser, ASTNode, AST_NODE_TYPES };
}
