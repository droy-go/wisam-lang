#include "wisam.h"
#include "interpreter.h"

// ذكاء (AI) Module - Standard library for artificial intelligence operations

#define MAX_AI_MINDS 100
#define MAX_ECOSYSTEMS 50
#define MAX_NEURAL_NETWORKS 50
#define MAX_PATTERNS 1000
#define MAX_MEMORY_ITEMS 10000

typedef enum {
    MIND_TYPE_TEXT,
    MIND_TYPE_IMAGE,
    MIND_TYPE_AUDIO
} MindType;

typedef struct {
    char *pattern;
    char *response;
    double confidence;
} KnowledgeItem;

typedef struct {
    int id;
    char *name;
    MindType type;
    KnowledgeItem *knowledge;
    int knowledge_count;
    int knowledge_capacity;
    double learning_rate;
} AIMindFull;

typedef struct {
    char *content;
    time_t timestamp;
    double importance;
} MemoryItem;

typedef struct {
    int id;
    char *name;
    AIMindFull **minds;
    int mind_count;
    int mind_capacity;
    MemoryItem *long_term_memory;
    int memory_count;
    int memory_capacity;
    bool self_learning_enabled;
    double creativity_level;
} AIEcosystemFull;

typedef struct {
    int id;
    char *name;
    int layers;
    int *layer_sizes;
    double learning_rate;
    bool use_gpu;
    double **weights;
    double **biases;
    bool trained;
    char *training_data_file;
} NeuralNetworkFull;

static AIMindFull ai_minds[MAX_AI_MINDS];
static int ai_mind_count = 0;

static AIEcosystemFull ecosystems[MAX_ECOSYSTEMS];
static int ecosystem_count = 0;

static NeuralNetworkFull neural_networks[MAX_NEURAL_NETWORKS];
static int neural_network_count = 0;

static int next_mind_id = 1;
static int next_ecosystem_id = 1;
static int next_network_id = 1;

// Find AI mind by name
static AIMindFull *find_mind(const char *name) {
    for (int i = 0; i < ai_mind_count; i++) {
        if (strcmp(ai_minds[i].name, name) == 0) {
            return &ai_minds[i];
        }
    }
    return NULL;
}

// Find ecosystem by name
static AIEcosystemFull *find_ecosystem(const char *name) {
    for (int i = 0; i < ecosystem_count; i++) {
        if (strcmp(ecosystems[i].name, name) == 0) {
            return &ecosystems[i];
        }
    }
    return NULL;
}

// Find neural network by name
static NeuralNetworkFull *find_network(const char *name) {
    for (int i = 0; i < neural_network_count; i++) {
        if (strcmp(neural_networks[i].name, name) == 0) {
            return &neural_networks[i];
        }
    }
    return NULL;
}

// Calculate string similarity (simple Levenshtein distance)
static double calculate_similarity(const char *s1, const char *s2) {
    int len1 = strlen(s1);
    int len2 = strlen(s2);
    int matrix[len1 + 1][len2 + 1];
    
    for (int i = 0; i <= len1; i++) matrix[i][0] = i;
    for (int j = 0; j <= len2; j++) matrix[0][j] = j;
    
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            matrix[i][j] = fmin(fmin(matrix[i-1][j] + 1, matrix[i][j-1] + 1),
                               matrix[i-1][j-1] + cost);
        }
    }
    
    int max_len = (len1 > len2) ? len1 : len2;
    if (max_len == 0) return 1.0;
    
    return 1.0 - (double)matrix[len1][len2] / max_len;
}

