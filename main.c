/******************************************************************************
* Copyright (C) 2023 Advanced Micro Devices, Inc. All Rights Reserved.
* SPDX-License-Identifier: MIT
******************************************************************************/
/*
 * helloworld.c: simple test application
 *
 * This application configures UART 16550 to baud rate 9600.
 * PS7 UART (Zynq) is not initialized by this application, since
 * bootrom/bsp configures it to baud rate 115200
 *
 * ------------------------------------------------
 * | UART TYPE   BAUD RATE                        |
 * ------------------------------------------------
 *   uartns550   9600
 *   uartlite    Configurable only in HW design
 *   ps7_uart    115200 (configured by bootrom/bsp)
 */

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

#define USE_FPGA 0
#define FILTER_DATA_SIZE (1024)
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
        output[i] = FIXED_TO_FLOAT(input[i] << 1);
    }
}

void floatToFixed(double* input, fixed* output, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = FLOAT_TO_FIXED(input[i]);
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

/* low pass cutoff w=0.2 (1 MHz) */
static double h[FILTER_LENGTH] = {
  0.0006829175509092024,
  0.00243748358928575,
  0.003142207770501069,
  -0.002422524866965798,
  -0.016726894887862633,
  -0.02972871164928671,
  -0.017411385814133325,
  0.04162378298129599,
  0.1408647243662474,
  0.23773146400425554,
  0.27823817659456984,
  0.23773146400425554,
  0.1408647243662474,
  0.04162378298129599,
  -0.017411385814133325,
  -0.02972871164928671,
  -0.016726894887862633,
  -0.002422524866965798,
  0.003142207770501069,
  0.00243748358928575,
  0.0006829175509092024
};
int main()
{
    init_platform();
    XilTickTimer_Init(&TimerInst);
    // double x[FILTER_DATA_SIZE] = {0.0};
    // double y[FILTER_DATA_SIZE] = {1.0};
    // fixed x_fixed[FILTER_DATA_SIZE] = {0};
    // fixed y_fixed[FILTER_DATA_SIZE] = {0};
    fixed h_fixed[FILTER_LENGTH] = {0};
    fixed w_fixed[FILTER_LENGTH] = {0};

    double* x = malloc(sizeof(double) * FILTER_DATA_SIZE);
    double* y = malloc(sizeof(double) * FILTER_DATA_SIZE);
    fixed* x_fixed = malloc(sizeof(fixed) * FILTER_DATA_SIZE);
    fixed* y_fixed = malloc(sizeof(fixed) * FILTER_DATA_SIZE);
    if (!x || !y || !x_fixed || !y_fixed)
    {
        debug_out("\r\nFailed to allocate memory!!! Pointers: x:%p y%p xf%p yf%p", x, y, x_fixed, y_fixed);
        return 1;
    }
    clearArray_Double(x, FILTER_DATA_SIZE);
    clearArray_Double(y, FILTER_DATA_SIZE);
    clearArray_Fixed(x_fixed, FILTER_DATA_SIZE);
    clearArray_Fixed(y_fixed, FILTER_DATA_SIZE);

    // x[0] = 23.0;
    // x[1] = 25.0;

    /* generate input signal: 500 KHz signal of interest, 3.3 & 5.1 MHz noise of 5 & 10 times size */
    genInputSamples(x, FILTER_DATA_SIZE,  500000., 1.,  1.0/SAMPLE_RATE);
    genInputSamples(x, FILTER_DATA_SIZE, 3300000., 10., 1.0/SAMPLE_RATE);
    genInputSamples(x, FILTER_DATA_SIZE, 5100000., 5.,  1.0/SAMPLE_RATE);



    // test_time(1);
    // test_time(100);
    // test_time(1000);
    // test_time(1000000);
    // test_time(10000000);

    XTime startTime;
    XTime endTime;
    
    floatToFixed(x, x_fixed, FILTER_DATA_SIZE);
    floatToFixed(y, y_fixed, FILTER_DATA_SIZE);
    floatToFixed(h, h_fixed, ARRAY_LEN(h));

    // printSamplesCSV(x, y, FILTER_DATA_SIZE);
    debug_out("\n\r\n\rTEST START:\n\r");
    XTime curTime;
    u32 uTime;
    for (int i = 0; i < 1000; i++)
    {
        XTime_GetTime(&curTime);
        uTime = curTime;
        // debug_out("\r\n%lld - %lu - %u", curTime, uTime, uTime);
        debug_out("\r\n%0.2f", time_diff_us(0, curTime));
        usleep(50);
    }
    XTime_GetTime(&startTime);
    
#define NUM_LOOPS 1
    for (int j = 0; j < NUM_LOOPS; j++) {
#if 1 //USE_FPGA
    for (int i = 0; i < FILTER_DATA_SIZE; i++)
    {
        y_fixed[i] = fir_fixed(FILTER_LENGTH, &h_fixed[0], &w_fixed[0], x_fixed[i]);
    }

#else
    int ret = dma_transfer((u8*)x_fixed, (u8*)y_fixed, FILTER_DATA_SIZE * sizeof(fixed)); // length in bytes, not array size
#endif
    }
    XTime_GetTime(&endTime);

    fixedToFloat(x_fixed, x, FILTER_DATA_SIZE);
    fixedToFloat(y_fixed, y, FILTER_DATA_SIZE);

    printSamplesCSV(x, y, FILTER_DATA_SIZE);

    
    debug_out("\n\rStart Time: %llu, End Time: %llu, Total Time: %0.1f ||| Configuration - len=%d\n\r", startTime, endTime, time_diff_us(startTime, endTime), FILTER_LENGTH);

    cleanup_platform();

    free(x);
    free(y);
    free(x_fixed);
    free(y_fixed);
    return 0;
}
