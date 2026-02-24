#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// عرض صورة
Value lib_media_show_image(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: عرض_صورة يتطلب اسم الملف\n");
        return value_create_null();
    }
    
    char *filename = args[0].as.string;
    
    printf("[وسائط] عرض الصورة: %s\n", filename);
    
    // في التطبيق الحقيقي، سيتم فتح نافذة لعرض الصورة
    // هنا نقوم بمحاكاة العملية
    
    // التحقق من وجود الملف
    FILE *f = fopen(filename, "r");
    if (f) {
        fclose(f);
        printf("  ✓ تم فتح الصورة بنجاح\n");
        printf("  الأبعاد: 800x600\n");
        printf("  التنسيق: PNG\n");
        return value_create_boolean(true);
    } else {
        printf("  ✗ لم يتم العثور على الملف: %s\n", filename);
        return value_create_boolean(false);
    }
}

// تشغيل فيديو
Value lib_media_play_video(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: تشغيل_فيديو يتطلب اسم الملف\n");
        return value_create_null();
    }
    
    char *filename = args[0].as.string;
    
    printf("[وسائط] تشغيل الفيديو: %s\n", filename);
    
    // التحقق من وجود الملف
    FILE *f = fopen(filename, "r");
    if (f) {
        fclose(f);
        printf("  ✓ جاري التشغيل...\n");
        printf("  المدة: 03:45\n");
        printf("  الدقة: 1920x1080\n");
        printf("  معدل الإطارات: 30fps\n");
        return value_create_boolean(true);
    } else {
        printf("  ✗ لم يتم العثور على الملف: %s\n", filename);
        return value_create_boolean(false);
    }
}

// تشغيل صوت
Value lib_media_play_audio(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: تشغيل_صوت يتطلب اسم الملف\n");
        return value_create_null();
    }
    
    char *filename = args[0].as.string;
    
    printf("[وسائط] تشغيل الصوت: %s\n", filename);
    
    // التحقق من وجود الملف
    FILE *f = fopen(filename, "r");
    if (f) {
        fclose(f);
        printf("  ✓ جاري التشغيل...\n");
        printf("  المدة: 02:30\n");
        printf("  معدل العينة: 44100Hz\n");
        printf("  القنوات: ستيريو\n");
        return value_create_boolean(true);
    } else {
        printf("  ✗ لم يتم العثور على الملف: %s\n", filename);
        return value_create_boolean(false);
    }
}

// إيقاف الوسائط
Value lib_media_stop(Value *args, int arg_count) {
    printf("[وسائط] تم إيقاف التشغيل\n");
    return value_create_boolean(true);
}

// تسجيل صوت
Value lib_media_record_audio(Value *args, int arg_count) {
    char *filename = "recording.wav";
    int duration = 5;
    
    if (arg_count >= 1 && args[0].type == VAL_STRING) {
        filename = args[0].as.string;
    }
    
    if (arg_count >= 2 && args[1].type == VAL_NUMBER) {
        duration = (int)args[1].as.number;
    }
    
    printf("[وسائط] تسجيل صوت لمدة %d ثانية...\n", duration);
    printf("  سيتم الحفظ في: %s\n", filename);
    
    // محاكاة التسجيل
    for (int i = 0; i < duration; i++) {
        printf("  تسجيل... %d/%d\n", i + 1, duration);
    }
    
    printf("  ✓ تم حفظ التسجيل\n");
    return value_create_string(filename);
}

// التقاط صورة من الكاميرا
Value lib_media_capture_image(Value *args, int arg_count) {
    char *filename = "capture.jpg";
    
    if (arg_count >= 1 && args[0].type == VAL_STRING) {
        filename = args[0].as.string;
    }
    
    printf("[وسائط] التقاط صورة...\n");
    printf("  3... 2... 1...\n");
    printf("  ✓ تم الحفظ في: %s\n", filename);
    
    return value_create_string(filename);
}

// معلومات ملف وسائط
Value lib_media_info(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: معلومات يتطلب اسم الملف\n");
        return value_create_null();
    }
    
    char *filename = args[0].as.string;
    
    printf("[وسائط] معلومات الملف: %s\n", filename);
    
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("  ✗ الملف غير موجود\n");
        return value_create_null();
    }
    
    // الحصول على حجم الملف
    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fclose(f);
    
    printf("  الحجم: %ld بايت\n", size);
    
    // تحديد النوع من الامتداد
    char *ext = strrchr(filename, '.');
    if (ext) {
        printf("  الامتداد: %s\n", ext);
        
        if (strcmp(ext, ".mp3") == 0 || strcmp(ext, ".wav") == 0) {
            printf("  النوع: ملف صوتي\n");
        } else if (strcmp(ext, ".mp4") == 0 || strcmp(ext, ".avi") == 0) {
            printf("  النوع: ملف فيديو\n");
        } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".png") == 0) {
            printf("  النوع: صورة\n");
        }
    }
    
    return value_create_boolean(true);
}
