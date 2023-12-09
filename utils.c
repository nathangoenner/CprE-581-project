
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include "fir_filters.h"
#include "fir_filters_fixed.h"
#include "xiltimer.h"
#include <math.h>
#include <xparameters.h>
#include "dma.h"
#include "sleep.h"
#include <time.h>
#include <stdlib.h>
#include <stdarg.h>
#include "gpio.h"
#include "utils.h"

#define FILTER_LENGTH (21)
#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])
#define PI (3.1415926535897)
#define SAMPLE_RATE (10000000) // 10 MHz

void debug_out(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    char str[256];
    vsnprintf(str, sizeof(str), format, args);
    xil_printf("%s", str);

}

void printSamplesCSV(double* input, double* output, uint32_t length)
{
    xil_printf("\r\nSample,In,Out");
    for (uint32_t i = 0; i < length; i++)
    {
        debug_out("\r\n%lu,%.4f,%.4f", i, input[i], output[i]);
    }
}

void genInputSamples(double* input, uint32_t length, double frequency, double amplitude, double fs)
{
    for (uint32_t i = 0; i < length; i++)
    {
        input[i] += sin(fs * i * 2 * PI * frequency) * amplitude;
        // debug_out("Gen %d: variable=%0.2f", i, input[i]);
    }
}

void clearArray_Double(double* input, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        input[i] = 0.0;
    }
}

void clearArray_Fixed(fixed* input, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        input[i] = 0;
    }
}

/* Convert from fixed point of FPGA to float to print - FPGA output/input fractional bits are different so scale the output */
void fixedToFloat(fixed* input, double* output, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = FIXED_TO_FLOAT(input[i] << 1, FRACTIONAL_BITS);
    }
}

void floatToFixed(double* input, fixed* output, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = FLOAT_TO_FIXED(input[i], FRACTIONAL_BITS);
    }
}

double time_diff_us(XTime start, XTime end)
{
    double time_us = ((double)(end - start) * 1000000) / COUNTS_PER_SECOND;
    return time_us;
}
    
void test_time(u32 delay)
{
    // sleep(0);
    XTime start, end;

    XTime_GetTime(&start);
    usleep(delay);
    XTime_GetTime(&end);

    debug_out("\r\nDelay: %u, Time: %0.4f, Start: %llu, End: %llu", delay, time_diff_us(start, end), start, end);
}