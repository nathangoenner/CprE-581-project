#include "filter_utils.h"


// Circular wrap of pointer p, relative to array w
void wrap(int M, double *w, double **p) {
    if (*p > w + M)
        *p -= M + 1;  // when *p = w + M + 1, it wraps around to *p = w

    if (*p < w)
        *p += M + 1;  // when *p = w - 1, it wraps around to *p = w + M
}


void wrap_fixed(int M, fixed *w, fixed **p) {
    if (*p > w + M)
        *p -= M + 1;  // when *p = w + M + 1, it wraps around to *p = w

    if (*p < w)
        *p += M + 1;  // when *p = w - 1, it wraps around to *p = w + M
}

// Circular wrap of pointer offset q, relative to array w
void wrap2(int M, int *q) {
    if (*q > M)
        *q -= M + 1;  // when *q = M + 1, it wraps around to *q = 0

    if (*q < 0)
        *q += M + 1;  // when *q = -1, it wraps around to *q = M
}

void delay_fixed(int D, fixed *w) {
    int i;

    for (i = D; i >= 1; i--)  // reverse-order updating
        w[i] = w[i - 1];
}

// Delay by D time samples
void delay(int D, double *w) {
    int i;

    for (i = D; i >= 1; i--)  // reverse-order updating
        w[i] = w[i - 1];
}

// Dot product of two length-(M+1) vectors
double dot(int M, double *h, double *w) {
    int i;
    double y;

    for (y = 0, i = 0; i <= M; i++)  // compute dot product
        y += h[i] * w[i];      

    return y;
}

fixed dot_fixed(int M, fixed *h, fixed *w) {
    int i;
    fixed y;

    for (y = 0, i = 0; i <= M; i++)  // compute dot product
        y = FIXED_ADD(y, FIXED_MUL(h[i], w[i]));      

    return y;
}
