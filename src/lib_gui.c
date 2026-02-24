#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WINDOWS 10
#define MAX_WIDGETS 50

typedef enum {
    WIDGET_BUTTON,
    WIDGET_LABEL,
    WIDGET_INPUT,
    WIDGET_CHECKBOX,
    WIDGET_IMAGE
} WidgetType;

typedef struct {
    char *id;
    WidgetType type;
    char *text;
    int x, y;
    int width, height;
    void (*callback)(void);
} Widget;

typedef struct {
    char *title;
    int width, height;
    bool visible;
    Widget widgets[MAX_WIDGETS];
    int widget_count;
} Window;

static Window windows[MAX_WINDOWS];
static int window_count = 0;
static int active_window = -1;

// إنشاء نافذة جديدة
Value lib_gui_create_window(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: إنشاء_نافذة يتطلب عنواناً\n");
        return value_create_null();
    }
    
    char *title = args[0].as.string;
    int width = 800;
    int height = 600;
    
    if (arg_count >= 2 && args[1].type == VAL_NUMBER) {
        width = (int)args[1].as.number;
    }
    
    if (arg_count >= 3 && args[2].type == VAL_NUMBER) {
        height = (int)args[2].as.number;
    }
    
    if (window_count >= MAX_WINDOWS) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للنوافذ\n");
        return value_create_null();
    }
    
    windows[window_count].title = strdup(title);
    windows[window_count].width = width;
    windows[window_count].height = height;
    windows[window_count].visible = true;
    windows[window_count].widget_count = 0;
    
    printf("[واجهات] إنشاء نافذة: %s (%dx%d)\n", title, width, height);
    
    active_window = window_count;
    window_count++;
    
    return value_create_number(active_window);
}

// إضافة زر
Value lib_gui_add_button(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: إضافة_زر يتطلب نصاً\n");
        return value_create_null();
    }
    
    Window *win = &windows[active_window];
    
    if (win->widget_count >= MAX_WIDGETS) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للعناصر\n");
        return value_create_null();
    }
    
    Widget *w = &win->widgets[win->widget_count];
    w->id = strdup(args[0].as.string);
    w->type = WIDGET_BUTTON;
    w->text = strdup(args[0].as.string);
    w->x = 10;
    w->y = 10 + win->widget_count * 40;
    w->width = 100;
    w->height = 30;
    w->callback = NULL;
    
    printf("[واجهات] إضافة زر: '%s' في (%d, %d)\n", w->text, w->x, w->y);
    
    win->widget_count++;
    
    return value_create_boolean(true);
}

// إضافة نص
Value lib_gui_add_label(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: إضافة_نص يتطلب نصاً\n");
        return value_create_null();
    }
    
    Window *win = &windows[active_window];
    
    if (win->widget_count >= MAX_WIDGETS) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للعناصر\n");
        return value_create_null();
    }
    
    Widget *w = &win->widgets[win->widget_count];
    w->id = strdup(args[0].as.string);
    w->type = WIDGET_LABEL;
    w->text = strdup(args[0].as.string);
    w->x = 10;
    w->y = 10 + win->widget_count * 30;
    
    printf("[واجهات] إضافة نص: '%s' في (%d, %d)\n", w->text, w->x, w->y);
    
    win->widget_count++;
    
    return value_create_boolean(true);
}

// إضافة حقل إدخال
Value lib_gui_add_input(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    char *placeholder = "";
    if (arg_count >= 1 && args[0].type == VAL_STRING) {
        placeholder = args[0].as.string;
    }
    
    Window *win = &windows[active_window];
    
    if (win->widget_count >= MAX_WIDGETS) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للعناصر\n");
        return value_create_null();
    }
    
    Widget *w = &win->widgets[win->widget_count];
    w->id = strdup(placeholder);
    w->type = WIDGET_INPUT;
    w->text = strdup(placeholder);
    w->x = 10;
    w->y = 10 + win->widget_count * 35;
    w->width = 200;
    w->height = 25;
    
    printf("[واجهات] إضافة حقل إدخال في (%d, %d) placeholder: '%s'\n", 
           w->x, w->y, placeholder);
    
    win->widget_count++;
    
    return value_create_boolean(true);
}

