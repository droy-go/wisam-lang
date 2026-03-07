#include "wisam.h"
#include "interpreter.h"

// مخزن (Storage) Module - Standard library for data storage operations

#define MAX_DATABASES 100
#define MAX_RECORDS 10000
#define MAX_KEY_LEN 256
#define MAX_VALUE_LEN 4096

typedef struct {
    char *key;
    char *value;
    time_t created;
    time_t modified;
} StorageRecord;

typedef struct {
    char *name;
    StorageRecord *records;
    int record_count;
    int capacity;
    bool persistent;
    char *filename;
} StorageDatabase;

static StorageDatabase databases[MAX_DATABASES];
static int db_count = 0;

// Find database by name
static StorageDatabase *find_database(const char *name) {
    for (int i = 0; i < db_count; i++) {
        if (strcmp(databases[i].name, name) == 0) {
            return &databases[i];
        }
    }
    return NULL;
}

// Find record by key
static StorageRecord *find_record(StorageDatabase *db, const char *key) {
    for (int i = 0; i < db->record_count; i++) {
        if (strcmp(db->records[i].key, key) == 0) {
            return &db->records[i];
        }
    }
    return NULL;
}

// Serialize value to string
static char *serialize_value(Value *val) {
    if (!val) return strdup("null");
    
    char *result = malloc(MAX_VALUE_LEN);
    
    switch (val->type) {
        case TYPE_NUMBER:
            snprintf(result, MAX_VALUE_LEN, "num:%f", val->data.number);
            break;
        case TYPE_STRING:
            snprintf(result, MAX_VALUE_LEN, "str:%s", val->data.string);
            break;
        case TYPE_BOOL:
            snprintf(result, MAX_VALUE_LEN, "bool:%s", val->data.boolean ? "true" : "false");
            break;
        case TYPE_STRUCT:
            snprintf(result, MAX_VALUE_LEN, "struct:%s", val->data.strct->name);
            break;
        default:
            strcpy(result, "null");
            break;
    }
    
    return result;
}

// Deserialize string to value
static Value *deserialize_value(const char *str) {
    Value *val = create_value(TYPE_NULL);
    
    if (strncmp(str, "num:", 4) == 0) {
        val->type = TYPE_NUMBER;
        val->data.number = atof(str + 4);
    } else if (strncmp(str, "str:", 4) == 0) {
        val->type = TYPE_STRING;
        val->data.string = strdup(str + 4);
    } else if (strncmp(str, "bool:", 5) == 0) {
        val->type = TYPE_BOOL;
        val->data.boolean = (strcmp(str + 5, "true") == 0);
    }
    
    return val;
}

