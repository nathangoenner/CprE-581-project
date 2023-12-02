#include "fixed.h"
#ifndef FILTER_UTILS_H
#define FILTER_UTILS_H

// Function declarations for utility functions
void wrap(int M, double *w, double **p);
void wrap_fixed(int M, fixed *w, fixed **p);
void wrap2(int M, int *q);
void delay(int D, double *w);
void delay_fixed(int D, fixed *w);
double dot(int M, double *h, double *w);
fixed dot_fixed(int M, fixed *h, fixed *w);

#endif // FILTER_UTILS_H
