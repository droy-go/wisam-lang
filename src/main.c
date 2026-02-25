#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// عرض شعار وسام
void print_logo() {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════════╗\n");
    printf("║                                                                  ║\n");
    printf("║                    🌟 لغة وسام Wisam 🌟                          ║\n");
    printf("║              البرمجة باللغة العربية بامتياز                       ║\n");
    printf("║                                                                  ║\n");
    printf("║              الإصدار %s - %s                    ║\n", WISAM_VERSION, WISAM_VERSION_NAME);
    printf("║                                                                  ║\n");
    printf("╚══════════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

// عرض المساعدة
void print_help() {
    printf("استخدام: wisam [خيارات] <ملف>\n\n");
    printf("خيارات:\n");
    printf("  -h, --help          عرض هذه المساعدة\n");
    printf("  -v, --version       عرض الإصدار\n");
    printf("  -i, --interactive   الوضع التفاعلي\n");
    printf("  -c, --compile       ترجمة الملف إلى C\n");
    printf("  -r, --run           تشغيل الملف\n");
    printf("  -t, --tokens        عرض الرموز المميزة\n");
    printf("  -a, --ast           عرض شجرة النحو\n");
    printf("  -d, --debug         وضع التصحيح\n");
    printf("\n");
    printf("أمثلة:\n");
    printf("  wisam program.wsm        تشغيل ملف وسام\n");
    printf("  wisam -i                 الوضع التفاعلي\n");
    printf("  wisam -c program.wsm     ترجمة إلى C\n");
    printf("  wisam -t program.wsm     عرض الرموز\n");
    printf("\n");
}

// عرض الإصدار
void print_version() {
    printf("وسام الإصدار %s (%s)\n", WISAM_VERSION, WISAM_VERSION_NAME);
    printf("لغة برمجة عربية متكاملة\n");
    printf("Copyright (c) 2024-2025 Wisam Programming Language\n");
    printf("صنع بـ ❤️ في الوطن العربي\n");
}

// قراءة ملف
char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "خطأ: لا يمكن فتح الملف '%s'\n", filename);
        return NULL;
    }
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *content = malloc(size + 1);
    if (!content) {
        fclose(file);
        return NULL;
    }
    
    fread(content, 1, size, file);
    content[size] = '\0';
    
    fclose(file);
    return content;
}

// طباعة الرموز
void print_tokens(Token *tokens, int count) {
    printf("═ الرموز المميزة ═════════════════════════════════════════════════\n\n");
    
    const char *token_names[] = {
        "ليكن", "ثابت", "إذا", "إذن", "وإلا", "انتهى", "لكل", "من", "إلى",
        "دالة", "أعد", "هيكل", "صنف", "استورد", "أنشئ", "باسم", "طالما",
        "توقف", "استمر", "و", "أو", "ليس", "في", "من", "حاول", "امسك",
        "أخيراً", "ألقِ", "حسب", "حالة", "افتراضي", "نفذ", "حتى", "لكل_عنصر",
        "أنتج", "غير_متزامن", "انتظر", "عام", "خاص", "محمي", "ثابت_الصنف",
        "يرث", "ينفذ", "جديد", "هذا", "أب", "فارغ", "صحيح", "خطأ",
        "رقم", "نص", "منطقي", "مصفوفة", "كائن", "معرف", "=", "+", "-",
        "*", "/", "%", "^", "==", "!=", ">", "<", ">=", "<=", "(", ")",
        "{", "}", "[", "]", "،", ".", ":", "؛", "سطر_جديد", "EOF",
        "اكتب", "ادخل", "نوع", "حجم", "عقل", "منظومة", "تعلّم", "اسأل",
        "استجب", "احفظ", "حمّل", "GPU", "درّب", "شبكة_عصبية", "طبقات",
        "معدل_تعلم", "نموذج", "تنبأ", "مجموعة_بيانات", "++", "--",
        "+=", "-=", "*=", "/=", "%=", "&", "|", "^", "~", "<<", ">>"
    };
    
    for (int i = 0; i < count && tokens[i].type != TOKEN_EOF; i++) {
        const char *name = (tokens[i].type < sizeof(token_names) / sizeof(token_names[0])) 
                          ? token_names[tokens[i].type] : "غير_معروف";
        printf("[%4d:%3d] %-20s '%s'\n", 
               tokens[i].line, tokens[i].column, name, tokens[i].value);
    }
    
    printf("\n══════════════════════════════════════════════════════════════════\n");
}

