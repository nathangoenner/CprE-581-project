
#include <stdint.h>
#include <stdio.h>
#include "platform.h"
#include "xil_printf.h"
#include <xil_cache.h>
#include <xparameters.h>
#include "sleep.h"
// #include "asm/cacheflush.h"

#include "xaxidma.h"

// #define TEST_START_VALUE 0xC
// #define MAX_PKT_LEN      0x20

volatile uint32_t* dma_MM2S_Control = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR);
volatile uint32_t* dma_MM2S_Status = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x4);
volatile uint32_t* dma_MM2S_Source = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x18);
volatile uint32_t* dma_MM2S_Length = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x28);

volatile uint32_t* dma_S2MM_Control = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x30);
volatile uint32_t* dma_S2MM_Status = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x34);
volatile uint32_t* dma_S2MM_Dest = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x48);
volatile uint32_t* dma_S2MM_Length = (volatile uint32_t*)((uintptr_t)XPAR_XAXIDMA_0_BASEADDR+0x58);

#define GET_BIT(variable, bit) ((*(uint32_t*)variable & (1 << bit)) != 0)

void print_control_regs(void)
{
    char str[128];
    sprintf(str, "MM2S Control: %x, S2MM Control: %x", *dma_MM2S_Control, *dma_S2MM_Control);
    xil_printf("\r\n%s", str);
}

int dma_manual(uintptr_t source, uintptr_t dest, uint32_t length)
{
    char str[128];
    sprintf(str, "\r\nSource: %p, Dest: %p, Length: %u", source, dest, length);
    xil_printf("%s", str);

    /* Reset DMA */
    *dma_S2MM_Control |= 0x04;
    *dma_MM2S_Control |= 0x04;
    sprintf(str, "\r\nSource: %p, Dest: %p, Length: %u", source, dest, length);
    xil_printf("%s", str);
    print_control_regs();

    /* Stop DMA */
    *dma_S2MM_Control &= ~(0x01);
    *dma_MM2S_Control &= ~(0x01);
    sprintf(str, "\r\nSource: %p, Dest: %p, Length: %u", source, dest, length);
    xil_printf("%s", str);

    
    *dma_MM2S_Source = source;
    *dma_S2MM_Dest = dest;


    Xil_DCacheFlushRange(source, length);
    xil_printf("\r\nStarting DMA transfer...");
    print_control_regs();
    *dma_MM2S_Control |= 0x01;
    *dma_S2MM_Control |= 0x01;
    *dma_MM2S_Length = length;
    *dma_S2MM_Length = length;

    int retries = 0;
    while (!GET_BIT(dma_MM2S_Control, 12) ||
            !GET_BIT(dma_MM2S_Control, 1) ||
            !GET_BIT(dma_S2MM_Control, 12) ||
            !GET_BIT(dma_S2MM_Control, 1))
    {
        print_control_regs();
        if (++retries > 10)
        {
            break;
        }
        sleep(1);
    }


    // *(uint32_t*)dma_S2MM_Control &= ~((1 << 12) | (1 << 13) | (1 << 14));
    // *(uint32_t*)dma_MM2S_Control &= ~((1 << 12) | (1 << 13) | (1 << 14));

    // Xil_DCacheFlushRange(dest, length);
    Xil_DCacheInvalidateRange(dest, length);
    return 0;
}

// XAxiDma AxiDma;

// /* DMA from xaxidma.h driver example in polling mode */
// int dma_start(uintptr_t source, uintptr_t dest, uint32_t length)
// {
//     XAxiDma_Config *CfgPtr;
//     int Status;
//     int Tries = 10;
//     int Index;
//     u8 *TxBufferPtr;
//     u8 *RxBufferPtr;
//     u8 Value;
//     int TimeOut = 1000000U;

//     TxBufferPtr = (u8 *)source;
//     RxBufferPtr = (u8 *)dest;

//     /* Initialize the XAxiDma device.
//         */

//     CfgPtr = XAxiDma_LookupConfig(XPAR_XAXIDMA_0_BASEADDR);
//     if (!CfgPtr) {
//         xil_printf("No config found for %d\r\n", XPAR_XAXIDMA_0_BASEADDR);
//         return XST_FAILURE;
//     }

//     Status = XAxiDma_CfgInitialize(&AxiDma, CfgPtr);
//     if (Status != XST_SUCCESS) {
//         xil_printf("Initialization failed %d\r\n", Status);
//         return XST_FAILURE;
//     }

//     if (XAxiDma_HasSg(&AxiDma)) {
//         xil_printf("Device configured as SG mode \r\n");
//         return XST_FAILURE;
//     }

//     /* Disable interrupts, we use polling mode
// 	 */
// 	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
// 			    XAXIDMA_DEVICE_TO_DMA);
// 	XAxiDma_IntrDisable(&AxiDma, XAXIDMA_IRQ_ALL_MASK,
// 			    XAXIDMA_DMA_TO_DEVICE);

// 	/* Flush the buffers before the DMA transfer, in case the Data Cache
// 	 * is enabled
// 	 */
// 	Xil_DCacheFlushRange((UINTPTR)TxBufferPtr, length);
// 	Xil_DCacheFlushRange((UINTPTR)RxBufferPtr, length);

// 	for (Index = 0; Index < Tries; Index ++) {


// 		Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR) RxBufferPtr,
// 						length, XAXIDMA_DEVICE_TO_DMA);

// 		if (Status != XST_SUCCESS) {
// 			return XST_FAILURE;
// 		}

// 		Status = XAxiDma_SimpleTransfer(&AxiDma, (UINTPTR) TxBufferPtr,
// 						length, XAXIDMA_DMA_TO_DEVICE);

// 		if (Status != XST_SUCCESS) {
// 			return XST_FAILURE;
// 		}

// 		/*Wait till tranfer is done or 1usec * 10^6 iterations of timeout occurs*/
// 		while (TimeOut) {
// 			if (!(XAxiDma_Busy(&AxiDma, XAXIDMA_DEVICE_TO_DMA)) &&
// 			    !(XAxiDma_Busy(&AxiDma, XAXIDMA_DMA_TO_DEVICE))) {
// 				break;
// 			}
// 			TimeOut--;
// 			usleep(1U);
// 		}
// 	}

// }
