// Shows how to call an assembly routine with a return value
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

#define TEST 100

// The function is defined in delay.asm in same dir
// We just need to add a declaration here, the defination can be
// seperately linked
extern uint32_t parallel(uint32_t);

uint32_t ret = 0;
volatile unsigned int *shared = (unsigned int *)(0x10000);
#define out 1 // Bit number to output on

volatile register uint32_t __R30;
volatile register uint32_t __R31;
#define DAC_8734_DataBase 0x04    // register offset to zero cal base
#define DAC_8734_CalZeroBase 0x08 // register offset to zero cal base
#define DAC_8734_CalGainBase 0x0C // register offset to gain cal base
#define DAC_8734_Command 0
int DAC_CAL_tab[8] = {0x08, 0xfF, 0x0F, 0x4F,
                      0x3f, 0x80, 0x80, 0x80}; // zero's then gain's
/*  */
int main(void) {
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;
  // initdac
  parallel(100000000);
  shared[0] = 0xfacedead;
  shared[1] = __R30;
  shared[2] = ret;
  shared[3] = 0xdeaddead;
  __halt();
}
