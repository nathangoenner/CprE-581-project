#pragma once

#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])
#define PI (3.1415926535897)
#define SAMPLE_RATE (10000000) // 10 MHz

void debug_out(const char* format, ...);

void printSamplesCSV(double* input, double* output, uint32_t length);

void genInputSamples(double* input, uint32_t length, double frequency, double amplitude, double fs);

void clearArray_Double(double* input, uint32_t length);

void clearArray_Fixed(fixed* input, uint32_t length);

/* Convert from fixed point of FPGA to float to print - FPGA output/input fractional bits are different so scale the output */
void fixedToFloat(fixed* input, double* output, uint32_t length);

void floatToFixed(double* input, fixed* output, uint32_t length);

double time_diff_us(XTime start, XTime end);
    
void test_time(u32 delay);