// طباعة شجرة النحو
void print_ast(ASTNode *node, int indent) {
    if (!node) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    switch (node->type) {
        case AST_PROGRAM:
            printf("📦 برنامج (%d عبارة)\n", node->as.program.count);
            for (int i = 0; i < node->as.program.count; i++) {
                print_ast(node->as.program.statements[i], indent + 1);
            }
            break;
        case AST_LET:
            printf("📌 تعريف: %s\n", node->as.let.name);
            print_ast(node->as.let.value, indent + 1);
            break;
        case AST_CONST:
            printf("🔒 ثابت: %s\n", node->as.constant.name);
            print_ast(node->as.constant.value, indent + 1);
            break;
        case AST_ASSIGN:
            printf("✏️ تعيين: %s\n", node->as.assign.name);
            print_ast(node->as.assign.value, indent + 1);
            break;
        case AST_IF:
            printf("🔀 إذا\n");
            print_ast(node->as.if_stmt.condition, indent + 1);
            print_ast(node->as.if_stmt.then_branch, indent + 1);
            if (node->as.if_stmt.else_branch) {
                print_ast(node->as.if_stmt.else_branch, indent + 1);
            }
            break;
        case AST_FOR:
            printf("🔁 لكل: %s\n", node->as.for_loop.var_name);
            print_ast(node->as.for_loop.start, indent + 1);
            print_ast(node->as.for_loop.end, indent + 1);
            print_ast(node->as.for_loop.body, indent + 1);
            break;
        case AST_WHILE:
            printf("🔄 طالما\n");
            print_ast(node->as.while_loop.condition, indent + 1);
            print_ast(node->as.while_loop.body, indent + 1);
            break;
        case AST_FUNCTION_DEF:
            printf("⚙️ دالة: %s (%d معامل)\n", 
                   node->as.function_def.name, node->as.function_def.param_count);
            print_ast(node->as.function_def.body, indent + 1);
            break;
        case AST_FUNCTION_CALL:
            printf("📞 استدعاء: %s (%d معامل)\n", 
                   node->as.function_call.name, node->as.function_call.arg_count);
            for (int i = 0; i < node->as.function_call.arg_count; i++) {
                print_ast(node->as.function_call.args[i], indent + 1);
            }
            break;
        case AST_RETURN:
            printf("⬅️ أعد\n");
            if (node->as.return_stmt.value) {
                print_ast(node->as.return_stmt.value, indent + 1);
            }
            break;
        case AST_PRINT:
            printf("🖨️ اكتب\n");
            print_ast(node->as.print.expression, indent + 1);
            break;
        case AST_INPUT:
            printf("⌨️ ادخل\n");
            break;
        case AST_LITERAL:
            {
                char *str = value_to_string(&node->as.literal.value);
                printf("💎 قيمة: %s\n", str);
                free(str);
            }
            break;
        case AST_IDENTIFIER:
            printf("🏷️ معرف: %s\n", node->as.identifier.name);
            break;
        case AST_BINARY_OP:
            printf("➕ عملية ثنائية\n");
            print_ast(node->as.binary_op.left, indent + 1);
            print_ast(node->as.binary_op.right, indent + 1);
            break;
        case AST_UNARY_OP:
            printf("➖ عملية أحادية\n");
            print_ast(node->as.unary_op.operand, indent + 1);
            break;
        case AST_ARRAY:
            printf("📚 مصفوفة (%d عنصر)\n", node->as.array.count);
            for (int i = 0; i < node->as.array.count; i++) {
                print_ast(node->as.array.elements[i], indent + 1);
            }
            break;
        case AST_ARRAY_ACCESS:
            printf("📖 وصول للمصفوفة\n");
            print_ast(node->as.array_access.array, indent + 1);
            print_ast(node->as.array_access.index, indent + 1);
            break;
        case AST_BREAK:
            printf("⏹️ توقف\n");
            break;
        case AST_CONTINUE:
            printf("▶️ استمر\n");
            break;
        default:
            printf("❓ عقدة غير معروفة: %d\n", node->type);
            break;
    }
}

