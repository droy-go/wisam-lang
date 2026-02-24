#include "wisam.h"
#include <math.h>
#include <stdlib.h>

// دوال الرياضيات الأساسية

Value lib_math_abs(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(fabs(args[0].as.number));
}

Value lib_math_sqrt(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(sqrt(args[0].as.number));
}

Value lib_math_pow(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_NUMBER || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(pow(args[0].as.number, args[1].as.number));
}

Value lib_math_sin(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(sin(args[0].as.number));
}

Value lib_math_cos(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(cos(args[0].as.number));
}

Value lib_math_tan(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(tan(args[0].as.number));
}

Value lib_math_log(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(log(args[0].as.number));
}

Value lib_math_log10(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(log10(args[0].as.number));
}

Value lib_math_exp(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(exp(args[0].as.number));
}

Value lib_math_floor(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(floor(args[0].as.number));
}

Value lib_math_ceil(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(ceil(args[0].as.number));
}

Value lib_math_round(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(round(args[0].as.number));
}

Value lib_math_max(Value *args, int arg_count) {
    if (arg_count < 2) return value_create_null();
    double max = args[0].as.number;
    for (int i = 1; i < arg_count; i++) {
        if (args[i].type == VAL_NUMBER && args[i].as.number > max) {
            max = args[i].as.number;
        }
    }
    return value_create_number(max);
}

Value lib_math_min(Value *args, int arg_count) {
    if (arg_count < 2) return value_create_null();
    double min = args[0].as.number;
    for (int i = 1; i < arg_count; i++) {
        if (args[i].type == VAL_NUMBER && args[i].as.number < min) {
            min = args[i].as.number;
        }
    }
    return value_create_number(min);
}

Value lib_math_random(Value *args, int arg_count) {
    return value_create_number((double)rand() / RAND_MAX);
}

Value lib_math_random_int(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_NUMBER || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    int min = (int)args[0].as.number;
    int max = (int)args[1].as.number;
    return value_create_number(min + rand() % (max - min + 1));
}

Value lib_math_pi(Value *args, int arg_count) {
    return value_create_number(M_PI);
}

Value lib_math_e(Value *args, int arg_count) {
    return value_create_number(M_E);
}

Value lib_math_degrees(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(args[0].as.number * 180.0 / M_PI);
}

Value lib_math_radians(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    return value_create_number(args[0].as.number * M_PI / 180.0);
}

Value lib_math_factorial(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    int n = (int)args[0].as.number;
    if (n < 0) return value_create_null();
    double result = 1;
    for (int i = 2; i <= n; i++) result *= i;
    return value_create_number(result);
}

Value lib_math_gcd(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_NUMBER || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    int a = (int)args[0].as.number;
    int b = (int)args[1].as.number;
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return value_create_number(abs(a));
}

Value lib_math_lcm(Value *args, int arg_count) {
    if (arg_count < 2 || args[0].type != VAL_NUMBER || args[1].type != VAL_NUMBER) {
        return value_create_null();
    }
    int a = (int)args[0].as.number;
    int b = (int)args[1].as.number;
    Value gcd_result = lib_math_gcd(args, 2);
    return value_create_number(abs(a * b) / (int)gcd_result.as.number);
}

Value lib_math_is_prime(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_boolean(false);
    }
    int n = (int)args[0].as.number;
    if (n < 2) return value_create_boolean(false);
    for (int i = 2; i * i <= n; i++) {
        if (n % i == 0) return value_create_boolean(false);
    }
    return value_create_boolean(true);
}

Value lib_math_fibonacci(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_NUMBER) {
        return value_create_null();
    }
    int n = (int)args[0].as.number;
    if (n < 0) return value_create_null();
    if (n == 0) return value_create_number(0);
    if (n == 1) return value_create_number(1);
    double a = 0, b = 1;
    for (int i = 2; i <= n; i++) {
        double temp = a + b;
        a = b;
        b = temp;
    }
    return value_create_number(b);
}

Value lib_math_sum(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    double sum = 0;
    for (int i = 0; i < args[0].as.array.count; i++) {
        if (args[0].as.array.items[i]->type == VAL_NUMBER) {
            sum += args[0].as.array.items[i]->as.number;
        }
    }
    return value_create_number(sum);
}

Value lib_math_average(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    Value sum = lib_math_sum(args, 1);
    return value_create_number(sum.as.number / args[0].as.array.count);
}

Value lib_math_median(Value *args, int arg_count) {
    if (arg_count < 1 || args[0].type != VAL_ARRAY) {
        return value_create_null();
    }
    int n = args[0].as.array.count;
    if (n == 0) return value_create_null();
    
    // Create array of numbers
    double *arr = malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        arr[i] = args[0].as.array.items[i]->as.number;
    }
    
    // Simple bubble sort
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (arr[j] > arr[j + 1]) {
                double temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
    
    double result;
    if (n % 2 == 0) {
        result = (arr[n/2 - 1] + arr[n/2]) / 2;
    } else {
        result = arr[n/2];
    }
    
    free(arr);
    return value_create_number(result);
}
