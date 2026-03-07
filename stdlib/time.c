#include "wisam.h"
#include "interpreter.h"
#include <sys/time.h>

// زمن (Time) Module - Standard library for time operations

// Get current timestamp
static Value *time_now(Value **args, int arg_count) {
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = (double)time(NULL);
    return result;
}

// Get current date as string
static Value *time_date(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm_info);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(buffer);
    return result;
}

// Get current time as string
static Value *time_time(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", tm_info);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(buffer);
    return result;
}

// Get datetime as string
static Value *time_datetime(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[64];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(buffer);
    return result;
}

// Get year
static Value *time_year(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_year + 1900;
    return result;
}

// Get month (1-12)
static Value *time_month(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_mon + 1;
    return result;
}

// Get day of month (1-31)
static Value *time_day(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_mday;
    return result;
}

// Get hour (0-23)
static Value *time_hour(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_hour;
    return result;
}

// Get minute (0-59)
static Value *time_minute(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_min;
    return result;
}

// Get second (0-59)
static Value *time_second(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_sec;
    return result;
}

// Get day of week (0-6, Sunday = 0)
static Value *time_weekday(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_wday;
    return result;
}

// Get day of year (0-365)
static Value *time_yearday(Value **args, int arg_count) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = tm_info->tm_yday;
    return result;
}

// Sleep for specified seconds
static Value *time_sleep(Value **args, int arg_count) {
    if (arg_count < 1) {
        return create_value(TYPE_NULL);
    }
    
    double seconds = value_to_number(args[0]);
    
    #ifdef _WIN32
        Sleep((DWORD)(seconds * 1000));
    #else
        usleep((useconds_t)(seconds * 1000000));
    #endif
    
    return create_value(TYPE_NULL);
}

// Format time
static Value *time_format(Value **args, int arg_count) {
    if (arg_count < 1 || args[0]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    
    char buffer[256];
    strftime(buffer, sizeof(buffer), args[0]->data.string, tm_info);
    
    Value *result = create_value(TYPE_STRING);
    result->data.string = strdup(buffer);
    return result;
}

// Parse time string to timestamp
static Value *time_parse(Value **args, int arg_count) {
    if (arg_count < 2 || args[0]->type != TYPE_STRING || args[1]->type != TYPE_STRING) {
        return create_value(TYPE_NULL);
    }
    
    struct tm tm_info = {0};
    char *str = args[0]->data.string;
    char *format = args[1]->data.string;
    
    if (strptime(str, format, &tm_info) != NULL) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = (double)mktime(&tm_info);
        return result;
    }
    
    return create_value(TYPE_NULL);
}

// Add time (seconds)
static Value *time_add(Value **args, int arg_count) {
    if (arg_count < 2) {
        return create_value(TYPE_NULL);
    }
    
    time_t base = (time_t)value_to_number(args[0]);
    double seconds = value_to_number(args[1]);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = (double)(base + (time_t)seconds);
    return result;
}

// Get difference between two times
static Value *time_diff(Value **args, int arg_count) {
    if (arg_count < 2) {
        return create_value(TYPE_NULL);
    }
    
    time_t t1 = (time_t)value_to_number(args[0]);
    time_t t2 = (time_t)value_to_number(args[1]);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = (double)difftime(t1, t2);
    return result;
}

// Get microseconds precision time
static Value *time_microseconds(Value **args, int arg_count) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = (double)(tv.tv_sec * 1000000 + tv.tv_usec);
    return result;
}

// Get milliseconds precision time
static Value *time_milliseconds(Value **args, int arg_count) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = (double)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
    return result;
}

// Timer - start
static struct timeval timer_start;
static bool timer_running = false;

static Value *time_timer_start(Value **args, int arg_count) {
    gettimeofday(&timer_start, NULL);
    timer_running = true;
    return create_value(TYPE_NULL);
}