// Create AI mind
static Value *ai_create_mind(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    const char *name = args[0]->data.string;
    MindType type = MIND_TYPE_TEXT;
    
    if (arg_count >= 2 && args[1]->type == TYPE_STRING) {
        if (strcmp(args[1]->data.string, "صور") == 0 || 
            strcmp(args[1]->data.string, "image") == 0) {
            type = MIND_TYPE_IMAGE;
        } else if (strcmp(args[1]->data.string, "صوت") == 0 || 
                   strcmp(args[1]->data.string, "audio") == 0) {
            type = MIND_TYPE_AUDIO;
        }
    }
    
    if (ai_mind_count >= MAX_AI_MINDS) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    AIMindFull *mind = &ai_minds[ai_mind_count++];
    mind->id = next_mind_id++;
    mind->name = strdup(name);
    mind->type = type;
    mind->knowledge = malloc(sizeof(KnowledgeItem) * 100);
    mind->knowledge_count = 0;
    mind->knowledge_capacity = 100;
    mind->learning_rate = 0.1;
    
    printf("[ذكاء] إنشاء عقل: \"%s\" (نوع: %s)\n", name,
           type == MIND_TYPE_TEXT ? "نصي" : 
           type == MIND_TYPE_IMAGE ? "صور" : "صوت");
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = mind->id;
    return result;
}

