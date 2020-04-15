// pru1_0
#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define PRU0_DRAM 0x00000 // Offset to DRAM
// Skip the first 0x200 byte of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.
//(1<<1)+(1<<2)+(1<<3)+(1<<5)+(1<<6)+(1<<7)+(1<<9)+(1<<10)+(1<<11)+(1<<14)+(1<<15)+(1<<16)+(1<<17)+(1<<18)
void main(void) {
  while (1) {

    __R30 = __R30 | (1 << 12); //
    __delay_cycles(50);        // Wait 1/2 second
    __R30 = __R30 & ~(1 << 12);
    __delay_cycles(199950);
  }
  __halt();
}
