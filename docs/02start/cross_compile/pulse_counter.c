#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <stdint.h>

// trick to escape lint by editor
volatile register unsigned int __R30;
volatile register unsigned int __R31;

//#define PRU0_DRAM 0x00000 // Offset to DRAM
// Skip the first 0x200 byte of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.
// volatile unsigned int *pru0_dram = (unsigned int *)(PRU0_DRAM + 0x200);
volatile unsigned int *shared = (unsigned int *)(0x10000);
int main(void) {
  int i = 0;
  unsigned int cycle;
  unsigned int keep[10];
  unsigned int keep_ele;
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  PRU0_CTRL.CTRL_bit.CTR_EN = 1;
  shared[0] = 0x1234deed;
  PRU0_CTRL.CYCLE = cycle;
  //__delay_cycles(0);
  // keep_ele = __R31;
  keep[1] = (__R31 & 1);
  keep[1] += (__R31 & 1) << 1;
  // keep[1] += (__R31 & 1) << 2;

  /* shared[1] = __R31; */
  /* shared[2] = __R31; */
  /* shared[3] = __R31; */
  // keep[0] = __R31;
  cycle = PRU0_CTRL.CYCLE;
  // shared[1] = keep[0];
  shared[1] = keep[1];
  shared[2] = keep[2];
  shared[3] = keep[3];
  shared[4] = cycle;
  shared[5] = 0x123488ef;
  __halt();
}
