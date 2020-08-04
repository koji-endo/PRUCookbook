// Shows how to call an assembly routine with a return value
#include "resource_table_empty.h"
#include <pru_cfg.h>
#include <stdint.h>

#define TEST 100

// The function is defined in delay.asm in same dir
// We just need to add a declaration here, the defination can be
// seperately linked
extern uint32_t spi_transfer(uint32_t);

uint32_t ret = 0;
volatile unsigned int *shared = (unsigned int *)(0x10000);
#define out 1 // Bit number to output on

volatile register uint32_t __R30;
volatile register uint32_t __R31;
#define DAC_8734_DataBase 0x04 // register offset to zero cal base
/* register */
#define DAC_8734_CalZeroBase 0x08 // register offset to zero cal base
/* register*/
#define DAC_8734_CalGainBase 0x0C // register offset to gain cal base
/* register */
int DAC_CAL_tab[8] = {0x08, 0xfF, 0x0F, 0x4F,
                      0x3f, 0x80, 0x80, 0x80}; // zero's then gain's
/*  */
int main(void) {
  /* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
  /* CT_CFG.SYSCFG_bit.STANDBY_INIT = 0; */
  /* // initdac */
  /* uint32_t dac_init = 0x0000 | 1 << 9 | 1 << 8; */
  /* //  ret = spi_transfer(dac_init); */
  /* #<{(|  |)}># */
  /* int x = 0; */
  /* for (x = 0; x < 4; x++) { */
  /*   spi_transfer(((DAC_8734_CalZeroBase + x) << 16) + */
  /*                DAC_CAL_tab[x]); // Zero cal */
  /*   spi_transfer(((DAC_8734_CalGainBase + x) << 16) + */
  /*                DAC_CAL_tab[x + 4]); // Gain cal */
  /* } */
  /* #<{(| for (;;) { |)}># */
  /* int ii = 0; */
  /* //  for (ii = 0; ii < 256; ii++) { */
  /* ret = spi_transfer(((DAC_8734_DataBase) << 16) + ii * 256); */
  //  }
  /* } */
  shared[0] = 0xabbebeef;
  shared[1] = __R30;
  shared[2] = ret;
  shared[3] = 0xbeefbeef;
  __halt();
}
