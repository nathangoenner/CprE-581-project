#include "fixed.h"
#include "filter_utils.h"
#ifndef FIR_FILTERS_FIXED_H
#define FIR_FILTERS_FIXED_H

// Function declarations for FIR filters
fixed fir_fixed(int M, fixed *h, fixed *w, fixed x);
fixed fir2_fixed(int M, fixed *h, fixed *w, fixed x);
fixed fir3_fixed(int M, fixed *h, fixed *w, fixed x);
fixed cfir_fixed(int M, fixed *h, fixed *w, fixed **p, fixed x);
fixed cfir1_fixed(int M, fixed *h, fixed *w, fixed **p, fixed x);
fixed cfir2_fixed(int M, fixed *h, fixed *w, int *q, fixed x);

#endif // FIR_FILTERS_FIXED_H
