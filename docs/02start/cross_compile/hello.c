#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

// trick to escape lint by editor
typedef unsigned int uint32_t;
volatile register unsigned int __R30;
volatile register unsigned int __R31;

#define PRU0_DRAM 0x00000 // Offset to DRAM
// Skip the first 0x200 byte of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.
volatile unsigned int *pru0_dram = (unsigned int *)(PRU0_DRAM + 0x200);
int main(void) {
  int i;

  uint32_t on[] = {1, 2, 3, 4}; // Number of cycles to stay on
  uint32_t *gpio1 = (uint32_t *)GPIO3;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  pru0_dram[0] = on[0]; // Copy to DRAM0 so the ARM can change it
  while (1) {
    gpio1[GPIO_SETDATAOUT] = USR3; // The the USR3 LED on
    if (on[0] != 0) {
      __delay_cycles(100000000); // Wait 1/2 second
    }
    gpio1[GPIO_CLEARDATAOUT] = USR3;

    __delay_cycles(100000000);
  }
  __halt();
}

// Turns off triggers
#pragma DATA_SECTION(init_pins, ".init_pins")
#pragma RETAIN(init_pins)
const char init_pins[] = "/sys/class/leds/beaglebone:green:usr3/trigger\0none\0"
                         "\0\0";
