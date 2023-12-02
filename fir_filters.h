#include "filter_utils.h"

#ifndef FIR_FILTERS_H
#define FIR_FILTERS_H

// Function declarations for FIR filters
double fir(int M, double *h, double *w, double x);
double fir2(int M, double *h, double *w, double x);
double fir3(int M, double *h, double *w, double x);
double cfir(int M, double *h, double *w, double **p, double x);
double cfir1(int M, double *h, double *w, double **p, double x);
double cfir2(int M, double *h, double *w, int *q, double x);

#endif // FIR_FILTERS_H