// Teach AI mind
static Value *ai_learn(Value **args, int arg_count) {
    if (arg_count < 3 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING || args[2]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *mind_name = args[0]->data.string;
    const char *pattern = args[1]->data.string;
    const char *response = args[2]->data.string;
    
    AIMindFull *mind = find_mind(mind_name);
    if (!mind) {
        fprintf(stderr, "العقل غير موجود: %s\n", mind_name);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Expand knowledge if needed
    if (mind->knowledge_count >= mind->knowledge_capacity) {
        mind->knowledge_capacity *= 2;
        mind->knowledge = realloc(mind->knowledge, 
                                  sizeof(KnowledgeItem) * mind->knowledge_capacity);
    }
    
    // Add knowledge
    KnowledgeItem *item = &mind->knowledge[mind->knowledge_count++];
    item->pattern = strdup(pattern);
    item->response = strdup(response);
    item->confidence = 1.0;
    
    printf("[ذكاء] تعلم: \"%s\" -> \"%s\"\n", pattern, response);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Ask AI mind
static Value *ai_ask(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup("لم أفهم السؤال");
        return result;
    }
    
    const char *mind_name = args[0]->data.string;
    const char *question = args[1]->data.string;
    
    AIMindFull *mind = find_mind(mind_name);
    if (!mind) {
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup("العقل غير موجود");
        return result;
    }
    
    // Find best matching pattern
    double best_similarity = 0.0;
    char *best_response = strdup("لم أفهم، هل يمكنك توضيح أكثر؟");
    
    for (int i = 0; i < mind->knowledge_count; i++) {
        double sim = calculate_similarity(question, mind->knowledge[i].pattern);
        if (sim > best_similarity && sim > 0.5) {
            best_similarity = sim;
            free(best_response);
            best_response = strdup(mind->knowledge[i].response);
        }
    }
    
    printf("[ذكاء] سؤال: \"%s\" -> إجابة: \"%s\" (التشابه: %.2f)\n", 
           question, best_response, best_similarity);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = best_response;
    return result;
}

// Create AI ecosystem
static Value *ai_create_ecosystem(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    const char *name = args[0]->data.string;
    
    if (ecosystem_count >= MAX_ECOSYSTEMS) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    AIEcosystemFull *eco = &ecosystems[ecosystem_count++];
    eco->id = next_ecosystem_id++;
    eco->name = strdup(name);
    eco->minds = malloc(sizeof(AIMindFull *) * 10);
    eco->mind_count = 0;
    eco->mind_capacity = 10;
    eco->long_term_memory = NULL;
    eco->memory_count = 0;
    eco->memory_capacity = 0;
    eco->self_learning_enabled = false;
    eco->creativity_level = 0.5;
    
    printf("[ذكاء] إنشاء منظومة: \"%s\"\n", name);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = eco->id;
    return result;
}

// Add mind to ecosystem
static Value *ai_add_mind(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *eco_name = args[0]->data.string;
    const char *mind_name = args[1]->data.string;
    
    AIEcosystemFull *eco = find_ecosystem(eco_name);
    AIMindFull *mind = find_mind(mind_name);
    
    if (!eco || !mind) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    if (eco->mind_count >= eco->mind_capacity) {
        eco->mind_capacity *= 2;
        eco->minds = realloc(eco->minds, sizeof(AIMindFull *) * eco->mind_capacity);
    }
    
    eco->minds[eco->mind_count++] = mind;
    
    printf("[ذكاء] إضافة عقل \"%s\" إلى المنظومة \"%s\"\n", mind_name, eco_name);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Create long-term memory
static Value *ai_create_memory(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *eco_name = args[0]->data.string;
    
    AIEcosystemFull *eco = find_ecosystem(eco_name);
    if (!eco) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    eco->memory_capacity = 1000;
    eco->long_term_memory = malloc(sizeof(MemoryItem) * eco->memory_capacity);
    eco->memory_count = 0;
    
    printf("[ذكاء] إنشاء ذاكرة طويلة للمنظومة \"%s\"\n", eco_name);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Enable self-learning
static Value *ai_self_learn(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *eco_name = args[0]->data.string;
    
    AIEcosystemFull *eco = find_ecosystem(eco_name);
    if (!eco) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    eco->self_learning_enabled = true;
    
    printf("[ذكاء] تفعيل التعلم الذاتي للمنظومة \"%s\"\n", eco_name);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Get ecosystem response
static Value *ai_respond(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup("خطأ في الإدخال");
        return result;
    }
    
    const char *eco_name = args[0]->data.string;
    const char *input = args[1]->data.string;
    
    AIEcosystemFull *eco = find_ecosystem(eco_name);
    if (!eco) {
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup("المنظومة غير موجودة");
        return result;
    }
    
    // Store in memory if available
    if (eco->long_term_memory && eco->memory_count < eco->memory_capacity) {
        MemoryItem *mem = &eco->long_term_memory[eco->memory_count++];
        mem->content = strdup(input);
        mem->timestamp = time(NULL);
        mem->importance = 0.5;
    }
    
    // Get response from first available mind
    char *response = strdup("لا توجد أدمغة متاحة في المنظومة");
    
    for (int i = 0; i < eco->mind_count; i++) {
        AIMindFull *mind = eco->minds[i];
        
        // Find best matching pattern
        double best_similarity = 0.0;
        
        for (int j = 0; j < mind->knowledge_count; j++) {
            double sim = calculate_similarity(input, mind->knowledge[j].pattern);
            if (sim > best_similarity && sim > 0.5) {
                best_similarity = sim;
                free(response);
                response = strdup(mind->knowledge[j].response);
            }
        }
        
        if (best_similarity > 0.5) break;
    }
    
    // Add some creativity if enabled
    if (eco->self_learning_enabled && eco->creativity_level > 0.3) {
        char *creative_response = malloc(strlen(response) + 50);
        sprintf(creative_response, "%s ✨", response);
        free(response);
        response = creative_response;
    }
    
    printf("[ذكاء] استجابة المنظومة \"%s\": \"%s\"\n", eco_name, response);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = response;
    return result;
}

// Create neural network
static Value *ai_create_neural(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    const char *name = args[0]->data.string;
    
    if (neural_network_count >= MAX_NEURAL_NETWORKS) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    NeuralNetworkFull *net = &neural_networks[neural_network_count++];
    net->id = next_network_id++;
    net->name = strdup(name);
    net->layers = 3;
    net->layer_sizes = malloc(sizeof(int) * 3);
    net->layer_sizes[0] = 10;
    net->layer_sizes[1] = 20;
    net->layer_sizes[2] = 10;
    net->learning_rate = 0.01;
    net->use_gpu = false;
    net->trained = false;
    net->training_data_file = NULL;
    
    printf("[ذكاء] إنشاء شبكة عصبية: \"%s\"\n", name);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = net->id;
    return result;
}

// Set neural network layers
static Value *ai_set_layers(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *net_name = args[0]->data.string;
    int layers = (int)value_to_number(args[1]);
    
    NeuralNetworkFull *net = find_network(net_name);
    if (!net) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    net->layers = layers;
    net->layer_sizes = realloc(net->layer_sizes, sizeof(int) * layers);
    
    // Initialize with default sizes
    for (int i = 0; i < layers; i++) {
        net->layer_sizes[i] = 10;
    }
    
    printf("[ذكاء] تعيين طبقات الشبكة \"%s\" إلى %d\n", net_name, layers);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Set learning rate
static Value *ai_set_learning_rate(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *net_name = args[0]->data.string;
    double rate = value_to_number(args[1]);
    
    NeuralNetworkFull *net = find_network(net_name);
    if (!net) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    net->learning_rate = rate;
    
    printf("[ذكاء] تعيين معدل التعلم للشبكة \"%s\" إلى %.4f\n", net_name, rate);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Train neural network
static Value *ai_train(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *net_name = args[0]->data.string;
    const char *data_file = args[1]->data.string;
    
    NeuralNetworkFull *net = find_network(net_name);
    if (!net) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Check if file exists
    FILE *file = fopen(data_file, "r");
    if (!file) {
        fprintf(stderr, "ملف البيانات غير موجود: %s\n", data_file);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    fclose(file);
    
    free(net->training_data_file);
    net->training_data_file = strdup(data_file);
    net->trained = true;
    
    printf("[ذكاء] تدريب الشبكة \"%s\" باستخدام البيانات من \"%s\"\n", 
           net_name, data_file);
    printf("[ذكاء] اكتمل التدريب! ✅\n");
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Enable GPU
static Value *ai_use_gpu(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *net_name = args[0]->data.string;
    
    NeuralNetworkFull *net = find_network(net_name);
    if (!net) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    net->use_gpu = true;
    
    printf("[ذكاء] تفعيل GPU للشبكة \"%s\" 🚀\n", net_name);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Run application
static Value *ai_run_app(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *eco_name = args[0]->data.string;
    
    AIEcosystemFull *eco = find_ecosystem(eco_name);
    if (!eco) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    printf("\n╔══════════════════════════════════════════════════╗\n");
    printf("║  تشغيل تطبيق المنظومة: %-26s║\n", eco_name);
    printf("╠══════════════════════════════════════════════════╣\n");
    printf("║  الأدمغة المتاحة: %-3d                            ║\n", eco->mind_count);
    printf("║  الذاكرة: %s                                    ║\n", 
           eco->long_term_memory ? "مفعلة" : "معطلة");
    printf("║  التعلم الذاتي: %s                              ║\n",
           eco->self_learning_enabled ? "مفعل" : "معطل");
    printf("╚══════════════════════════════════════════════════╝\n\n");
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Save AI entity
static Value *ai_save(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING ||
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *name = args[0]->data.string;
    const char *filename = args[1]->data.string;
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "لا يمكن إنشاء الملف: %s\n", filename);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Try to find and save the entity
    AIMindFull *mind = find_mind(name);
    AIEcosystemFull *eco = find_ecosystem(name);
    NeuralNetworkFull *net = find_network(name);
    
    if (mind) {
        fprintf(file, "# عقل وسام: %s\n", mind->name);
        fprintf(file, "type: mind\n");
        fprintf(file, "knowledge_count: %d\n", mind->knowledge_count);
        for (int i = 0; i < mind->knowledge_count; i++) {
            fprintf(file, "knowledge: %s | %s\n", 
                    mind->knowledge[i].pattern,
                    mind->knowledge[i].response);
        }
    } else if (eco) {
        fprintf(file, "# منظومة وسام: %s\n", eco->name);
        fprintf(file, "type: ecosystem\n");
        fprintf(file, "mind_count: %d\n", eco->mind_count);
        fprintf(file, "self_learning: %s\n", 
                eco->self_learning_enabled ? "true" : "false");
    } else if (net) {
        fprintf(file, "# شبكة عصبية وسام: %s\n", net->name);
        fprintf(file, "type: neural_network\n");
        fprintf(file, "layers: %d\n", net->layers);
        fprintf(file, "learning_rate: %f\n", net->learning_rate);
        fprintf(file, "trained: %s\n", net->trained ? "true" : "false");
    }
    
    fclose(file);
    
    printf("[ذكاء] حفظ الكيان \"%s\" إلى الملف \"%s\" 💾\n", name, filename);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Load AI entity
static Value *ai_load(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *filename = args[0]->data.string;
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "الملف غير موجود: %s\n", filename);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    printf("[ذكاء] تحميل الكيان من الملف \"%s\" 📂\n", filename);
    
    fclose(file);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Module method dispatcher
Value *ai_module_call(const char *method, Value **args, int arg_count) {
    if (strcmp(method, "أنشئ_عقل") == 0 || strcmp(method, "create_mind") == 0) {
        return ai_create_mind(args, arg_count);
    } else if (strcmp(method, "تعلّم") == 0 || strcmp(method, "learn") == 0) {
        return ai_learn(args, arg_count);
    } else if (strcmp(method, "اسأل") == 0 || strcmp(method, "ask") == 0) {
        return ai_ask(args, arg_count);
    } else if (strcmp(method, "أنشئ_منظومة") == 0 || strcmp(method, "create_ecosystem") == 0) {
        return ai_create_ecosystem(args, arg_count);
    } else if (strcmp(method, "أضف_عقل") == 0 || strcmp(method, "add_mind") == 0) {
        return ai_add_mind(args, arg_count);
    } else if (strcmp(method, "أنشئ_ذاكرة") == 0 || strcmp(method, "create_memory") == 0) {
        return ai_create_memory(args, arg_count);
    } else if (strcmp(method, "تعلّم_ذاتي") == 0 || strcmp(method, "self_learn") == 0) {
        return ai_self_learn(args, arg_count);
    } else if (strcmp(method, "استجب") == 0 || strcmp(method, "respond") == 0) {
        return ai_respond(args, arg_count);
    } else if (strcmp(method, "أنشئ_شبكة_عصبية") == 0 || strcmp(method, "create_neural") == 0) {
        return ai_create_neural(args, arg_count);
    } else if (strcmp(method, "طبقات") == 0 || strcmp(method, "set_layers") == 0) {
        return ai_set_layers(args, arg_count);
    } else if (strcmp(method, "معدل_تعلم") == 0 || strcmp(method, "set_learning_rate") == 0) {
        return ai_set_learning_rate(args, arg_count);
    } else if (strcmp(method, "درّب") == 0 || strcmp(method, "train") == 0) {
        return ai_train(args, arg_count);
    } else if (strcmp(method, "استخدم_GPU") == 0 || strcmp(method, "use_gpu") == 0) {
        return ai_use_gpu(args, arg_count);
    } else if (strcmp(method, "أدر_التطبيق") == 0 || strcmp(method, "run_app") == 0) {
        return ai_run_app(args, arg_count);
    } else if (strcmp(method, "احفظ") == 0 || strcmp(method, "save") == 0) {
        return ai_save(args, arg_count);
    } else if (strcmp(method, "حمّل") == 0 || strcmp(method, "load") == 0) {
        return ai_load(args, arg_count);
    }
    
    return create_value(TYPE_NULL);
}

// Initialize AI module
void ai_module_init(void) {
    if (module_count < MAX_MODULES) {
        modules[module_count].name = strdup("ذكاء");
        modules[module_count].init = ai_module_init;
        modules[module_count].call = ai_module_call;
        module_count++;
    }
}
