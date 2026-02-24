#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// تنفيذ أمر
Value lib_system_execute(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    int result = system(args[0].as.string);
    return value_create_number(WEXITSTATUS(result));
}

// تنفيذ أمر والحصول على الإخراج
Value lib_system_capture(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    FILE *fp = popen(args[0].as.string, "r");
    if (!fp) return value_create_null();
    
    char buffer[4096];
    char *result = malloc(1);
    result[0] = '\0';
    size_t total_len = 0;
    
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        size_t len = strlen(buffer);
        result = realloc(result, total_len + len + 1);
        strcpy(result + total_len, buffer);
        total_len += len;
    }
    
    pclose(fp);
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// الحصول على متغير بيئة
Value lib_system_getenv(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *value = getenv(args[0].as.string);
    if (value) {
        return value_create_string(value);
    }
    return value_create_null();
}

// تعيين متغير بيئة
Value lib_system_setenv(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    return value_create_boolean(setenv(args[0].as.string, args[1].as.string, 1) == 0);
}

// معرف العملية
Value lib_system_getpid(Value *args, int arg_count) {
    return value_create_number(getpid());
}

// معرف العملية الأب
Value lib_system_getppid(Value *args, int arg_count) {
    return value_create_number(getppid());
}

// معرف المستخدم
Value lib_system_getuid(Value *args, int arg_count) {
    return value_create_number(getuid());
}

// معرف المجموعة
Value lib_system_getgid(Value *args, int arg_count) {
    return value_create_number(getgid());
}

// اسم المستخدم
Value lib_system_getusername(Value *args, int arg_count) {
    char *user = getenv("USER");
    if (!user) user = getenv("USERNAME");
    if (user) return value_create_string(user);
    return value_create_null();
}

// المجلد الرئيسي
Value lib_system_gethome(Value *args, int arg_count) {
    char *home = getenv("HOME");
    if (home) return value_create_string(home);
    return value_create_null();
}

// مسار التنفيذ
Value lib_system_getpath(Value *args, int arg_count) {
    char *path = getenv("PATH");
    if (path) return value_create_string(path);
    return value_create_null();
}

// إيقاف مؤقت
Value lib_system_sleep(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    sleep((unsigned int)args[0].as.number);
    return value_create_null();
}

// إيقاف مؤقت (مللي ثانية)
Value lib_system_msleep(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    usleep((useconds_t)(args[0].as.number * 1000));
    return value_create_null();
}

// الخروج
Value lib_system_exit(Value *args, int arg_count) {
    int code = 0;
    if (arg_count >= 1 && args[0].type == VAL_NUMBER) {
        code = (int)args[0].as.number;
    }
    exit(code);
    return value_create_null();
}

// إرسال إشارة
Value lib_system_kill(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_NUMBER || args[1].type != VAL_NUMBER) {
        return value_create_boolean(false);
    }
    return value_create_boolean(kill((pid_t)args[0].as.number, (int)args[1].as.number) == 0);
}

// إنشاء عملية فرعية
Value lib_system_fork(Value *args, int arg_count) {
    pid_t pid = fork();
    return value_create_number(pid);
}

// انتظار عملية
Value lib_system_wait(Value *args, int arg_count) {
    int status;
    pid_t pid = wait(&status);
    return value_create_number(pid);
}

// تنفيذ برنامج
Value lib_system_exec(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_boolean(false);
    }
    
    char **argv = malloc((arg_count + 1) * sizeof(char*));
    for (int i = 0; i < arg_count; i++) {
        if (args[i].type == VAL_STRING) {
            argv[i] = args[i].as.string;
        }
    }
    argv[arg_count] = NULL;
    
    execvp(argv[0], argv);
    free(argv);
    return value_create_boolean(false);
}

// معلومات النظام
Value lib_system_info(Value *args, int arg_count) {
    Value result = value_create_object();
    
    result.as.object.keys[0] = strdup("نظام_التشغيل");
    result.as.object.values[0] = malloc(sizeof(Value));
    #ifdef __linux__
    *result.as.object.values[0] = value_create_string("Linux");
    #elif __APPLE__
    *result.as.object.values[0] = value_create_string("macOS");
    #else
    *result.as.object.values[0] = value_create_string("Unknown");
    #endif
    
    result.as.object.keys[1] = strdup("المعمارية");
    result.as.object.values[1] = malloc(sizeof(Value));
    #if __x86_64__
    *result.as.object.values[1] = value_create_string("x86_64");
    #elif __i386__
    *result.as.object.values[1] = value_create_string("x86");
    #elif __arm__
    *result.as.object.values[1] = value_create_string("ARM");
    #else
    *result.as.object.values[1] = value_create_string("Unknown");
    #endif
    
    result.as.object.count = 2;
    return result;
}
