
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
  /* unsigned int latch=0x0; */
  /* int sign=1; */
  while (1) {
    unsigned int num = 1;
    /* if(num==-1||num==4096){ */
    /*   sign=-sign; */
    /* } */
    /* num+=sign; */
    for (num = 1; num < 4096; num++) {
      __R30 = (~num) & (~(1 << 12));
      /* __R30 = shared[0]|(1<11); */
      __delay_cycles(50); // 0.25 microsec
      __R30 = __R30 | (1 << 12);
      /* __R30 = __R30 | (1<<11); */
      shared[1] = __R30;
      __delay_cycles(100); // 0.5 microsec
    }
  }
  __halt();
}
