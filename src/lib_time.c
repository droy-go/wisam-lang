#include "wisam.h"
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

// الحصول على الوقت الحالي
Value lib_time_now(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[26];
    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", tm_info);
    
    return value_create_string(buffer);
}

// تنسيق الوقت
Value lib_time_format(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: صيّغ يتطلب نمط تنسيق\n");
        return value_create_null();
    }
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[256];
    char *format = args[0].as.string;
    
    // تحويل بعض التنسيقات العربية
    char c_format[256];
    strcpy(c_format, format);
    
    // استبدال بعض الكلمات العربية
    if (strstr(c_format, "سنة")) {
        char temp[256];
        snprintf(temp, sizeof(temp), "%d", tm_info->tm_year + 1900);
        strcpy(c_format, temp);
    } else if (strstr(c_format, "شهر")) {
        char temp[256];
        snprintf(temp, sizeof(temp), "%d", tm_info->tm_mon + 1);
        strcpy(c_format, temp);
    } else if (strstr(c_format, "يوم")) {
        char temp[256];
        snprintf(temp, sizeof(temp), "%d", tm_info->tm_mday);
        strcpy(c_format, temp);
    }
    
    strftime(buffer, sizeof(buffer), c_format, tm_info);
    
    return value_create_string(buffer);
}

// تأخير التنفيذ
Value lib_time_sleep(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: تأخير يتطلب عدداً من الثواني\n");
        return value_create_null();
    }
    
    double seconds = args[0].as.number;
    usleep((useconds_t)(seconds * 1000000));
    
    return value_create_null();
}

// الحصول على السنة
Value lib_time_year(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return value_create_number(tm_info->tm_year + 1900);
}

// الحصول على الشهر
Value lib_time_month(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return value_create_number(tm_info->tm_mon + 1);
}

// الحصول على اليوم
Value lib_time_day(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return value_create_number(tm_info->tm_mday);
}

// الحصول على الساعة
Value lib_time_hour(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return value_create_number(tm_info->tm_hour);
}

// الحصول على الدقيقة
Value lib_time_minute(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return value_create_number(tm_info->tm_min);
}

// الحصول على الثانية
Value lib_time_second(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    return value_create_number(tm_info->tm_sec);
}

// الحصول على اليوم من الأسبوع
Value lib_time_weekday(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char *days[] = {"الأحد", "الإثنين", "الثلاثاء", "الأربعاء", 
                    "الخميس", "الجمعة", "السبت"};
    
    return value_create_string(days[tm_info->tm_wday]);
}

// الحصول على اسم الشهر
Value lib_time_month_name(Value *args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char *months[] = {"يناير", "فبراير", "مارس", "إبريل", "مايو", "يونيو",
                      "يوليو", "أغسطس", "سبتمبر", "أكتوبر", "نوفمبر", "ديسمبر"};
    
    return value_create_string(months[tm_info->tm_mon]);
}

// إنشاء مؤقت
Value lib_time_timer(Value *args, int arg_count) {
    static time_t start_time = 0;
    
    if (arg_count >= 1 && args[0].type == VAL_STRING) {
        if (strcmp(args[0].as.string, "ابدأ") == 0 || 
            strcmp(args[0].as.string, "start") == 0) {
            start_time = time(NULL);
            return value_create_boolean(true);
        } else if (strcmp(args[0].as.string, "توقف") == 0 || 
                   strcmp(args[0].as.string, "stop") == 0) {
            if (start_time == 0) {
                return value_create_number(0);
            }
            double elapsed = difftime(time(NULL), start_time);
            start_time = 0;
            return value_create_number(elapsed);
        } else if (strcmp(args[0].as.string, "اقرأ") == 0 || 
                   strcmp(args[0].as.string, "read") == 0) {
            if (start_time == 0) {
                return value_create_number(0);
            }
            return value_create_number(difftime(time(NULL), start_time));
        }
    }
    
    return value_create_null();
}

// قياس الوقت المنقضي
Value lib_time_elapsed(Value *args, int arg_count) {
    static time_t markers[10] = {0};
    
    if (arg_count >= 1 && args[0].type == VAL_NUMBER) {
        int marker_id = (int)args[0].as.number;
        if (marker_id < 0 || marker_id >= 10) {
            return value_create_null();
        }
        
        if (arg_count >= 2 && args[1].type == VAL_STRING) {
            if (strcmp(args[1].as.string, "ضع") == 0 || 
                strcmp(args[1].as.string, "set") == 0) {
                markers[marker_id] = time(NULL);
                return value_create_boolean(true);
            }
        }
        
        if (markers[marker_id] == 0) {
            return value_create_number(0);
        }
        
        return value_create_number(difftime(time(NULL), markers[marker_id]));
    }
    
    return value_create_null();
}
