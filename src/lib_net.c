#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// هيكل لحفظ استجابة HTTP
typedef struct {
    char *data;
    size_t size;
} HTTPResponse;

// Callback لـ libcurl
static size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    HTTPResponse *resp = (HTTPResponse *)userp;
    
    char *ptr = realloc(resp->data, resp->size + realsize + 1);
    if (!ptr) return 0;
    
    resp->data = ptr;
    memcpy(&(resp->data[resp->size]), contents, realsize);
    resp->size += realsize;
    resp->data[resp->size] = '\0';
    
    return realsize;
}

// طلب HTTP GET
Value lib_net_http_get(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: http_get يتطلب URL\n");
        return value_create_null();
    }
    
    char *url = args[0].as.string;
    
    printf("[شبكة] طلب GET: %s\n", url);
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "خطأ: فشل تهيئة CURL\n");
        return value_create_null();
    }
    
    HTTPResponse resp = {0};
    resp.data = malloc(1);
    resp.data[0] = '\0';
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "خطأ في الطلب: %s\n", curl_easy_strerror(res));
        free(resp.data);
        return value_create_null();
    }
    
    printf("  رمز الاستجابة: %ld\n", http_code);
    printf("  حجم البيانات: %zu بايت\n", resp.size);
    
    // إنشاء كائن الاستجابة
    Value result = value_create_object();
    
    // إضافة الحقول
    result.as.object.keys[0] = strdup("status");
    result.as.object.values[0] = malloc(sizeof(Value));
    *result.as.object.values[0] = value_create_number(http_code);
    
    result.as.object.keys[1] = strdup("body");
    result.as.object.values[1] = malloc(sizeof(Value));
    *result.as.object.values[1] = value_create_string(resp.data);
    
    result.as.object.count = 2;
    
    free(resp.data);
    
    return result;
}

// طلب HTTP POST
Value lib_net_http_post(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: http_post يتطلب URL وبيانات\n");
        return value_create_null();
    }
    
    char *url = args[0].as.string;
    char *data = args[1].as.string;
    
    printf("[شبكة] طلب POST: %s\n", url);
    printf("  البيانات: %s\n", data);
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "خطأ: فشل تهيئة CURL\n");
        return value_create_null();
    }
    
    HTTPResponse resp = {0};
    resp.data = malloc(1);
    resp.data[0] = '\0';
    
    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    CURLcode res = curl_easy_perform(curl);
    
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "خطأ في الطلب: %s\n", curl_easy_strerror(res));
        free(resp.data);
        return value_create_null();
    }
    
    printf("  رمز الاستجابة: %ld\n", http_code);
    
    Value result = value_create_object();
    
    result.as.object.keys[0] = strdup("status");
    result.as.object.values[0] = malloc(sizeof(Value));
    *result.as.object.values[0] = value_create_number(http_code);
    
    result.as.object.keys[1] = strdup("body");
    result.as.object.values[1] = malloc(sizeof(Value));
    *result.as.object.values[1] = value_create_string(resp.data);
    
    result.as.object.count = 2;
    
    free(resp.data);
    
    return result;
}

// إنشاء خادم ويب بسيط
Value lib_net_create_server(Value *args, int arg_count) {
    int port = 8080;
    
    if (arg_count >= 1 && args[0].type == VAL_NUMBER) {
        port = (int)args[0].as.number;
    }
    
    printf("[شبكة] إنشاء خادم ويب على المنفذ %d...\n", port);
    printf("  الخادم يعمل على http://localhost:%d\n", port);
    printf("  (اضغط Ctrl+C للإيقاف)\n");
    
    // في التطبيق الحقيقي، سيتم إنشاء خادم HTTP كامل
    // هنا نقوم بمحاكاة
    
    return value_create_number(port);
}

// تحميل ملف
Value lib_net_download(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        fprintf(stderr, "خطأ: تحميل يتطلب URL واسم الملف\n");
        return value_create_null();
    }
    
    char *url = args[0].as.string;
    char *filename = args[1].as.string;
    
    printf("[شبكة] تحميل: %s -> %s\n", url, filename);
    
    CURL *curl = curl_easy_init();
    if (!curl) {
        return value_create_boolean(false);
    }
    
    FILE *fp = fopen(filename, "wb");
    if (!fp) {
        curl_easy_cleanup(curl);
        return value_create_boolean(false);
    }
    
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    
    CURLcode res = curl_easy_perform(curl);
    
    curl_easy_cleanup(curl);
    fclose(fp);
    
    if (res != CURLE_OK) {
        fprintf(stderr, "خطأ في التحميل: %s\n", curl_easy_strerror(res));
        return value_create_boolean(false);
    }
    
    printf("  ✓ تم التحميل بنجاح\n");
    
    return value_create_boolean(true);
}

// الحصول على IP
Value lib_net_get_ip(Value *args, int arg_count) {
    printf("[شبكة] الحصول على عنوان IP...\n");
    
    // في التطبيق الحقيقي، سيتم الحصول على IP الفعلي
    return value_create_string("127.0.0.1");
}

// فحص منفذ
Value lib_net_check_port(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        fprintf(stderr, "خطأ: فحص_منفذ يتطلب رقم المنفذ\n");
        return value_create_null();
    }
    
    int port = (int)args[0].as.number;
    
    printf("[شبكة] فحص المنفذ %d...\n", port);
    
    // في التطبيق الحقيقي، سيتم فحص المنفذ فعلياً
    return value_create_boolean(true);
}

// ترميز URL
Value lib_net_url_encode(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: ترميز_url يتطلب نصاً\n");
        return value_create_null();
    }
    
    char *text = args[0].as.string;
    char *encoded = curl_easy_escape(NULL, text, strlen(text));
    
    if (!encoded) {
        return value_create_string(text);
    }
    
    Value result = value_create_string(encoded);
    curl_free(encoded);
    
    return result;
}

// فك ترميز URL
Value lib_net_url_decode(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        fprintf(stderr, "خطأ: فك_ترميز_url يتطلب نصاً\n");
        return value_create_null();
    }
    
    char *text = args[0].as.string;
    char *decoded = curl_easy_unescape(NULL, text, strlen(text), NULL);
    
    if (!decoded) {
        return value_create_string(text);
    }
    
    Value result = value_create_string(decoded);
    curl_free(decoded);
    
    return result;
}

// إرسال بريد إلكتروني (محاكاة)
Value lib_net_send_email(Value *args, int arg_count) {
    if (arg_count < 4 || args[0].type != VAL_STRING || args[1].type != VAL_STRING ||
        args[2].type != VAL_STRING || args[3].type != VAL_STRING) {
        fprintf(stderr, "خطأ: إرسال_بريد يتطلب إلى، من، الموضوع، والمحتوى\n");
        return value_create_null();
    }
    
    printf("[شبكة] إرسال بريد إلكتروني...\n");
    printf("  إلى: %s\n", args[0].as.string);
    printf("  من: %s\n", args[1].as.string);
    printf("  الموضوع: %s\n", args[2].as.string);
    printf("  المحتوى: %s...\n", args[3].as.string);
    
    return value_create_boolean(true);
}
