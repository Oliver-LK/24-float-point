#include <stdio.h> 
#include <stdint.h>  // Libary for different interger sizes
#include <limits.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include "float24.h"


//  Initialises a the mantissa and exponent to the 24bit number
Float24_t float24_init(int16_t mantissa, int8_t exponent)
{
    Float24_t New_number;
    New_number.mantissa = mantissa;
    New_number.exponent = exponent;
    return New_number;
}

//  Prints the newly initialised mantissa & exponent
void float24_print(Float24_t value)
{
    printf("%hd * 2 ^ %hhd\n", value.mantissa, value.exponent);
}

//  Reduces the oversizeMantissa to a number that can be represented using an int16_t
static Float24_t float24_normalise(int32_t oversizeMantissa, int8_t exponent)
{
    Float24_t normalized_num;
    int32_t normalized_mantissa = oversizeMantissa;
    int16_t internal_exponent = 0;

    if(oversizeMantissa > INT16_MAX || oversizeMantissa < -1 * INT16_MAX) {
        normalized_mantissa = oversizeMantissa;
        
        // Divides the mantissa down until its within the size range of int16_t
        while((normalized_mantissa) > INT16_MAX || normalized_mantissa < -1 * INT16_MAX) {
            internal_exponent += 1;
            normalized_mantissa = oversizeMantissa / pow(2, internal_exponent);
        }
    }

    exponent += internal_exponent;
    normalized_num = float24_init(normalized_mantissa, exponent);
    return normalized_num; 
}

//  Multiplies 2 Float24_t's together
Float24_t float24_multiply(Float24_t num1, Float24_t num2)
{
    Float24_t result;

    int32_t mantissaResult = num1.mantissa * num2.mantissa;
    int8_t exponentResult = num1.exponent + num2.exponent;

    result = float24_normalise(mantissaResult, exponentResult);

    return result;
}


//  Adds 2 Float24_t's together
Float24_t float24_add(Float24_t num1, Float24_t num2)
{
    Float24_t result;

    int8_t large_exponent = MAX(num1.exponent, num2.exponent);
    int32_t mantissaResult1 = num1.mantissa * (pow(2, num1.exponent) / pow(2, large_exponent));
    int32_t mantissaResult2 = num2.mantissa * (pow(2, num2.exponent) / pow(2, large_exponent));
    int32_t mantissaResult = mantissaResult1 + mantissaResult2;
    result = float24_normalise(mantissaResult, large_exponent);
    
    return result;
}

//  This function reads the users input and updates the values located at each address
void reader(char* read, int8_t* b_index, int8_t* read_index, bool* rubbish)  
{
    while(read[*read_index - 1] != EOF && read[*read_index - 1] != ' ' && read[*read_index - 1] != '\n') {
        read[*read_index] = getchar();
        if(read[*read_index] == 'b') {
            *b_index = *read_index;
        } else if(read[*read_index] > 57 && read[*read_index] != 'b') {
            *rubbish = true;
        }
        *read_index += 1;
    }
}

//  Converts user input into a Float24_t
Float24_t float24_read(void)
{
    int32_t mantissa = 0, exponent = 0;
    int8_t read_index = 0;
    int8_t b_index = 0;
    char read[BUFFER_SIZE];
    bool rubbish = false;

    //  Calls the reader function that reads the user input. Values are updated by passing address
    reader(read, &b_index, &read_index, &rubbish);

    char mantissa_array[BUFFER_SIZE];
    char exponent_array[BUFFER_SIZE];
    int32_t mantissa_size = 0;
    int32_t exponent_size = 0;
    int16_t index_hold;

    //  This block splits the mantissa and the exponent into separate arrays and records there size
    for(int16_t sorting_index = 0; sorting_index < read_index - 1; sorting_index ++) {
        if(sorting_index < b_index) {
            mantissa_array[sorting_index] = read[sorting_index];
            mantissa_size ++;
        } else if (sorting_index > b_index) {
            exponent_array[sorting_index - b_index -1] = read[sorting_index];
            exponent_size ++;
        }
    }

    //  Checks to see if the mantissa has a negative sign. If so then size is accounted for
    index_hold = IS_NEG(mantissa_array[0]);

    //  Converts the string into a int
    for(int16_t mantissa_index = index_hold; mantissa_size > mantissa_index; mantissa_index++ ) {
        mantissa += (mantissa_array[mantissa_index] - CHAR_TO_NUM) * pow(10, b_index - mantissa_index - 1);
    }

    //  If negative sign then will make mantissa negative
    mantissa = IS_NEG(mantissa_array[0]) == true ? mantissa * -1 : mantissa;
    
    //  The proccess is then repeated for the exponent
    index_hold = IS_NEG(exponent_array[0]);
    

    for(int16_t exponent_index = index_hold; exponent_size > exponent_index; exponent_index++ ) {
        exponent += (exponent_array[exponent_index] - CHAR_TO_NUM) * pow(10, exponent_size - exponent_index - 1);
    }
    
    exponent = IS_NEG(exponent_array[0]) == true ? exponent * -1 : exponent;

    exponent = (exponent > 127) ? 127 : exponent;

    //  Checks to see if the input given values were rubbish or not
    return rubbish == true ? float24_normalise(0, -128) : float24_normalise(mantissa, exponent);
}


void float24_max(Float24_t* num1, Float24_t* num2, Float24_t** max)
{
    Float24_t negative_num2 = {-1*(num2->mantissa), num2->exponent};
    Float24_t new_num = float24_add(*num1, negative_num2);

    if(new_num.mantissa >= 0 && new_num.mantissa != 0) {
        *max = num1;
    } else if (new_num.mantissa < 0 && new_num.mantissa != 0) {
        *max = num2;
    } else if (num1->exponent >= num2->exponent) {
        *max = num1;
    } else if (num1->exponent < num2->exponent) {
        *max = num2;
    }
    
}

Float24_t* float24_arrayMax(Float24_t* array, size_t size, void (*func)(Float24_t*, Float24_t*, Float24_t**))
{
    Float24_t* largest_num = &array[0];

    // Ensures code does not loop if the size of the array is 1
    if(size > 1) {
        for(int16_t index = 1; index < size; index ++) {
            float24_max(largest_num, &array[index], &largest_num);
        }
    }
    return largest_num;
}

