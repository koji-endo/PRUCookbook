#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

// trick to escape lint by editor
typedef unsigned int uint32_t;
volatile register unsigned int __R30;
volatile register unsigned int __R31;

//#define PRU0_DRAM 0x00000 // Offset to DRAM
// Skip the first 0x200 byte of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.
// volatile unsigned int *pru0_dram = (unsigned int *)(PRU0_DRAM + 0x200);
int main(void) {
  int i = 0;
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  while (1) {
    __R30 = __R30 | (1 << 15); // pr2_pru1_gpo15 P8_10
                               // __delay_cycles(50);        // Wait 1/2 second
    __delay_cycles(100000000);
    __R30 = __R30 & ~(1 << 15); // pr2_pru1_gpo15 P8_10
                                // __delay_cycles(199950);
    __delay_cycles(100000000);
    i++;
    if (i % 3 == 0) {
      __R31 = 34; // 18+16
      i = 0;
    }
  }
  __halt();
}
