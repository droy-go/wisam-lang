#include "wisam.h"
#include "lexer.h"
#include "parser.h"
#include "interpreter.h"

// Function to read file contents
static char *read_file(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Read file
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

// Print usage
static void print_usage(const char *program) {
    printf("Wisam Programming Language (وسام) v%s\n", WISAM_VERSION);
    printf("Usage: %s [options] <file.wsm>\n", program);
    printf("\nOptions:\n");
    printf("  -h, --help       Show this help message\n");
    printf("  -v, --version    Show version information\n");
    printf("  -t, --tokens     Print tokens (for debugging)\n");
    printf("  -a, --ast        Print AST (for debugging)\n");
    printf("  -i, --interactive  Run interactive shell\n");
    printf("\nExamples:\n");
    printf("  %s hello.wsm          Run a Wisam program\n", program);
    printf("  %s -i                 Start interactive shell\n", program);
}

// Print version
static void print_version(void) {
    printf("Wisam Programming Language (وسام) v%s\n", WISAM_VERSION);
    printf("Written in C for Arabic programming\n");
    printf("Operating System: %s\n", WISAM_OS);
}

// Interactive shell
static void run_interactive(void) {
    printf("Wisam Interactive Shell (وسام) v%s\n", WISAM_VERSION);
    printf("اكتب 'خروج' أو 'exit' للخروج\n");
    printf(">>> ");
    
    char line[MAX_LINE_LEN];
    
    while (fgets(line, sizeof(line), stdin)) {
        // Remove newline
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Check for exit
        if (strcmp(line, "exit") == 0 || strcmp(line, "خروج") == 0) {
            break;
        }
        
        // Skip empty lines
        if (strlen(line) == 0) {
            printf(">>> ");
            continue;
        }
        
        // Create lexer and parser
        Lexer *lexer = lexer_create(line);
        if (!lexer) {
            printf("Error: Failed to create lexer\n");
            printf(">>> ");
            continue;
        }
        
        Parser *parser = parser_create(lexer);
        if (!parser) {
            lexer_destroy(lexer);
            printf("Error: Failed to create parser\n");
            printf(">>> ");
            continue;
        }
        
        // Parse and interpret
        ASTNode *ast = parser_parse(parser);
        if (ast) {
            interpret(ast);
            ast_destroy(ast);
        }
        
        parser_destroy(parser);
        lexer_destroy(lexer);
        
        printf(">>> ");
    }
    
    printf("\nوداعاً! 👋\n");
}

// Run a file
static int run_file(const char *filename, bool print_tokens, bool print_ast) {
    // Set locale for UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    // Read file
    char *source = read_file(filename);
    if (!source) {
        return 1;
    }
    
    // Initialize interpreter
    interpreter_init();
    
    // Create lexer
    Lexer *lexer = lexer_create(source);
    if (!lexer) {
        fprintf(stderr, "Error: Failed to create lexer\n");
        free(source);
        return 1;
    }
    
    // Print tokens if requested
    if (print_tokens) {
        printf("=== Tokens ===\n");
        Token *token;
        while ((token = lexer_next_token(lexer))->type != TOKEN_EOF) {
            printf("[%s] '%s' (line %d, col %d)\n",
                   token_type_to_string(token->type),
                   token->value ? token->value : "",
                   token->line, token->column);
            token_destroy(token);
        }
        token_destroy(token);
        printf("\n");
        
        // Recreate lexer for parsing
        lexer_destroy(lexer);
        lexer = lexer_create(source);
    }
    
    // Create parser
    Parser *parser = parser_create(lexer);
    if (!parser) {
        fprintf(stderr, "Error: Failed to create parser\n");
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Parse
    ASTNode *ast = parser_parse(parser);
    if (!ast) {
        fprintf(stderr, "Error: Parsing failed\n");
        parser_destroy(parser);
        lexer_destroy(lexer);
        free(source);
        return 1;
    }
    
    // Print AST if requested
    if (print_ast) {
        printf("=== AST ===\n");
        ast_print(ast, 0);
        printf("\n");
    }
    
    // Interpret
    printf("=== Output ===\n");
    interpret(ast);
    
    // Cleanup
    ast_destroy(ast);
    parser_destroy(parser);
    lexer_destroy(lexer);
    interpreter_cleanup();
    free(source);
    
    return 0;
}

int main(int argc, char *argv[]) {
    // Set locale for UTF-8
    setlocale(LC_ALL, "en_US.UTF-8");
    
    // Seed random
    srand((unsigned)time(NULL));
    
    // Parse arguments
    bool print_tokens = false;
    bool print_ast = false;
    bool interactive = false;
    const char *filename = NULL;
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_version();
            return 0;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tokens") == 0) {
            print_tokens = true;
        } else if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ast") == 0) {
            print_ast = true;
        } else if (strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--interactive") == 0) {
            interactive = true;
        } else if (argv[i][0] != '-') {
            filename = argv[i];
        } else {
            fprintf(stderr, "Unknown option: %s\n", argv[i]);
            print_usage(argv[0]);
            return 1;
        }
    }
    
    // Run in appropriate mode
    if (interactive) {
        run_interactive();
        return 0;
    }
    
    if (filename) {
        return run_file(filename, print_tokens, print_ast);
    }
    
    // No file specified, print usage
    print_usage(argv[0]);
    return 0;
}
