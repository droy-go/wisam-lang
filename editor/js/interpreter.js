// ============================================
// Wisam Code Editor - Interpreter
// محرر وسام - المفسر
// ============================================

// Value types
const VALUE_TYPES = {
    NUMBER: 'NUMBER',
    STRING: 'STRING',
    BOOLEAN: 'BOOLEAN',
    NULL: 'NULL',
    ARRAY: 'ARRAY',
    OBJECT: 'OBJECT',
    FUNCTION: 'FUNCTION'
};

// Value class
class Value {
    constructor(type, value) {
        this.type = type;
        this.value = value;
    }
    
    toString() {
        switch (this.type) {
            case VALUE_TYPES.NULL:
                return 'فارغ';
            case VALUE_TYPES.BOOLEAN:
                return this.value ? 'صحيح' : 'خطأ';
            case VALUE_TYPES.STRING:
                return this.value;
            case VALUE_TYPES.NUMBER:
                return String(this.value);
            case VALUE_TYPES.ARRAY:
                return '[' + this.value.map(v => v.toString()).join('، ') + ']';
            default:
                return String(this.value);
        }
    }
    
    isTruthy() {
        switch (this.type) {
            case VALUE_TYPES.NULL:
                return false;
            case VALUE_TYPES.BOOLEAN:
                return this.value;
            case VALUE_TYPES.NUMBER:
                return this.value !== 0;
            case VALUE_TYPES.STRING:
                return this.value.length > 0;
            case VALUE_TYPES.ARRAY:
                return this.value.length > 0;
            default:
                return true;
        }
    }
}

// Environment class for variable scoping
class Environment {
    constructor(parent = null) {
        this.variables = new Map();
        this.constants = new Set();
        this.parent = parent;
    }
    
    define(name, value, isConstant = false) {
        this.variables.set(name, value);
        if (isConstant) {
            this.constants.add(name);
        }
    }
    
    get(name) {
        if (this.variables.has(name)) {
            return this.variables.get(name);
        }
        if (this.parent) {
            return this.parent.get(name);
        }
        return null;
    }
    
    set(name, value) {
        if (this.variables.has(name)) {
            if (this.constants.has(name)) {
                throw new Error(`لا يمكن تعديل الثابت "${name}"`);
            }
            this.variables.set(name, value);
            return true;
        }
        if (this.parent) {
            return this.parent.set(name, value);
        }
        return false;
    }
    
    exists(name) {
        return this.variables.has(name) || (this.parent && this.parent.exists(name));
    }
}

// Interpreter class
class Interpreter {
    constructor() {
        this.globalEnv = new Environment();
        this.currentEnv = this.globalEnv;
        this.output = [];
        this.isReturning = false;
        this.returnValue = null;
        this.isBreaking = false;
        this.isContinuing = false;
        
        // Define built-in constants
        this.globalEnv.define('صحيح', new Value(VALUE_TYPES.BOOLEAN, true), true);
        this.globalEnv.define('خطأ', new Value(VALUE_TYPES.BOOLEAN, false), true);
        this.globalEnv.define('فارغ', new Value(VALUE_TYPES.NULL, null), true);
        this.globalEnv.define('PI', new Value(VALUE_TYPES.NUMBER, Math.PI), true);
        this.globalEnv.define('E', new Value(VALUE_TYPES.NUMBER, Math.E), true);
    }
    
    // Main evaluation method
    evaluate(node) {
        switch (node.type) {
            case AST_NODE_TYPES.PROGRAM:
                return this.evaluateProgram(node);
            case AST_NODE_TYPES.LET:
                return this.evaluateLet(node);
            case AST_NODE_TYPES.CONST:
                return this.evaluateConst(node);
            case AST_NODE_TYPES.ASSIGN:
                return this.evaluateAssign(node);
            case AST_NODE_TYPES.IF:
                return this.evaluateIf(node);
            case AST_NODE_TYPES.FOR:
                return this.evaluateFor(node);
            case AST_NODE_TYPES.WHILE:
                return this.evaluateWhile(node);
            case AST_NODE_TYPES.FUNCTION_DEF:
                return this.evaluateFunctionDef(node);
            case AST_NODE_TYPES.FUNCTION_CALL:
                return this.evaluateFunctionCall(node);
            case AST_NODE_TYPES.RETURN:
                return this.evaluateReturn(node);
            case AST_NODE_TYPES.PRINT:
                return this.evaluatePrint(node);
            case AST_NODE_TYPES.BINARY_OP:
                return this.evaluateBinaryOp(node);
            case AST_NODE_TYPES.UNARY_OP:
                return this.evaluateUnaryOp(node);
            case AST_NODE_TYPES.LITERAL:
                return this.evaluateLiteral(node);
            case AST_NODE_TYPES.IDENTIFIER:
                return this.evaluateIdentifier(node);
            case AST_NODE_TYPES.ARRAY:
                return this.evaluateArray(node);
            case AST_NODE_TYPES.BREAK:
                return this.evaluateBreak();
            case AST_NODE_TYPES.CONTINUE:
                return this.evaluateContinue();
            default:
                throw new Error(`نوع عقدة غير معروف: ${node.type}`);
        }
    }
    
