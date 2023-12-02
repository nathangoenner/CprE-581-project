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
// #include "fir_filters_fixed.h"
#include "xiltimer.h"
#include <math.h>
#include <xparameters.h>
#include "dma.h"
#include "sleep.h"


#define FILTER_DATA_SIZE (512)
#define FILTER_LENGTH (21)
#define ARRAY_LEN(x) sizeof(x)/sizeof(x[0])
#define PI (3.1415926535897)
#define SAMPLE_RATE (10000000) // 10 MHz

void printSamplesCSV(double* input, double* output, uint32_t length)
{
    xil_printf("\r\nSample,In,Out");
    for (uint32_t i = 0; i < length; i++)
    {
        char str[128];
        snprintf(str, ARRAY_LEN(str), "\r\n%lu,%.4f,%.4f", i, input[i], output[i]);
        xil_printf("%s", str);
    }
}

void genInputSamples(double* input, uint32_t length, double frequency, double amplitude, double fs)
{
    for (uint32_t i = 0; i < length; i++)
    {
        input[i] += sin(fs * i * 2 * PI * frequency) * amplitude;
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

void fixedToFloat(fixed* input, double* output, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = FIXED_TO_FLOAT(input[i]);
    }
}

void floatToFixed(double* input, fixed* output, uint32_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        output[i] = FLOAT_TO_FIXED(input[i]);
    }
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
    double x[FILTER_DATA_SIZE] = {0.0};
    double y[FILTER_DATA_SIZE] = {1.0};
    fixed x_fixed[FILTER_DATA_SIZE] = {0};
    fixed y_fixed[FILTER_DATA_SIZE] = {0};
    // double h_fpga[FILTER_LENGTH] = {6,0,-4,-3,5,6,-6,-13,7,44,64,44,7,-13,-6,6,5,-3,-4,0,6};
    double w[FILTER_LENGTH] = {0.0};
    init_platform();

    uint32_t* dma_control = XPAR_XAXIDMA_0_BASEADDR;

    // *dma_control |= 0x04;
    // while ((*dma_control & 0x04) != 0)
    // {
    //     xil_printf("\r\nDMA_WAIT");
    //     sleep(1);
    // }

    dma_test();


    /* generate input signal: 500 KHz signal of interest, 3.3 & 5.1 MHz noise of 5 & 10 times size */
    genInputSamples(&x[0], ARRAY_LEN(x),  500000., 1.,  1.0/SAMPLE_RATE);
    genInputSamples(&x[0], ARRAY_LEN(x), 3300000., 10., 1.0/SAMPLE_RATE);
    genInputSamples(&x[0], ARRAY_LEN(x), 5100000., 5.,  1.0/SAMPLE_RATE);

    XTime startTime;
    XTime endTime;
    xil_printf("\n\r\n\rTEST START:\n\r");
    XTime_GetTime(&startTime);
    // for (int j = 0; j < 10; j++) {
    // for (int i = 0; i < FILTER_DATA_SIZE; i++)
    // {
    //     y[i] = fir(FILTER_LENGTH, &h[0], &w[0], x[i]);
    // }
    // }
    floatToFixed(x, x_fixed, ARRAY_LEN(x));
    floatToFixed(y, y_fixed, ARRAY_LEN(y));
    // int ret = dma_start(&x_fixed[0], &y_fixed[0], ARRAY_LEN(x_fixed));
    // int ret = dma_manual(&(x_fixed[0]), &y_fixed, 16);
    int ret = dma_manual((uintptr_t) &x[0], (uintptr_t) &y[0], 16);
    xil_printf("\r\nReturn: %d", ret);
    // fixedToFloat(y_fixed, y, ARRAY_LEN(y_fixed));
    XTime_GetTime(&endTime);

    printSamplesCSV(&x[0], &y[0], ARRAY_LEN(y));

    
    char str[128];
    snprintf(str, ARRAY_LEN(str), "\n\rStart Time: %llu, End Time: %llu, Total Time: %llu Y: %.4f\n\r", startTime, endTime, endTime-startTime, y[FILTER_DATA_SIZE-1]);
    xil_printf("%s", str);
    // dma_start(&x_fixed, &y_fixed, ARRAY_LEN(x_fixed));
    cleanup_platform();
    return 0;
}
