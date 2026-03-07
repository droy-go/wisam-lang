#include "wisam.h"
#include "interpreter.h"

// واجهات (UI) Module - Standard library for user interface operations

#define MAX_WINDOWS 10
#define MAX_WIDGETS 100

typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_TEXT_INPUT,
    WIDGET_CHECKBOX,
    WIDGET_RADIO,
    WIDGET_LIST,
    WIDGET_IMAGE
} WidgetType;

typedef struct {
    int id;
    WidgetType type;
    char *text;
    int x, y;
    int width, height;
    bool visible;
    bool enabled;
    void (*on_click)(void);
    void (*on_change)(const char *value);
} Widget;

typedef struct {
    int id;
    char *title;
    int x, y;
    int width, height;
    bool visible;
    Widget *widgets;
    int widget_count;
} Window;

static Window windows[MAX_WINDOWS];
static int window_count = 0;
static int next_widget_id = 1;
static int next_window_id = 1;

// Find window by ID
static Window *find_window(int id) {
    for (int i = 0; i < window_count; i++) {
        if (windows[i].id == id) {
            return &windows[i];
        }
    }
    return NULL;
}

// Find widget by ID
static Widget *find_widget(Window *win, int id) {
    for (int i = 0; i < win->widget_count; i++) {
        if (win->widgets[i].id == id) {
            return &win->widgets[i];
        }
    }
    return NULL;
}

// Create window
static Value *ui_create_window(Value **args, int arg_count) {
    const char *title = (arg_count >= 1 && args[0]->type == TYPE_STRING) 
                        ? args[0]->data.string : "نافذة وسام";
    
    int width = (arg_count >= 2) ? (int)value_to_number(args[1]) : 800;
    int height = (arg_count >= 3) ? (int)value_to_number(args[2]) : 600;
    
    if (window_count >= MAX_WINDOWS) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    Window *win = &windows[window_count++];
    win->id = next_window_id++;
    win->title = strdup(title);
    win->x = 100;
    win->y = 100;
    win->width = width;
    win->height = height;
    win->visible = true;
    win->widgets = NULL;
    win->widget_count = 0;
    
    printf("[واجهات] إنشاء نافذة: \"%s\" (%dx%d)\n", title, width, height);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = win->id;
    return result;
}

