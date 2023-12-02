#include "fir_filters_fixed.h"

// FIR filter in direct form using fixed-point arithmetic
fixed fir_fixed(int M, fixed *h, fixed *w, fixed x) {
    int i;
    fixed y;

    w[0] = x;

    for (y = 0, i = 0; i <= M; i++)
        y = FIXED_ADD(y, FIXED_MUL(h[i], w[i]));

    for (i = M; i >= 1; i--)
        w[i] = w[i - 1];

    return y;
}

// FIR filter in direct form using dot product and delay functions
fixed fir2_fixed(int M, fixed *h, fixed *w, fixed x) {
    fixed y;

    w[0] = x;

    y = dot_fixed(M, h, w);

    delay_fixed(M, w);

    return y;
}

// FIR filter emulating a DSP chip
fixed fir3_fixed(int M, fixed *h, fixed *w, fixed x) {
    int i;
    fixed y;

    w[0] = x;

    for (y = FIXED_MUL(h[M], w[M]), i = M - 1; i >= 0; i--) {
        w[i + 1] = w[i];
        y = FIXED_ADD(y, FIXED_MUL(h[i], w[i]));
    }

    return y;
}

// FIR filter implemented with circular delay-line buffer
fixed cfir_fixed(int M, fixed *h, fixed *w, fixed **p, fixed x) {
    int i;
    fixed y;

    **p = x;

    for (y = 0, i = 0; i <= M; i++) {
        y = FIXED_ADD(y, FIXED_MUL((*h++), *(*p)++));
        wrap_fixed(M, w, p);
    }

    (*p)--; 
    wrap_fixed(M, w, p);

    return y;
}

// FIR filter implemented with circular delay-line buffer
fixed cfir1_fixed(int M, fixed *h, fixed *w, fixed **p, fixed x) {
    int i;
    fixed y;

    *(*p)-- = x;
    wrap_fixed(M, w, p); 

    for (y = 0, h += M, i = M; i >= 0; i--) {
        y = FIXED_ADD(y, FIXED_MUL((*h--), *(*p)--));
        wrap_fixed(M, w, p);
    }

    return y;
}

// FIR filter implemented with circular delay-line buffer
fixed cfir2_fixed(int M, fixed *h, fixed *w, int *q, fixed x) {
    int i;
    fixed y;

    w[*q] = x;

    for (y = 0, i = 0; i <= M; i++) {
        y = FIXED_ADD(y, FIXED_MUL((*h++), w[(*q)++]));
        wrap2(M, q);
    }

    (*q)--; 
    wrap2(M, q);

    return y;
}
