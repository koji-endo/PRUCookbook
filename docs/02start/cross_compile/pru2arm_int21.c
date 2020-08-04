#include "prugpio.h"
#include "resource_table_pru0.h"
#include <pru_cfg.h>
#include <pru_intc.h>
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
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  __R31 = 0x00000000;              // Clear any pending PRU-generated events
  CT_INTC.CMR4_bit.CH_MAP_16 = 1;  // Map event 16 to channel 1
  CT_INTC.HMR0_bit.HINT_MAP_1 = 1; // Map channel 1 to host 1
  CT_INTC.SICR = 16;               // Ensure event 16 is cleared
  CT_INTC.EISR = 16;               // Enable event 16
  CT_INTC.HIEISR |= (1 << 0);      // Enable Host interrupt 1
  CT_INTC.GER = 1;

  while (1) {
    __R31 = 35;
    __delay_cycles(200000000);
  }
  __halt();
}
