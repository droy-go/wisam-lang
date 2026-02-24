#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// عرض شعار وسام
void print_logo() {
    printf("\n");
    printf("╔═══════════════════════════════════════════════════════════╗\n");
    printf("║                                                           ║\n");
    printf("║                    لغة وسام Wisam                         ║\n");
    printf("║              البرمجة باللغة العربية                        ║\n");
    printf("║                                                           ║\n");
    printf("║                   الإصدار %s                            ║\n", WISAM_VERSION);
    printf("║                                                           ║\n");
    printf("╚═══════════════════════════════════════════════════════════╝\n");
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
    printf("\n");
    printf("أمثلة:\n");
    printf("  wisam program.wsm        تشغيل ملف وسام\n");
    printf("  wisam -i                 الوضع التفاعلي\n");
    printf("  wisam -c program.wsm     ترجمة إلى C\n");
    printf("\n");
}

// عرض الإصدار
void print_version() {
    printf("وسام الإصدار %s\n", WISAM_VERSION);
    printf("لغة برمجة عربية متكاملة\n");
    printf("Copyright (c) 2024\n");
}

// الوضع التفاعلي
void interactive_mode() {
    print_logo();
    printf("الوضع التفاعلي - اكتب 'خروج' للخروج\n\n");
    
    Interpreter *interp = interpreter_create();
    char line[1024];
    
    // تعريف المتغيرات الأساسية
    environment_define(interp->global_env, "اسم", value_create_string("وسام"), true);
    environment_define(interp->global_env, "إصدار", value_create_number(1.0), true);
    
    while (1) {
        printf("وسام> ");
        fflush(stdout);
        
        if (!fgets(line, sizeof(line), stdin)) {
            break;
        }
        
        // إزالة newline
        line[strcspn(line, "\n")] = '\0';
        
        // التحقق من الخروج
        if (strcmp(line, "خروج") == 0 || strcmp(line, "exit") == 0) {
            printf("وداعاً! 👋\n");
            break;
        }
        
        // تخطي الأسطر الفارغة
        if (strlen(line) == 0) continue;
        
        // تنفيذ الأمر
        Lexer *lexer = lexer_create(line);
        int token_count;
        Token *tokens = lexer_tokenize(lexer, &token_count);
        
        Parser *parser = parser_create(tokens, token_count);
        ASTNode *ast = parser_parse(parser);
        
        interpreter_run(interp, ast);
        
        // تحرير الذاكرة
        // TODO: تحرير AST والرموز
        
        parser_destroy(parser);
        lexer_destroy(lexer);
    }
    
    interpreter_destroy(interp);
}

// تشغيل ملف
void run_file(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "خطأ: لا يمكن فتح الملف '%s'\n", filename);
        exit(1);
    }
    
    // قراءة الملف بالكامل
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    // إنشاء المترجم وتشغيله
    Lexer *lexer = lexer_create(source);
    int token_count;
    Token *tokens = lexer_tokenize(lexer, &token_count);
    
    Parser *parser = parser_create(tokens, token_count);
    ASTNode *ast = parser_parse(parser);
    
    Interpreter *interp = interpreter_create();
    
    // تعريف المتغيرات الأساسية
    environment_define(interp->global_env, "اسم", value_create_string("وسام"), true);
    environment_define(interp->global_env, "إصدار", value_create_number(1.0), true);
    environment_define(interp->global_env, "ميتا", value_create_object(), false);
    
    interpreter_run(interp, ast);
    
    interpreter_destroy(interp);
    parser_destroy(parser);
    lexer_destroy(lexer);
    free(source);
}

// ترجمة ملف إلى C
void compile_to_c(const char *filename) {
    printf("جاري ترجمة '%s' إلى C...\n", filename);
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "خطأ: لا يمكن فتح الملف '%s'\n", filename);
        exit(1);
    }
    
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    char *source = malloc(size + 1);
    fread(source, 1, size, f);
    source[size] = '\0';
    fclose(f);
    
    // إنشاء ملف C
    char output_name[256];
    snprintf(output_name, sizeof(output_name), "%s.c", filename);
    
    FILE *out = fopen(output_name, "w");
    if (!out) {
        fprintf(stderr, "خطأ: لا يمكن إنشاء ملف الإخراج\n");
        exit(1);
    }
    
    fprintf(out, "/* تم إنشاء هذا الملف تلقائياً من وسام */\n");
    fprintf(out, "#include <stdio.h>\n");
    fprintf(out, "#include <stdlib.h>\n");
    fprintf(out, "#include <string.h>\n\n");
    fprintf(out, "int main() {\n");
    fprintf(out, "    printf(\"تنفيذ برنامج وسام\\n\");\n");
    fprintf(out, "    /* TODO: ترجمة الكود العربي */\n");
    fprintf(out, "    return 0;\n");
    fprintf(out, "}\n");
    
    fclose(out);
    
    printf("تم إنشاء '%s'\n", output_name);
    
    free(source);
}

int main(int argc, char *argv[]) {
    // تعيين اللغة العربية
    setlocale(LC_ALL, "");
    
    if (argc < 2) {
        print_logo();
        print_help();
        return 0;
    }
    
    // معالجة الخيارات
    if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        print_help();
        return 0;
    }
    
    if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
        print_version();
        return 0;
    }
    
    if (strcmp(argv[1], "-i") == 0 || strcmp(argv[1], "--interactive") == 0) {
        interactive_mode();
        return 0;
    }
    
    if (strcmp(argv[1], "-c") == 0 || strcmp(argv[1], "--compile") == 0) {
        if (argc < 3) {
            fprintf(stderr, "خطأ: يجب تحديد ملف للترجمة\n");
            return 1;
        }
        compile_to_c(argv[2]);
        return 0;
    }
    
    if (strcmp(argv[1], "-r") == 0 || strcmp(argv[1], "--run") == 0) {
        if (argc < 3) {
            fprintf(stderr, "خطأ: يجب تحديد ملف للتشغيل\n");
            return 1;
        }
        run_file(argv[2]);
        return 0;
    }
    
    // افتراضياً، تشغيل الملف
    run_file(argv[1]);
    
    return 0;
}
