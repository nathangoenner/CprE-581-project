#include <stdint.h>

// Define the fixed-point data type (e.g., 32-bit integer)
// Note that the Xylinx FIR compiler has a 48-bit accumulator
typedef int32_t fixed;

// Define the number of fractional bits
// On the Xylinx 
#define FRACTIONAL_BITS 16  

// FIR Compiler output has 15 fractional bits compared to 16 input
#define FIR_OUTPUT_FRACTIONAL_BITS 15

// Macro to convert from float to fixed-point
#define FLOAT_TO_FIXED(x, frac_bits) ((fixed)((x) * (1 << frac_bits)))

// Macro to convert from fixed-point to float
#define FIXED_TO_FLOAT(x, frac_bits) ((float)(x) / (1 << frac_bits))

// Macro for fixed-point multiplication
#define FIXED_MUL(a, b, frac_bits) (((fixed)(a) * (b)) >> frac_bits)

// Macro for fixed-point addition
#define FIXED_ADD(a, b) ((fixed)(a) + (b))

// Macro for fixed-point subtraction
#define FIXED_SUB(a, b) ((fixed)(a) - (b))

// ignore small differences 
#define FIXED_POINT_EPSILON 1 // or this?  might need to tweak. (1 << (FIXED_POINT_FRACTIONAL_BITS - 10))