    evaluateProgram(node) {
        let result = new Value(VALUE_TYPES.NULL, null);
        
        for (const statement of node.statements) {
            result = this.evaluate(statement);
            
            if (this.isReturning || this.isBreaking || this.isContinuing) {
                break;
            }
        }
        
        return result;
    }
    
    evaluateLet(node) {
        const value = this.evaluate(node.value);
        this.currentEnv.define(node.name, value);
        return value;
    }
    
    evaluateConst(node) {
        const value = this.evaluate(node.value);
        this.currentEnv.define(node.name, value, true);
        return value;
    }
    
    evaluateAssign(node) {
        const value = this.evaluate(node.value);
        if (!this.currentEnv.set(node.name, value)) {
            throw new Error(`المتغير "${node.name}" غير معرف`);
        }
        return value;
    }
    
    evaluateIf(node) {
        const condition = this.evaluate(node.condition);
        
        if (condition.isTruthy()) {
            return this.evaluate(node.thenBranch);
        } else if (node.elseBranch) {
            return this.evaluate(node.elseBranch);
        }
        
        return new Value(VALUE_TYPES.NULL, null);
    }
    
    evaluateFor(node) {
        const start = this.evaluate(node.start);
        const end = this.evaluate(node.end);
        
        if (start.type !== VALUE_TYPES.NUMBER || end.type !== VALUE_TYPES.NUMBER) {
            throw new Error('قيم البداية والنهاية يجب أن تكون أرقاماً');
        }
        
        // Create new environment for the loop
        const loopEnv = new Environment(this.currentEnv);
        const prevEnv = this.currentEnv;
        this.currentEnv = loopEnv;
        
        let result = new Value(VALUE_TYPES.NULL, null);
        
        for (let i = start.value; i <= end.value; i++) {
            loopEnv.define(node.varName, new Value(VALUE_TYPES.NUMBER, i));
            
            result = this.evaluate(node.body);
            
            if (this.isReturning) {
                break;
            }
            
            if (this.isBreaking) {
                this.isBreaking = false;
                break;
            }
            
            if (this.isContinuing) {
                this.isContinuing = false;
            }
        }
        
        this.currentEnv = prevEnv;
        return result;
    }
    
    evaluateWhile(node) {
        let result = new Value(VALUE_TYPES.NULL, null);
        
        while (true) {
            const condition = this.evaluate(node.condition);
            
            if (!condition.isTruthy()) {
                break;
            }
            
            result = this.evaluate(node.body);
            
            if (this.isReturning) {
                break;
            }
            
            if (this.isBreaking) {
                this.isBreaking = false;
                break;
            }
            
            if (this.isContinuing) {
                this.isContinuing = false;
            }
        }
        
        return result;
    }
    
    evaluateFunctionDef(node) {
        const func = new Value(VALUE_TYPES.FUNCTION, {
            name: node.name,
            params: node.params,
            body: node.body,
            closure: this.currentEnv
        });
        
        this.currentEnv.define(node.name, func);
        return func;
    }
    
    evaluateFunctionCall(node) {
        // Check for built-in functions
        const builtInResult = this.evaluateBuiltInFunction(node.name, node.args);
        if (builtInResult !== null) {
            return builtInResult;
        }
        
        // Get function from environment
        const func = this.currentEnv.get(node.name);
        
        if (!func) {
            throw new Error(`الدالة "${node.name}" غير معرفة`);
        }
        
        if (func.type !== VALUE_TYPES.FUNCTION) {
            throw new Error(`"${node.name}" ليست دالة`);
        }
        
        // Evaluate arguments
        const args = node.args.map(arg => this.evaluate(arg));
        
        // Create function environment
        const funcEnv = new Environment(func.value.closure);
        
        // Bind parameters
        for (let i = 0; i < func.value.params.length && i < args.length; i++) {
            funcEnv.define(func.value.params[i], args[i]);
        }
        
        // Execute function body
        const prevEnv = this.currentEnv;
        this.currentEnv = funcEnv;
        
        this.evaluate(func.value.body);
        
        const result = this.returnValue || new Value(VALUE_TYPES.NULL, null);
        this.isReturning = false;
        this.returnValue = null;
        
        this.currentEnv = prevEnv;
        
        return result;
    }
    
