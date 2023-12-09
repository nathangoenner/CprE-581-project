/* CPRE581 Term Project - FPGA Accelerated FIR Filtering
 * Nathan Goenner
 * Wesley Jones
 * Bryce Hall
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
#include "gpio.h"
#include "utils.h"

int test_fpga(fixed* x, fixed* y);
int test_arm(fixed* x, fixed* y);
int test_arm_float(double* x, double* y);

#define FILTER_DATA_SIZE (2048)
#define FILTER_ORDER  (20)
#define FILTER_LENGTH (FILTER_ORDER + 1)


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

    /* Initialize the GPIO pins that we use for measurement */
    led_init();
    set_led0(0);
    set_led1(0);

    /* Allocate memory for the input/output arrays of arbitrary length */
    double* x = malloc(sizeof(double) * FILTER_DATA_SIZE);
    double* y = malloc(sizeof(double) * FILTER_DATA_SIZE);
    fixed* x_fixed = malloc(sizeof(fixed) * FILTER_DATA_SIZE);
    fixed* y_fixed = malloc(sizeof(fixed) * FILTER_DATA_SIZE);

    /* Check for failure to allocate memory */
    if (!x || !y || !x_fixed || !y_fixed)
    {
        debug_out("\r\nFailed to allocate memory!!! Pointers: x:%p y%p xf%p yf%p", x, y, x_fixed, y_fixed);
        return 1;
    }

    /* malloc doesn't clear memory, so make sure we initialize the arrays */
    clearArray_Double(x, FILTER_DATA_SIZE);
    clearArray_Double(y, FILTER_DATA_SIZE);
    clearArray_Fixed(x_fixed, FILTER_DATA_SIZE);
    clearArray_Fixed(y_fixed, FILTER_DATA_SIZE);

    /* generate input signal: 500 KHz signal of interest, 3.3 & 5.1 MHz noise of 5 & 10 times size */
    genInputSamples(x, FILTER_DATA_SIZE,  500000., 1.,  1.0/SAMPLE_RATE);
    genInputSamples(x, FILTER_DATA_SIZE, 3300000., 10., 1.0/SAMPLE_RATE);
    genInputSamples(x, FILTER_DATA_SIZE, 5100000., 5.,  1.0/SAMPLE_RATE);    

    floatToFixed(x, x_fixed, FILTER_DATA_SIZE);
    // floatToFixed(y, y_fixed, FILTER_DATA_SIZE);

    // printSamplesCSV(x, y, FILTER_DATA_SIZE);
    debug_out("\n\r\n\rTEST START:\n\r");

    test_fpga(&x_fixed[0], &y_fixed[0]);
    test_arm(&x_fixed[0], &y_fixed[0]);

    fixedToFloat(x_fixed, x, FILTER_DATA_SIZE);
    fixedToFloat(y_fixed, y, FILTER_DATA_SIZE);


    printSamplesCSV(x, y, FILTER_DATA_SIZE);
    test_arm_float(x, y);
    debug_out("\r\nTest Finished!");
    

    cleanup_platform();

    free(x);
    free(y);
    free(x_fixed);
    free(y_fixed);
    return 0;
}

int test_fpga(fixed* x, fixed* y)
{
    int ret;
    set_led0(1);
    ret = dma_transfer((u8*)x, (u8*)y, FILTER_DATA_SIZE * sizeof(fixed)); // length in bytes, not array size
    set_led0(0);
    return ret;
}

int test_arm(fixed* x, fixed* y)
{
    fixed h_fixed[FILTER_LENGTH] = {0};
    fixed w_fixed[FILTER_LENGTH] = {0};
    floatToFixed(h, h_fixed, ARRAY_LEN(h));

    set_led1(1);
    for (int i = 0; i < FILTER_DATA_SIZE; i++)
    {
        y[i] = fir3_fixed(FILTER_ORDER, &h_fixed[0], &w_fixed[0], x[i]);
    }
    set_led1(0);
    return 0;
}

int test_arm_float(double* x, double* y)
{
    double w[FILTER_LENGTH] = {0};

    set_led1(1);
    for (int i = 0; i < FILTER_DATA_SIZE; i++)
    {
        y[i] = fir3(FILTER_ORDER, &h[0], &w[0], x[i]);
    }
    set_led1(0);
    return 0;
}