// Close window
static Value *ui_close_window(Value **args, int arg_count) {
    if (arg_count < 1) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    
    for (int i = 0; i < window_count; i++) {
        if (windows[i].id == window_id) {
            free(windows[i].title);
            for (int j = 0; j < windows[i].widget_count; j++) {
                free(windows[i].widgets[j].text);
            }
            free(windows[i].widgets);
            
            // Shift remaining windows
            for (int j = i; j < window_count - 1; j++) {
                windows[j] = windows[j + 1];
            }
            window_count--;
            
            Value *result = create_value(TYPE_BOOL);
            result->data.boolean = true;
            return result;
        }
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = false;
    return result;
}

// Set window title
static Value *ui_set_title(Value **args, int arg_count) {
    if (arg_count < 2 || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    const char *title = args[1]->data.string;
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    free(win->title);
    win->title = strdup(title);
    
    printf("[واجهات] تغيير عنوان النافذة: \"%s\"\n", title);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Set window size
static Value *ui_set_size(Value **args, int arg_count) {
    if (arg_count < 3) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    int width = (int)value_to_number(args[1]);
    int height = (int)value_to_number(args[2]);
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    win->width = width;
    win->height = height;
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Add button
static Value *ui_add_button(Value **args, int arg_count) {
    if (arg_count < 2 || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    const char *text = args[1]->data.string;
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    win->widgets = realloc(win->widgets, sizeof(Widget) * (win->widget_count + 1));
    Widget *widget = &win->widgets[win->widget_count++];
    
    widget->id = next_widget_id++;
    widget->type = WIDGET_BUTTON;
    widget->text = strdup(text);
    widget->x = 10;
    widget->y = 10 + (win->widget_count - 1) * 40;
    widget->width = 100;
    widget->height = 30;
    widget->visible = true;
    widget->enabled = true;
    widget->on_click = NULL;
    widget->on_change = NULL;
    
    printf("[واجهات] إضافة زر: \"%s\"\n", text);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = widget->id;
    return result;
}

// Add label
static Value *ui_add_label(Value **args, int arg_count) {
    if (arg_count < 2 || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    const char *text = args[1]->data.string;
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    win->widgets = realloc(win->widgets, sizeof(Widget) * (win->widget_count + 1));
    Widget *widget = &win->widgets[win->widget_count++];
    
    widget->id = next_widget_id++;
    widget->type = WIDGET_LABEL;
    widget->text = strdup(text);
    widget->x = 10;
    widget->y = 10 + (win->widget_count - 1) * 40;
    widget->width = 200;
    widget->height = 20;
    widget->visible = true;
    widget->enabled = true;
    widget->on_click = NULL;
    widget->on_change = NULL;
    
    printf("[واجهات] إضافة تسمية: \"%s\"\n", text);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = widget->id;
    return result;
}

// Add text input
static Value *ui_add_input(Value **args, int arg_count) {
    if (arg_count < 2 || args[1]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    const char *placeholder = args[1]->data.string;
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = -1;
        return result;
    }
    
    win->widgets = realloc(win->widgets, sizeof(Widget) * (win->widget_count + 1));
    Widget *widget = &win->widgets[win->widget_count++];
    
    widget->id = next_widget_id++;
    widget->type = WIDGET_TEXT_INPUT;
    widget->text = strdup(placeholder);
    widget->x = 10;
    widget->y = 10 + (win->widget_count - 1) * 40;
    widget->width = 200;
    widget->height = 25;
    widget->visible = true;
    widget->enabled = true;
    widget->on_click = NULL;
    widget->on_change = NULL;
    
    printf("[واجهات] إضافة حقل إدخال\n");
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = widget->id;
    return result;
}

// Set widget text
static Value *ui_set_text(Value **args, int arg_count) {
    if (arg_count < 3 || args[2]->type != TYPE_STRING) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    int widget_id = (int)value_to_number(args[1]);
    const char *text = args[2]->data.string;
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    Widget *widget = find_widget(win, widget_id);
    if (!widget) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    free(widget->text);
    widget->text = strdup(text);
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Get widget text
static Value *ui_get_text(Value **args, int arg_count) {
    if (arg_count < 2) {
        return create_value(TYPE_NULL);
    }
    
    int window_id = (int)value_to_number(args[0]);
    int widget_id = (int)value_to_number(args[1]);
    
    Window *win = find_window(window_id);
    if (!win) {
        return create_value(TYPE_NULL);
    }
    
    Widget *widget = find_widget(win, widget_id);
    if (!widget) {
        return create_value(TYPE_NULL);
    }
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(widget->text);
    return result;
}

// Show message box
static Value *ui_message(Value **args, int arg_count) {
    const char *message = (arg_count >= 1 && args[0]->type == TYPE_STRING) 
                          ? args[0]->data.string : "";
    const char *title = (arg_count >= 2 && args[1]->type == TYPE_STRING) 
                        ? args[1]->data.string : "رسالة";
    
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║  %s\n", title);
    printf("╠══════════════════════════════════════╣\n");
    printf("║  %s\n", message);
    printf("╚══════════════════════════════════════╝\n\n");
    
    return create_value(TYPE_NULL);
}

// Show confirmation dialog
static Value *ui_confirm(Value **args, int arg_count) {
    const char *message = (arg_count >= 1 && args[0]->type == TYPE_STRING) 
                          ? args[0]->data.string : "";
    
    printf("\n%s (نعم/لا): ", message);
    
    char response[10];
    if (fgets(response, sizeof(response), stdin)) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = (response[0] == 'ن' || response[0] == 'y' || 
                                response[0] == 'Y');
        return result;
    }
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = false;
    return result;
}

// Show input dialog
static Value *ui_input_dialog(Value **args, int arg_count) {
    const char *prompt = (arg_count >= 1 && args[0]->type == TYPE_STRING) 
                         ? args[0]->data.string : "أدخل قيمة:";
    
    printf("\n%s ", prompt);
    
    char input[256];
    if (fgets(input, sizeof(input), stdin)) {
        // Remove newline
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        Value *result = create_value(TYPE_STRING);
        result->data.string = strdup(input);
        return result;
    }
    
    return create_value(TYPE_NULL);
}

// Clear window
static Value *ui_clear(Value **args, int arg_count) {
    if (arg_count < 1) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    int window_id = (int)value_to_number(args[0]);
    
    Window *win = find_window(window_id);
    if (!win) {
        Value *result = create_value(TYPE_BOOL);
        result->data.boolean = false;
        return result;
    }
    
    // Free all widgets
    for (int i = 0; i < win->widget_count; i++) {
        free(win->widgets[i].text);
    }
    free(win->widgets);
    win->widgets = NULL;
    win->widget_count = 0;
    
    Value *result = create_value(TYPE_BOOL);
    result->data.boolean = true;
    return result;
}

// Run event loop (placeholder)
static Value *ui_run(Value **args, int arg_count) {
    printf("[واجهات] تشغيل حلقة الأحداث...\n");
    printf("(اضغط Enter للإنهاء)\n");
    getchar();
    
    return create_value(TYPE_NULL);
}

// Module method dispatcher
Value *ui_module_call(const char *method, Value **args, int arg_count) {
    if (strcmp(method, "أنشئ_نافذة") == 0 || strcmp(method, "create_window") == 0) {
        return ui_create_window(args, arg_count);
    } else if (strcmp(method, "أغلق_النافذة") == 0 || strcmp(method, "close_window") == 0) {
        return ui_close_window(args, arg_count);
    } else if (strcmp(method, "عنوان_النافذة") == 0 || strcmp(method, "set_title") == 0) {
        return ui_set_title(args, arg_count);
    } else if (strcmp(method, "حجم_النافذة") == 0 || strcmp(method, "set_size") == 0) {
        return ui_set_size(args, arg_count);
    } else if (strcmp(method, "أضف_زر") == 0 || strcmp(method, "add_button") == 0) {
        return ui_add_button(args, arg_count);
    } else if (strcmp(method, "أضف_تسمية") == 0 || strcmp(method, "add_label") == 0) {
        return ui_add_label(args, arg_count);
    } else if (strcmp(method, "أضف_إدخال") == 0 || strcmp(method, "add_input") == 0) {
        return ui_add_input(args, arg_count);
    } else if (strcmp(method, "عيّن_النص") == 0 || strcmp(method, "set_text") == 0) {
        return ui_set_text(args, arg_count);
    } else if (strcmp(method, "احصل_على_النص") == 0 || strcmp(method, "get_text") == 0) {
        return ui_get_text(args, arg_count);
    } else if (strcmp(method, "رسالة") == 0 || strcmp(method, "message") == 0) {
        return ui_message(args, arg_count);
    } else if (strcmp(method, "تأكيد") == 0 || strcmp(method, "confirm") == 0) {
        return ui_confirm(args, arg_count);
    } else if (strcmp(method, "إدخال") == 0 || strcmp(method, "input_dialog") == 0) {
        return ui_input_dialog(args, arg_count);
    } else if (strcmp(method, "امسح") == 0 || strcmp(method, "clear") == 0) {
        return ui_clear(args, arg_count);
    } else if (strcmp(method, "شغّل") == 0 || strcmp(method, "run") == 0) {
        return ui_run(args, arg_count);
    }
    
    return create_value(TYPE_NULL);
}

// Initialize UI module
void ui_module_init(void) {
    if (module_count < MAX_MODULES) {
        modules[module_count].name = strdup("واجهات");
        modules[module_count].init = ui_module_init;
        modules[module_count].call = ui_module_call;
        module_count++;
    }
}
