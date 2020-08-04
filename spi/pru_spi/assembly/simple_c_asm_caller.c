// Shows how to call an assembly routine with one parameter
#include "prugpio.h"
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

// The function is defined in delay.asm in same dir
// We just need to add a declaration here, the defination can be
// seperately linked
extern void my_delay_cycles(uint32_t);

volatile register uint32_t R30;
volatile register uint32_t R31;

void main(void) {
  uint32_t gpio = P9_31; // Select which pin to toggle.;

  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  while (1) {
    R30 |= gpio; // Set the GPIO pin to 1
    my_delay_cycles(1);
    R30 &= ~gpio; // Clear the GPIO pin
    my_delay_cycles(1);
  }
}
