#include <stdio.h> 
#include <stdint.h>  // Libary for different interger sizes
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "float24.h"

int main()
{
    Float24_t new_float = {0, 0};
    Float24_t float_array[400];
    size_t array_size = 0;
    bool exit_loop = false;
    do { 
        new_float = float24_read();
        float_array[array_size] = new_float;
        array_size++;
        exit_loop = (new_float.exponent == -128 && new_float.mantissa == 0) ? false : true;
        if (array_size >= 100) {
            exit_loop = false;
            array_size++;
        }
    } while(exit_loop == true);

    array_size -= 1;

    Float24_t keep_float;
    Float24_t* max_float = float24_arrayMax(float_array, array_size, &float24_max);

    for(uint8_t index = 0; index < array_size; index++) {
        //  Conditional statement for compactness.
        keep_float = index == 0 ? float_array[0] : float24_add(float_array[index], keep_float);
        printf("Array[%d]: %f\n", index, float24_asIEEE(float_array[index]));
    }

    float accumulative_sum = float24_asIEEE(keep_float);
    float squared_sum = float24_asIEEE(float24_multiply(keep_float, keep_float));

    printf("Accumulated Sum: %f\n", accumulative_sum);
    printf("Square Accumulated Sum: %f\n", squared_sum);
    printf("Max of Numbers: %f\n", float24_asIEEE(*max_float));

    return 0;
}
