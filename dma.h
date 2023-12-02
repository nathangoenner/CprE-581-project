
#include "xaxidma.h"
#include "xparameters.h"
#include "xdebug.h"
#include "sleep.h"

#if defined(XPAR_UARTNS550_0_BASEADDR)
#include "xuartns550_l.h"       /* to use uartns550 */
#endif
// int dma_start(uintptr_t source, uintptr_t dest, uint32_t length);
int dma_manual(uintptr_t source, uintptr_t dest, uint32_t length);
int dma_test(void);