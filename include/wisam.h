#ifndef WISAM_H
#define WISAM_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <wchar.h>
#include <locale.h>
#include <time.h>
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
    #define WISAM_OS "Windows"
#else
    #include <unistd.h>
    #include <sys/resource.h>
    #define WISAM_OS "Linux"
#endif

#define WISAM_VERSION "1.0.0"
#define MAX_TOKEN_LEN 256
#define MAX_LINE_LEN 4096
#define MAX_VARS 1000
#define MAX_FUNCS 500
#define MAX_STRUCTS 100
#define MAX_MODULES 50

typedef enum {
    // Data types
    TYPE_NULL,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_ARRAY,
    TYPE_STRUCT,
    TYPE_FUNCTION,
    TYPE_MODULE,
    TYPE_AI_MIND,
    TYPE_AI_ECOSYSTEM,
    TYPE_NEURAL_NETWORK
} ValueType;

typedef struct Value Value;
typedef struct StructField StructField;
typedef struct WisamStruct WisamStruct;

typedef struct {
    char *name;
    Value *value;
} StructMember;

struct StructField {
    char *name;
    ValueType type;
    StructField *next;
};

struct WisamStruct {
    char *name;
    StructField *fields;
    int field_count;
    StructMember *members;
    int member_count;
};

typedef struct {
    Value **items;
    int count;
    int capacity;
} Array;

typedef struct {
    char **patterns;
    char **responses;
    int count;
    int capacity;
} AIMind;

typedef struct {
    char *name;
    int layers;
    double learning_rate;
    bool use_gpu;
    char *training_data;
} NeuralNetwork;

typedef struct {
    char *name;
    AIMind *text_mind;
    bool has_long_term_memory;
    bool self_learning;
    NeuralNetwork **networks;
    int network_count;
} AIEcosystem;

struct Value {
    ValueType type;
    union {
        double number;
        char *string;
        bool boolean;
        Array *array;
        WisamStruct *strct;
        AIMind *ai_mind;
        AIEcosystem *ai_ecosystem;
        NeuralNetwork *neural_net;
    } data;
};

typedef struct {
    char *name;
    Value *value;
    bool is_constant;
} Variable;

typedef struct {
    char *name;
    char **params;
    int param_count;
    struct ASTNode **body;
    int body_count;
} Function;

typedef struct {
    char *name;
    void (*init)(void);
    Value (*call)(const char *method, Value *args, int arg_count);
} Module;

// Global state
extern Variable variables[MAX_VARS];
extern int var_count;
extern Function functions[MAX_FUNCS];
extern int func_count;
extern WisamStruct structs[MAX_STRUCTS];
extern int struct_count;
extern Module modules[MAX_MODULES];
extern int module_count;

// Utility functions
Value *create_value(ValueType type);
void free_value(Value *val);
Value *copy_value(Value *val);
char *value_to_string(Value *val);
double value_to_number(Value *val);

// UTF-8 helper functions
int utf8_char_len(unsigned char c);
int utf8_strlen(const char *s);
char *utf8_substring(const char *s, int start, int len);

#endif // WISAM_H