// إضافة صورة
Value lib_gui_add_image(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: إضافة_صورة يتطلب اسم الملف\n");
        return value_create_null();
    }
    
    Window *win = &windows[active_window];
    
    if (win->widget_count >= MAX_WIDGETS) {
        fprintf(stderr, "خطأ: تجاوز الحد الأقصى للعناصر\n");
        return value_create_null();
    }
    
    Widget *w = &win->widgets[win->widget_count];
    w->id = strdup(args[0].as.string);
    w->type = WIDGET_IMAGE;
    w->text = strdup(args[0].as.string);
    w->x = 10;
    w->y = 10 + win->widget_count * 100;
    w->width = 200;
    w->height = 150;
    
    printf("[واجهات] إضافة صورة: '%s' في (%d, %d)\n", w->text, w->x, w->y);
    
    win->widget_count++;
    
    return value_create_boolean(true);
}

// عرض النافذة
Value lib_gui_show_window(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    Window *win = &windows[active_window];
    win->visible = true;
    
    printf("[واجهات] عرض النافذة: %s\n", win->title);
    printf("  الأبعاد: %dx%d\n", win->width, win->height);
    printf("  العناصر: %d\n", win->widget_count);
    
    return value_create_boolean(true);
}

// إخفاء النافذة
Value lib_gui_hide_window(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    windows[active_window].visible = false;
    printf("[واجهات] إخفاء النافذة\n");
    
    return value_create_boolean(true);
}

// إغلاق النافذة
Value lib_gui_close_window(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    Window *win = &windows[active_window];
    printf("[واجهات] إغلاق النافذة: %s\n", win->title);
    
    // تحرير الذاكرة
    free(win->title);
    for (int i = 0; i < win->widget_count; i++) {
        free(win->widgets[i].id);
        free(win->widgets[i].text);
    }
    
    // إزالة النافذة
    for (int i = active_window; i < window_count - 1; i++) {
        windows[i] = windows[i + 1];
    }
    window_count--;
    active_window = -1;
    
    return value_create_boolean(true);
}

// تعيين عنوان النافذة
Value lib_gui_set_title(Value *args, int arg_count) {
    if (active_window < 0 || active_window >= window_count) {
        fprintf(stderr, "خطأ: لا يوجد نافذة نشطة\n");
        return value_create_null();
    }
    
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: تعيين_عنوان يتطلب نصاً\n");
        return value_create_null();
    }
    
    free(windows[active_window].title);
    windows[active_window].title = strdup(args[0].as.string);
    
    printf("[واجهات] تغيير العنوان إلى: %s\n", args[0].as.string);
    
    return value_create_boolean(true);
}

// الحصول على قيمة إدخال
Value lib_gui_get_input(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: احصل_على_إدخال يتطلب معرف الحقل\n");
        return value_create_null();
    }
    
    // في التطبيق الحقيقي، سيتم قراءة القيمة من حقل الإدخال
    // هنا نقوم بمحاكاة
    printf("[واجهات] قراءة قيمة حقل الإدخال: %s\n", args[0].as.string);
    
    return value_create_string("");
}

// تعيين نص عنصر
Value lib_gui_set_text(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: تعيين_نص يتطلب معرف العنصر والنص\n");
        return value_create_null();
    }
    
    printf("[واجهات] تغيير نص '%s' إلى: %s\n", args[0].as.string, args[1].as.string);
    
    return value_create_boolean(true);
}

// تشغيل حلقة الأحداث
Value lib_gui_run_loop(Value *args, int arg_count) {
    printf("[واجهات] تشغيل حلقة الأحداث...\n");
    printf("  (اضغط Ctrl+C للخروج)\n");
    
    // في التطبيق الحقيقي، سيتم تشغيل حلقة أحداث GUI
    // هنا نقوم بمحاكاة
    
    return value_create_boolean(true);
}
