// pru2_0
#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <stdint.h>


volatile register unsigned int __R30;
volatile register unsigned int __R31;
volatile unsigned int *shared=(unsigned int *) (0x10008);
void main(void) {
  shared[0]= 0xbeefbeef;
  while (1) {
    if (shared[0] == 0xbeefbeef) {
      __R30 = __R30 | 1;
    } 
    if (shared[0] != 0xbeefbeef) {
      __R30 = __R30 & ~1;
    }
  }
  __halt();
}
