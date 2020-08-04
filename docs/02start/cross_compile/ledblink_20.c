//#include "prugpio.h"
#include "resource_table_pru0.h"
#include <pru_cfg.h>
#include <pru_intc.h>
#include <stdint.h>

// trick to escape lint by editor
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

  // p8.44 20_0
  // p8.41 20.1
  // p8.21/42 20.2

  while (1) {
    __R30 = __R30 | 7; // pr2_pru0_gpo15 p8_17
    __delay_cycles(100000000);
    __R30 = __R30 & ~7; // pr2_pru0_gpo15 p8_17
    __delay_cycles(300000000);
  }
  __halt();
}
