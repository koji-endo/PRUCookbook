// pru1_0
#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include <stdint.h>
#define PRU_SRAM __far __attribute__((cregister("PRU_SHAREDMEM", near)))
PRU_SRAM volatile uint32_t shared_1;

volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define PRU0_DRAM 0x00000 // Offset to DRAM
// Skip the first 0x200 byte of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.
void main(void) {
  shared_1 = 0xbeefbeef;
  while (1) {
    if (shared_1 == 0xbeefbeef) {
      __R30 = __R30 | 131128;
    } 
    if (shared_1 != 0xbeefbeef) {
      __R30 = __R30 & ~131128;
    }
  }
  __halt();
}
