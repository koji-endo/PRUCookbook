// Shows how to call an assembly routine with a return value
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

#define TEST 100

// The function is defined in delay.asm in same dir
// We just need to add a declaration here, the defination can be
// seperately linked
extern uint32_t my_delay_cycles(uint32_t);

uint32_t ret;
volatile unsigned int *shared = (unsigned int *)(0x10000);
#define out 1 // Bit number to output on

volatile register uint32_t __R30;
volatile register uint32_t __R31;

int main(void) {
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  ret = my_delay_cycles(10);
  shared[0] = 0xfeedbeef;
  shared[1] = 0xabbe1234;
  shared[2] = ret;
  shared[3] = 0xabbe4321;
}
