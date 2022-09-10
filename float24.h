#ifndef FLOAT24_H
#define FLOAT24_H

#include <stdio.h> 
#include <stdint.h>  // Libary for different interger sizes
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

//  Definitions to simplify code
#define MAX(num1, num2) ((num1 > num2) ? num1 :num2)
#define CHAR_TO_NUM 48
#define IS_NEG(char1) ((char1 == '-') ? 1 : 0)
#define BUFFER_SIZE 20

//  Data type feild
typedef struct {
    int16_t mantissa;
    int8_t exponent;
} Float24_t;

//  Functions

Float24_t float24_init(int16_t mantissa, int8_t exponent);

void float24_print(Float24_t value);

Float24_t float24_multiply(Float24_t num1, Float24_t num2);

Float24_t float24_add(Float24_t num1, Float24_t num2);

void reader(char* read, int8_t* b_index, int8_t* read_index, bool* rubbish);

Float24_t float24_read(void);

float float24_asIEEE(Float24_t x);

void float24_max(Float24_t* num1, Float24_t* num2, Float24_t** max);

Float24_t* float24_arrayMax(Float24_t* array, size_t size, void (*func)(Float24_t*, Float24_t*, Float24_t**));

#endif  //  FLOAT24_H