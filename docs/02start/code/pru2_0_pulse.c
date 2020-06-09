// pru2_0
#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <stdint.h>

volatile register unsigned int __R30;
volatile register unsigned int __R31;
volatile unsigned int *shared = (unsigned int *)(0x10000);
void main(void) {
  unsigned int latch = 0x0;
  shared[0] = 0x0;
  while (1) {
    if (latch != shared[0]) {
      __R30 = shared[0] & (~(1 << 12)); // P8_32だと思われる。
      /* __R30 = shared[0]|(1<11); */
      __delay_cycles(100);
      __R30 = __R30 | (1 << 12);
      __delay_cycles(500);
      /* __R30 = __R30 | (1<<11); */
      latch = shared[0];
      shared[1] = __R30;
    }
  }
  __halt();
}