// Create a new database
static Value *storage_create(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *name = args[0]->data.string;
    
    // Check if database already exists
    if (find_database(name)) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = true; // Already exists, consider success
        return result;
    }
    
    if (db_count >= MAX_DATABASES) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Create new database
    StorageDatabase *db = &databases[db_count++];
    db->name = strdup(name);
    db->records = NULL;
    db->record_count = 0;
    db->capacity = 0;
    db->persistent = false;
    db->filename = NULL;
    
    // Check for persistent option
    if (arg_count >= 2 && args[1]->type == TYPE_STRING) {
        db->persistent = true;
        db->filename = strdup(args[1]->data.string);
        
        // Try to load existing data
        FILE *file = fopen(db->filename, "r");
        if (file) {
            char line[MAX_KEY_LEN + MAX_VALUE_LEN + 10];
            while (fgets(line, sizeof(line), file)) {
                // Parse line: key\tvalue\n
                char *tab = strchr(line, '\t');
                if (tab) {
                    *tab = '\0';
                    char *value = tab + 1;
                    
                    // Remove newline
                    char *nl = strchr(value, '\n');
                    if (nl) *nl = '\0';
                    
                    // Add record
                    if (db->record_count >= db->capacity) {
                        db->capacity = db->capacity * 2 + 10;
                        db->records = realloc(db->records, 
                                              sizeof(StorageRecord) * db->capacity);
                    }
                    
                    StorageRecord *rec = &db->records[db->record_count++];
                    rec->key = strdup(line);
                    rec->value = strdup(value);
                    rec->created = time(NULL);
                    rec->modified = time(NULL);
                }
            }
            fclose(file);
        }
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Add a record to database
static Value *storage_add(Value **args, int arg_count) {
    if (arg_count < 3 || args[0]->type != TYPE_STRING || 
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *db_name = args[0]->data.string;
    const char *key = args[1]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        fprintf(stderr, "Database not found: %s\n", db_name);
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Serialize value
    char *value_str = serialize_value(args[2]);
    
    // Check if key already exists
    StorageRecord *rec = find_record(db, key);
    if (rec) {
        // Update existing
        free(rec->value);
        rec->value = value_str;
        rec->modified = time(NULL);
    } else {
        // Add new record
        if (db->record_count >= db->capacity) {
            db->capacity = db->capacity * 2 + 10;
            db->records = realloc(db->records, 
                                  sizeof(StorageRecord) * db->capacity);
        }
        
        rec = &db->records[db->record_count++];
        rec->key = strdup(key);
        rec->value = value_str;
        rec->created = time(NULL);
        rec->modified = time(NULL);
    }
    
    // Save to file if persistent
    if (db->persistent && db->filename) {
        FILE *file = fopen(db->filename, "w");
        if (file) {
            for (int i = 0; i < db->record_count; i++) {
                fprintf(file, "%s\t%s\n", 
                        db->records[i].key, 
                        db->records[i].value);
            }
            fclose(file);
        }
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Read a record from database
static Value *storage_read(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || 
        args[1]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    const char *db_name = args[0]->data.string;
    const char *key = args[1]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        return create_value(TYPE_NULL);
    }
    
    StorageRecord *rec = find_record(db, key);
    if (!rec) {
        return create_value(TYPE_NULL);
    }
    
    return deserialize_value(rec->value);
}

// Delete a record from database
static Value *storage_delete(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || 
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *db_name = args[0]->data.string;
    const char *key = args[1]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Find and remove record
    for (int i = 0; i < db->record_count; i++) {
        if (strcmp(db->records[i].key, key) == 0) {
            free(db->records[i].key);
            free(db->records[i].value);
            
            // Shift remaining records
            for (int j = i; j < db->record_count - 1; j++) {
                db->records[j] = db->records[j + 1];
            }
            db->record_count--;
            
            // Save to file if persistent
            if (db->persistent && db->filename) {
                FILE *file = fopen(db->filename, "w");
                if (file) {
                    for (int i = 0; i < db->record_count; i++) {
                        fprintf(file, "%s\t%s\n", 
                                db->records[i].key, 
                                db->records[i].value);
                    }
                    fclose(file);
                }
            }
            
            Value *result = create_value(TYPE_BOOL);
            result->data.boolean = true;
            return result;
        }
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = false;
    return result;
}

// List all keys in database
static Value *storage_keys(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    const char *db_name = args[0]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        return create_value(TYPE_NULL);
    }
    
    Value *result = create_value(TYPE_ARRAY);
    result->data.array = malloc(sizeof(Array));
    result->data.array->items = malloc(sizeof(Value *) * db->record_count);
    result->data.array->count = db->record_count;
    result->data.array->capacity = db->record_count;
    
    for (int i = 0; i < db->record_count; i++) {
        Value *item = create_value(TYPE_STRING);
        item->data.string = strdup(db->records[i].key);
        result->data.array->items[i] = item;
    }
    
    return result;
}

// Get count of records
static Value *storage_count(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = 0;
        return result;
    }
    
    const char *db_name = args[0]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = 0;
        return result;
    }
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = db->record_count;
    return result;
}

// Clear all records
static Value *storage_clear(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *db_name = args[0]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Free all records
    for (int i = 0; i < db->record_count; i++) {
        free(db->records[i].key);
        free(db->records[i].value);
    }
    free(db->records);
    db->records = NULL;
    db->record_count = 0;
    db->capacity = 0;
    
    // Clear file if persistent
    if (db->persistent && db->filename) {
        FILE *file = fopen(db->filename, "w");
        if (file) fclose(file);
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Check if key exists
static Value *storage_exists(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || 
        args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *db_name = args[0]->data.string;
    const char *key = args[1]->data.string;
    
    StorageDatabase *db = find_database(db_name);
    if (!db) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = (find_record(db, key) != NULL);
    return result;
}

// Delete database
static Value *storage_drop(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    const char *db_name = args[0]->data.string;
    
    for (int i = 0; i < db_count; i++) {
        if (strcmp(databases[i].name, db_name) == 0) {
            // Free all records
            for (int j = 0; j < databases[i].record_count; j++) {
                free(databases[i].records[j].key);
                free(databases[i].records[j].value);
            }
            free(databases[i].records);
            free(databases[i].name);
            free(databases[i].filename);
            
            // Shift remaining databases
            for (int j = i; j < db_count - 1; j++) {
                databases[j] = databases[j + 1];
            }
            db_count--;
            
            Value *result = create_value(TYPE_BOOL);
            result->data.boolean = true;
            return result;
        }
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = false;
    return result;
}

// Module method dispatcher
Value *storage_module_call(const char *method, Value **args, int arg_count) {
    if (strcmp(method, "أنشئ") == 0 || strcmp(method, "create") == 0) {
        return storage_create(args, arg_count);
    } else if (strcmp(method, "أضف") == 0 || strcmp(method, "add") == 0) {
        return storage_add(args, arg_count);
    } else if (strcmp(method, "اقرأ") == 0 || strcmp(method, "read") == 0) {
        return storage_read(args, arg_count);
    } else if (strcmp(method, "احذف") == 0 || strcmp(method, "delete") == 0) {
        return storage_delete(args, arg_count);
    } else if (strcmp(method, "المفاتيح") == 0 || strcmp(method, "keys") == 0) {
        return storage_keys(args, arg_count);
    } else if (strcmp(method, "العدد") == 0 || strcmp(method, "count") == 0) {
        return storage_count(args, arg_count);
    } else if (strcmp(method, "امسح") == 0 || strcmp(method, "clear") == 0) {
        return storage_clear(args, arg_count);
    } else if (strcmp(method, "موجود") == 0 || strcmp(method, "exists") == 0) {
        return storage_exists(args, arg_count);
    } else if (strcmp(method, "احذف_قاعدة") == 0 || strcmp(method, "drop") == 0) {
        return storage_drop(args, arg_count);
    }
    
    return create_value(TYPE_NULL);
}

// Initialize storage module
void storage_module_init(void) {
    if (module_count < MAX_MODULES) {
        modules[module_count].name = strdup("مخزن");
        modules[module_count].init = storage_module_init;
        modules[module_count].call = storage_module_call;
        module_count++;
    }
}