// Timer - elapsed
static Value *time_timer_elapsed(Value **args, int arg_count) {
    if (!timer_running) {
        Value *result = create_value(TYPE_NUMBER);
        result->data.number = 0;
        return result;
    }
    
    struct timeval now;
    gettimeofday(&now, NULL);
    
    double elapsed = (now.tv_sec - timer_start.tv_sec) * 1000.0;
    elapsed += (now.tv_usec - timer_start.tv_usec) / 1000.0;
    
    Value *result = create_value(TYPE_NUMBER);
    result->data.number = elapsed;
    return result;
}

// Timer - stop
static Value *time_timer_stop(Value **args, int arg_count) {
    Value *elapsed = time_timer_elapsed(args, arg_count);
    timer_running = false;
    return elapsed;
}

// Module method dispatcher
Value *time_module_call(const char *method, Value **args, int arg_count) {
    if (strcmp(method, "الآن") == 0 || strcmp(method, "now") == 0) {
        return time_now(args, arg_count);
    } else if (strcmp(method, "التاريخ") == 0 || strcmp(method, "date") == 0) {
        return time_date(args, arg_count);
    } else if (strcmp(method, "الوقت") == 0 || strcmp(method, "time") == 0) {
        return time_time(args, arg_count);
    } else if (strcmp(method, "التاريخ_والوقت") == 0 || strcmp(method, "datetime") == 0) {
        return time_datetime(args, arg_count);
    } else if (strcmp(method, "السنة") == 0 || strcmp(method, "year") == 0) {
        return time_year(args, arg_count);
    } else if (strcmp(method, "الشهر") == 0 || strcmp(method, "month") == 0) {
        return time_month(args, arg_count);
    } else if (strcmp(method, "اليوم") == 0 || strcmp(method, "day") == 0) {
        return time_day(args, arg_count);
    } else if (strcmp(method, "الساعة") == 0 || strcmp(method, "hour") == 0) {
        return time_hour(args, arg_count);
    } else if (strcmp(method, "الدقيقة") == 0 || strcmp(method, "minute") == 0) {
        return time_minute(args, arg_count);
    } else if (strcmp(method, "الثانية") == 0 || strcmp(method, "second") == 0) {
        return time_second(args, arg_count);
    } else if (strcmp(method, "يوم_الأسبوع") == 0 || strcmp(method, "weekday") == 0) {
        return time_weekday(args, arg_count);
    } else if (strcmp(method, "يوم_السنة") == 0 || strcmp(method, "yearday") == 0) {
        return time_yearday(args, arg_count);
    } else if (strcmp(method, "نم") == 0 || strcmp(method, "sleep") == 0) {
        return time_sleep(args, arg_count);
    } else if (strcmp(method, "نسّق") == 0 || strcmp(method, "format") == 0) {
        return time_format(args, arg_count);
    } else if (strcmp(method, "حلّل") == 0 || strcmp(method, "parse") == 0) {
        return time_parse(args, arg_count);
    } else if (strcmp(method, "أضف") == 0 || strcmp(method, "add") == 0) {
        return time_add(args, arg_count);
    } else if (strcmp(method, "الفرق") == 0 || strcmp(method, "diff") == 0) {
        return time_diff(args, arg_count);
    } else if (strcmp(method, "الميكروثواني") == 0 || strcmp(method, "microseconds") == 0) {
        return time_microseconds(args, arg_count);
    } else if (strcmp(method, "المليثواني") == 0 || strcmp(method, "milliseconds") == 0) {
        return time_milliseconds(args, arg_count);
    } else if (strcmp(method, "ابدأ_المؤقت") == 0 || strcmp(method, "timer_start") == 0) {
        return time_timer_start(args, arg_count);
    } else if (strcmp(method, "الوقت_المستغرق") == 0 || strcmp(method, "timer_elapsed") == 0) {
        return time_timer_elapsed(args, arg_count);
    } else if (strcmp(method, "أوقف_المؤقت") == 0 || strcmp(method, "timer_stop") == 0) {
        return time_timer_stop(args, arg_count);
    }
    
    return create_value(TYPE_NULL);
}

// Initialize time module
void time_module_init(void) {
    if (module_count < MAX_MODULES) {
        modules[module_count].name = strdup("زمن");
        modules[module_count].init = time_module_init;
        modules[module_count].call = time_module_call;
        module_count++;
    }
}
