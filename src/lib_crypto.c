#include "wisam.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <openssl/evp.h>

// MD5 hash
Value lib_crypto_md5(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    unsigned char result[MD5_DIGEST_LENGTH];
    MD5((unsigned char*)args[0].as.string, strlen(args[0].as.string), result);
    
    char hex[33];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(hex + i * 2, "%02x", result[i]);
    }
    hex[32] = '\0';
    
    return value_create_string(hex);
}

// SHA1 hash
Value lib_crypto_sha1(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    unsigned char result[SHA_DIGEST_LENGTH];
    SHA1((unsigned char*)args[0].as.string, strlen(args[0].as.string), result);
    
    char hex[41];
    for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
        sprintf(hex + i * 2, "%02x", result[i]);
    }
    hex[40] = '\0';
    
    return value_create_string(hex);
}

// SHA256 hash
Value lib_crypto_sha256(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    unsigned char result[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)args[0].as.string, strlen(args[0].as.string), result);
    
    char hex[65];
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        sprintf(hex + i * 2, "%02x", result[i]);
    }
    hex[64] = '\0';
    
    return value_create_string(hex);
}

// Base64 encode
Value lib_crypto_base64_encode(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    const char *input = args[0].as.string;
    int input_len = strlen(input);
    
    EVP_ENCODE_CTX ctx;
    EVP_EncodeInit(&ctx);
    
    int output_len = ((input_len + 2) / 3) * 4 + 1;
    char *output = malloc(output_len);
    
    int len;
    EVP_EncodeUpdate(&ctx, (unsigned char*)output, &len, (unsigned char*)input, input_len);
    int final_len;
    EVP_EncodeFinal(&ctx, (unsigned char*)output + len, &final_len);
    
    output[len + final_len] = '\0';
    
    // Remove newline
    char *newline = strchr(output, '\n');
    if (newline) *newline = '\0';
    
    Value result = value_create_string(output);
    free(output);
    return result;
}

// Base64 decode
Value lib_crypto_base64_decode(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_STRING) {
        return value_create_null();
    }
    
    const char *input = args[0].as.string;
    int input_len = strlen(input);
    
    EVP_ENCODE_CTX ctx;
    EVP_DecodeInit(&ctx);
    
    char *output = malloc(input_len);
    int len;
    EVP_DecodeUpdate(&ctx, (unsigned char*)output, &len, (unsigned char*)input, input_len);
    int final_len;
    EVP_DecodeFinal(&ctx, (unsigned char*)output + len, &final_len);
    
    output[len + final_len] = '\0';
    
    Value result = value_create_string(output);
    free(output);
    return result;
}

// Simple XOR cipher
Value lib_crypto_xor(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    char *text = args[0].as.string;
    char *key = args[1].as.string;
    int text_len = strlen(text);
    int key_len = strlen(key);
    
    char *result = malloc(text_len + 1);
    for (int i = 0; i < text_len; i++) {
        result[i] = text[i] ^ key[i % key_len];
    }
    result[text_len] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// Caesar cipher
Value lib_crypto_caesar(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    
    char *text = args[0].as.string;
    int shift = (int)args[1].as.number;
    int len = strlen(text);
    
    char *result = malloc(len + 1);
    for (int i = 0; i < len; i++) {
        char c = text[i];
        if (c >= 'a' && c <= 'z') {
            result[i] = 'a' + (c - 'a' + shift + 26) % 26;
        } else if (c >= 'A' && c <= 'Z') {
            result[i] = 'A' + (c - 'A' + shift + 26) % 26;
        } else {
            result[i] = c;
        }
    }
    result[len] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// Generate random string
Value lib_crypto_random_string(Value *args, int arg_count) {
    int length = 16;
    if (arg_count >= 1 && args[0].type == VAL_NUMBER) {
        length = (int)args[0].as.number;
    }
    
    const char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    char *result = malloc(length + 1);
    
    for (int i = 0; i < length; i++) {
        result[i] = chars[rand() % (sizeof(chars) - 1)];
    }
    result[length] = '\0';
    
    Value val = value_create_string(result);
    free(result);
    return val;
}

// Generate UUID
Value lib_crypto_uuid(Value *args, int arg_count) {
    char uuid[37];
    const char *format = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";
    
    for (int i = 0; i < 36; i++) {
        if (format[i] == '-') {
            uuid[i] = '-';
        } else if (format[i] == '4') {
            uuid[i] = '4';
        } else if (format[i] == 'y') {
            uuid[i] = "89ab"[rand() % 4];
        } else {
            uuid[i] = "0123456789abcdef"[rand() % 16];
        }
    }
    uuid[36] = '\0';
    
    return value_create_string(uuid);
}

// HMAC (simplified)
Value lib_crypto_hmac(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_STRING || args[1].type != VAL_STRING) {
        return value_create_null();
    }
    
    // Simplified HMAC using SHA256
    char *key = args[0].as.string;
    char *message = args[1].as.string;
    
    // In real implementation, use proper HMAC
    char combined[1024];
    snprintf(combined, sizeof(combined), "%s%s", key, message);
    
    Value combined_val = value_create_string(combined);
    Value hash_args[1] = {combined_val};
    return lib_crypto_sha256(hash_args, 1);
}