    evaluateBuiltInFunction(name, args) {
        const evaluatedArgs = args.map(arg => this.evaluate(arg));
        
        switch (name) {
            case 'جذر':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.sqrt(evaluatedArgs[0].value));
                }
                break;
                
            case 'أس':
                if (evaluatedArgs.length >= 2 && 
                    evaluatedArgs[0].type === VALUE_TYPES.NUMBER &&
                    evaluatedArgs[1].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.pow(evaluatedArgs[0].value, evaluatedArgs[1].value));
                }
                break;
                
            case 'مطلق':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.abs(evaluatedArgs[0].value));
                }
                break;
                
            case 'أرض':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.floor(evaluatedArgs[0].value));
                }
                break;
                
            case 'سقف':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.ceil(evaluatedArgs[0].value));
                }
                break;
                
            case 'قريب':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.round(evaluatedArgs[0].value));
                }
                break;
                
            case 'جيب':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.sin(evaluatedArgs[0].value));
                }
                break;
                
            case 'جيب_التام':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.cos(evaluatedArgs[0].value));
                }
                break;
                
            case 'ظل':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.tan(evaluatedArgs[0].value));
                }
                break;
                
            case 'لوغاريتم':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.log(evaluatedArgs[0].value));
                }
                break;
                
            case 'لوغاريتم10':
                if (evaluatedArgs.length >= 1 && evaluatedArgs[0].type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.log10(evaluatedArgs[0].value));
                }
                break;
                
            case 'الطول':
                if (evaluatedArgs.length >= 1) {
                    if (evaluatedArgs[0].type === VALUE_TYPES.STRING) {
                        return new Value(VALUE_TYPES.NUMBER, evaluatedArgs[0].value.length);
                    }
                    if (evaluatedArgs[0].type === VALUE_TYPES.ARRAY) {
                        return new Value(VALUE_TYPES.NUMBER, evaluatedArgs[0].value.length);
                    }
                }
                break;
        }
        
        return null; // Not a built-in function
    }
    
    evaluateReturn(node) {
        if (node.value) {
            this.returnValue = this.evaluate(node.value);
        } else {
            this.returnValue = new Value(VALUE_TYPES.NULL, null);
        }
        this.isReturning = true;
        return this.returnValue;
    }
    
    evaluatePrint(node) {
        const value = this.evaluate(node.expression);
        let output = value.toString();
        
        // Handle string interpolation
        if (value.type === VALUE_TYPES.STRING) {
            output = this.interpolateString(value.value);
        }
        
        this.output.push(output);
        return new Value(VALUE_TYPES.NULL, null);
    }
    
    interpolateString(str) {
        // Replace {variable} with actual values
        return str.replace(/\{([^}]+)\}/g, (match, expr) => {
            try {
                // Try to evaluate the expression
                const lexer = new Lexer(expr);
                const tokens = lexer.tokenize();
                const parser = new Parser(tokens);
                const ast = parser.parse();
                const result = this.evaluate(ast.statements[0]);
                return result.toString();
            } catch (e) {
                // If evaluation fails, try to get variable value
                const varValue = this.currentEnv.get(expr.trim());
                if (varValue) {
                    return varValue.toString();
                }
                return match; // Return original if can't evaluate
            }
        });
    }
    
    evaluateBinaryOp(node) {
        const left = this.evaluate(node.left);
        const right = this.evaluate(node.right);
        
        switch (node.op) {
            case TOKEN_TYPES.PLUS:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, left.value + right.value);
                }
                if (left.type === VALUE_TYPES.STRING || right.type === VALUE_TYPES.STRING) {
                    return new Value(VALUE_TYPES.STRING, left.toString() + right.toString());
                }
                throw new Error('لا يمكن الجمع بين هذين النوعين');
                
            case TOKEN_TYPES.MINUS:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, left.value - right.value);
                }
                throw new Error('لا يمكن الطرح بين هذين النوعين');
                
            case TOKEN_TYPES.MULTIPLY:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, left.value * right.value);
                }
                throw new Error('لا يمكن الضرب بين هذين النوعين');
                
            case TOKEN_TYPES.DIVIDE:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    if (right.value === 0) {
                        throw new Error('قسمة على صفر');
                    }
                    return new Value(VALUE_TYPES.NUMBER, left.value / right.value);
                }
                throw new Error('لا يمكن القسمة بين هذين النوعين');
                
            case TOKEN_TYPES.MODULO:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, left.value % right.value);
                }
                throw new Error('لا يمكن باقي القسمة بين هذين النوعين');
                
            case TOKEN_TYPES.POWER:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, Math.pow(left.value, right.value));
                }
                throw new Error('لا يمكن الأس بين هذين النوعين');
                
            case TOKEN_TYPES.EQUAL:
                return new Value(VALUE_TYPES.BOOLEAN, this.valuesEqual(left, right));
                
            case TOKEN_TYPES.NOT_EQUAL:
                return new Value(VALUE_TYPES.BOOLEAN, !this.valuesEqual(left, right));
                
            case TOKEN_TYPES.GREATER:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.BOOLEAN, left.value > right.value);
                }
                throw new Error('لا يمكن المقارنة بين هذين النوعين');
                
            case TOKEN_TYPES.LESS:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.BOOLEAN, left.value < right.value);
                }
                throw new Error('لا يمكن المقارنة بين هذين النوعين');
                
            case TOKEN_TYPES.GREATER_EQ:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.BOOLEAN, left.value >= right.value);
                }
                throw new Error('لا يمكن المقارنة بين هذين النوعين');
                
            case TOKEN_TYPES.LESS_EQ:
                if (left.type === VALUE_TYPES.NUMBER && right.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.BOOLEAN, left.value <= right.value);
                }
                throw new Error('لا يمكن المقارنة بين هذين النوعين');
                
            case TOKEN_TYPES.AND:
                return new Value(VALUE_TYPES.BOOLEAN, left.isTruthy() && right.isTruthy());
                
            case TOKEN_TYPES.OR:
                return new Value(VALUE_TYPES.BOOLEAN, left.isTruthy() || right.isTruthy());
                
            default:
                throw new Error(`عملية غير معروفة: ${node.op}`);
        }
    }
    
    evaluateUnaryOp(node) {
        const operand = this.evaluate(node.operand);
        
        switch (node.op) {
            case TOKEN_TYPES.MINUS:
                if (operand.type === VALUE_TYPES.NUMBER) {
                    return new Value(VALUE_TYPES.NUMBER, -operand.value);
                }
                throw new Error('لا يمكن إشارة سالبة على هذا النوع');
                
            case TOKEN_TYPES.NOT:
                return new Value(VALUE_TYPES.BOOLEAN, !operand.isTruthy());
                
            default:
                throw new Error(`عملية أحادية غير معروفة: ${node.op}`);
        }
    }
    
    evaluateLiteral(node) {
        switch (node.valueType) {
            case 'number':
                return new Value(VALUE_TYPES.NUMBER, node.value);
            case 'string':
                return new Value(VALUE_TYPES.STRING, node.value);
            case 'boolean':
                return new Value(VALUE_TYPES.BOOLEAN, node.value);
            case 'null':
                return new Value(VALUE_TYPES.NULL, null);
            default:
                throw new Error(`نوع قيمة غير معروف: ${node.valueType}`);
        }
    }
    
    evaluateIdentifier(node) {
        const value = this.currentEnv.get(node.name);
        if (value === null) {
            throw new Error(`المتغير "${node.name}" غير معرف`);
        }
        return value;
    }
    
    evaluateArray(node) {
        const elements = node.elements.map(elem => this.evaluate(elem));
        return new Value(VALUE_TYPES.ARRAY, elements);
    }
    
    evaluateBreak() {
        this.isBreaking = true;
        return new Value(VALUE_TYPES.NULL, null);
    }
    
    evaluateContinue() {
        this.isContinuing = true;
        return new Value(VALUE_TYPES.NULL, null);
    }
    
    valuesEqual(a, b) {
        if (a.type !== b.type) return false;
        
        switch (a.type) {
            case VALUE_TYPES.NULL:
                return true;
            case VALUE_TYPES.NUMBER:
                return a.value === b.value;
            case VALUE_TYPES.STRING:
                return a.value === b.value;
            case VALUE_TYPES.BOOLEAN:
                return a.value === b.value;
            default:
                return a.value === b.value;
        }
    }
    
    // Run the interpreter
    run(ast) {
        this.output = [];
        this.isReturning = false;
        this.returnValue = null;
        this.isBreaking = false;
        this.isContinuing = false;
        
        try {
            this.evaluate(ast);
            return {
                success: true,
                output: this.output,
                errors: []
            };
        } catch (error) {
            return {
                success: false,
                output: this.output,
                errors: [error.message]
            };
        }
    }
}

// Export for use in other modules
if (typeof module !== 'undefined' && module.exports) {
    module.exports = { Interpreter, Value, Environment, VALUE_TYPES };
}