// الوضع التفاعلي
void interactive_mode() {
    print_logo();
    printf("الوضع التفاعلي - اكتب 'خروج' للخروج\n");
    printf("اكتب 'مساعدة' للحصول على قائمة الأوامر\n\n");
    
    Interpreter *interp = interpreter_create();
    char line[4096];
    
    // تعريف المتغيرات الأساسية
    environment_define(interp->global_env, "اسم_اللغة", 
                      value_create_string("وسام"), true);
    environment_define(interp->global_env, "إصدار", 
                      value_create_number(2.0), true);
    
    while (1) {
        printf("وسام> ");
        fflush(stdout);
        
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        
        // إزالة newline
        line[strcspn(line, "\n")] = 0;
        
        // أوامر خاصة
        if (strcmp(line, "خروج") == 0 || strcmp(line, "exit") == 0) {
            printf("مع السلامة! 👋\n");
            break;
        }
        
        if (strcmp(line, "مساعدة") == 0 || strcmp(line, "help") == 0) {
            printf("\nأوامر الوضع التفاعلي:\n");
            printf("  خروج          - الخروج من الوضع التفاعلي\n");
            printf("  مساعدة        - عرض هذه المساعدة\n");
            printf("  متغيرات       - عرض المتغيرات المعرفة\n");
            printf("  مسح           - مسح الشاشة\n");
            printf("\n");
            continue;
        }
        
        if (strcmp(line, "متغيرات") == 0) {
            printf("\nالمتغيرات المعرفة:\n");
            for (int i = 0; i < interp->global_env->var_count; i++) {
                char *str = value_to_string(&interp->global_env->variables[i].value);
                printf("  %s = %s\n", interp->global_env->variables[i].name, str);
                free(str);
            }
            printf("\n");
            continue;
        }
        
        if (strcmp(line, "مسح") == 0 || strcmp(line, "clear") == 0) {
            printf("\033[2J\033[H");
            print_logo();
            continue;
        }
        
        if (strlen(line) == 0) {
            continue;
        }
        
        // تحليل وتنفيذ الكود
        Lexer *lexer = lexer_create(line, "<تفاعلي>");
        int token_count;
        Token *tokens = lexer_tokenize(lexer, &token_count);
        
        if (lexer_get_error(lexer)) {
            fprintf(stderr, "خطأ في التحليل اللغوي: %s\n", lexer_get_error(lexer));
            free(tokens);
            lexer_destroy(lexer);
            continue;
        }
        
        Parser *parser = parser_create(tokens, token_count);
        ASTNode *ast = parser_parse(parser);
        
        if (parser_get_error(parser)) {
            fprintf(stderr, "خطأ نحوي عند السطر %d، العمود %d: %s\n",
                    parser_get_error_line(parser), parser_get_error_column(parser),
                    parser_get_error(parser));
            // تحرير الذاكرة
            free(ast);
            parser_destroy(parser);
            free(tokens);
            lexer_destroy(lexer);
            continue;
        }
        
        // تنفيذ البرنامج
        Value result = interpreter_evaluate(interp, ast);
        
        if (result.type != VAL_NULL) {
            char *str = value_to_string(&result);
            printf("➜ %s\n", str);
            free(str);
        }
        
        value_free(&result);
        
        // تحرير الذاكرة
        free(ast);
        parser_destroy(parser);
        free(tokens);
        lexer_destroy(lexer);
    }
    
    interpreter_destroy(interp);
}

// تشغيل ملف
int run_file(const char *filename, bool show_tokens, bool show_ast, bool debug) {
    char *source = read_file(filename);
    if (!source) {
        return 1;
    }
    
    if (debug) {
        printf("جاري تحليل الملف: %s\n", filename);
    }
    
    // التحليل اللغوي
    Lexer *lexer = lexer_create(source, filename);
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    if (lexer_get_error(lexer)) {
        fprintf(stderr, "خطأ في التحليل اللغوي: %s\n", lexer_get_error(lexer));
        free(tokens);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    if (show_tokens) {
        print_tokens(tokens, token_count);
        free(tokens);
        lexer_destroy(lexer);
        free(source);
        return 0;
    }
    
    // التحليل النحوي
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    if (parser_get_error(parser)) {
        fprintf(stderr, "خطأ نحوي عند السطر %d، العمود %d: %s\n",
                parser_get_error_line(parser), parser_get_error_column(parser),
                parser_get_error(parser));
        free(ast);
        parser_destroy(parser);
        free(tokens);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    if (show_ast) {
        printf("═ شجرة النحو ══════════════════════════════════════════════════════\n\n");
        print_ast(ast, 0);
        printf("\n══════════════════════════════════════════════════════════════════\n");
        free(ast);
        parser_destroy(parser);
        free(tokens);
        lexer_destroy(lexer);
        free(source);
        return 0;
    }
    
    // التنفيذ
    Interpreter *interp = interpreter_create();
    interpreter_run(interp, ast);
    interpreter_destroy(interp);
    
    // تحرير الذاكرة
    free(ast);
    parser_destroy(parser);
    free(tokens);
    lexer_destroy(lexer);
    free(source);
    
    return 0;
}

int main(int argc, char *argv[]) {
    // تعيين اللغة العربية
    setlocale(LC_ALL, "");
    
    if (argc < 2) {
        print_logo();
        print_help();
        return 0;
    }
    
    bool interactive = false;
    bool show_tokens = false;
    bool show_ast = false;
    bool debug = false;
    bool compile = false;
    const char *filename = NULL;
    
    // تحليل المعاملات
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_logo();
            print_help();
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
            interactive = true;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tokens") == 0) {
            show_tokens = true;
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ast") == 0) {
            show_ast = true;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = true;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--compile") == 0) {
            compile = true;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        }
    }
    
    if (interactive) {
        interactive_mode();
        return 0;
    }
    
    if (!filename) {
        fprintf(stderr, "خطأ: لم يتم تحديد ملف\n");
        print_help();
        return 1;
    }
    
    if (compile) {
        printf("خاصية الترجمة قيد التطوير...\n");
        return 0;
    }
    
    return run_file(filename, show_tokens, show_ast, debug);
}
