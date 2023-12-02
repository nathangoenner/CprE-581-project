#include "fir_filters.h"

// M = filter length - 1
// h = filter coefficients
// w = filter states
// x = current input sample
// y = current output sample

// FIR filter in direct form
double fir(int M, double *h, double *w, double x) {
    int i;
    double y;

    w[0] = x;  // read current input sample x

    for (y = 0, i = 0; i <= M; i++)
        y += h[i] * w[i];  // compute current output sample y

    for (i = M; i >= 1; i--)
        w[i] = w[i - 1];  // update states for next call

    return y;
}

// FIR filter in direct form using dot product and delay functions
double fir2(int M, double *h, double *w, double x) {
    double y;

    w[0] = x;  // read input

    y = dot(M, h, w);  // compute output

    delay(M, w);  // update states

    return y;
}

// FIR filter emulating a DSP chip
double fir3(int M, double *h, double *w, double x) {
    int i;
    double y;

    w[0] = x;  // read input

    for (y = h[M] * w[M], i = M - 1; i >= 0; i--) {
        w[i + 1] = w[i];  // data shift instruction
        y += h[i] * w[i];  // MAC instruction
    }

    return y;
}

// FIR filter implemented with circular delay-line buffer
double cfir(int M, double *h, double *w, double **p, double x) {
    int i;
    double y;

    **p = x;  // read input sample x

    for (y = 0, i = 0; i <= M; i++) {  // compute output sample y
        y += (*h++) * (*(*p)++);
        wrap(M, w, p);
    }

    (*p)--;  // update circular delay line
    wrap(M, w, p);

    return y;
}

// FIR filter implemented with circular delay-line buffer
double cfir1(int M, double *h, double *w, double **p, double x) {
    int i;
    double y;

    *(*p)-- = x;
    wrap(M, w, p);  // p now points to s[M]

    for (y = 0, h += M, i = M; i >= 0; i--) {  // h starts at h[M]
        y += (*h--) * (*(*p)--);
        wrap(M, w, p);
    }

    return y;
}

// FIR filter implemented with circular delay-line buffer
double cfir2(int M, double *h, double *w, int *q, double x) {
    int i;
    double y;

    w[*q] = x;  // read input sample x

    for (y = 0, i = 0; i <= M; i++) {  // compute output sample y
        y += (*h++) * w[(*q)++];
        wrap2(M, q);
    }

    (*q)--;  // update circular delay line
    wrap2(M, q);

    return y;
}
