#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/utsname.h>
#include <sys/resource.h>
#include <unistd.h>

// اسم البرنامج
static char *program_name = "وسام";

// تعيين اسم البرنامج
void lib_meta_set_program_name(const char *name) {
    free(program_name);
    program_name = strdup(name);
}

// الحصول على اسم البرنامج
Value lib_meta_program_name(Value *args, int arg_count) {
    return value_create_string(program_name);
}

// الحصول على اسم نظام التشغيل
Value lib_meta_os(Value *args, int arg_count) {
    struct utsname info;
    
    if (uname(&info) == 0) {
        char os_info[256];
        snprintf(os_info, sizeof(os_info), "%s %s", info.sysname, info.release);
        return value_create_string(os_info);
    }
    
    return value_create_string("غير معروف");
}

// الحصول على استخدام الذاكرة
Value lib_meta_memory_usage(Value *args, int arg_count) {
    struct rusage usage;
    
    if (getrusage(RUSAGE_SELF, &usage) == 0) {
        // تحويل إلى ميغابايت
        double mb = usage.ru_maxrss / 1024.0 / 1024.0;
        return value_create_number(mb);
    }
    
    return value_create_number(0);
}

// الحصول على معرف العملية
Value lib_meta_pid(Value *args, int arg_count) {
    return value_create_number(getpid());
}

// الحصول على معرف العملية الأب
Value lib_meta_ppid(Value *args, int arg_count) {
    return value_create_number(getppid());
}

// الحصول على معلومات النظام
Value lib_meta_system_info(Value *args, int arg_count) {
    Value result = value_create_object();
    
    struct utsname info;
    if (uname(&info) == 0) {
        result.as.object.keys[0] = strdup("نظام");
        result.as.object.values[0] = malloc(sizeof(Value));
        *result.as.object.values[0] = value_create_string(info.sysname);
        
        result.as.object.keys[1] = strdup("إصدار");
        result.as.object.values[1] = malloc(sizeof(Value));
        *result.as.object.values[1] = value_create_string(info.release);
        
        result.as.object.keys[2] = strdup("معمارية");
        result.as.object.values[2] = malloc(sizeof(Value));
        *result.as.object.values[2] = value_create_string(info.machine);
        
        result.as.object.count = 3;
    }
    
    return result;
}

// الحصول على وقت التشغيل
Value lib_meta_uptime(Value *args, int arg_count) {
    // في التطبيق الحقيقي، سيتم حساب وقت التشغيل الفعلي
    return value_create_number(0);
}

// الحصول على إصدار لغة وسام
Value lib_meta_wisam_version(Value *args, int arg_count) {
    return value_create_string(WISAM_VERSION);
}

// الحصول على معلومات المترجم
Value lib_meta_compiler(Value *args, int arg_count) {
    #ifdef __GNUC__
    char info[256];
    snprintf(info, sizeof(info), "GCC %d.%d.%d", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
    return value_create_string(info);
    #else
    return value_create_string("غير معروف");
    #endif
}

// الحصول على عدد المعالجات
Value lib_meta_cpu_count(Value *args, int arg_count) {
    long nprocs = sysconf(_SC_NPROCESSORS_ONLN);
    return value_create_number(nprocs);
}

// الحصول على اسم المستخدم
Value lib_meta_username(Value *args, int arg_count) {
    char *username = getenv("USER");
    if (!username) username = getenv("USERNAME");
    if (!username) username = "غير معروف";
    
    return value_create_string(username);
}

// الحصول على المجلد الحالي
Value lib_meta_cwd(Value *args, int arg_count) {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        return value_create_string(cwd);
    }
    return value_create_string("غير معروف");
}

// الحصول على متغير بيئة
Value lib_meta_getenv(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: getenv يتطلب اسم المتغير\n");
        return value_create_null();
    }
    
    char *value = getenv(args[0].as.string);
    if (value) {
        return value_create_string(value);
    }
    
    return value_create_null();
}

// تعيين متغير بيئة
Value lib_meta_setenv(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: setenv يتطلب اسم المتغير والقيمة\n");
        return value_create_null();
    }
    
    setenv(args[0].as.string, args[1].as.string, 1);
    return value_create_boolean(true);
}
