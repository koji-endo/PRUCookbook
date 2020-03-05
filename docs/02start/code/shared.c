
// From: http://git.ti.com/pru-software-support-package/pru-software-support-package/blobs/master/examples/am335x/PRU_access_const_table/PRU_access_const_table.c
#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table_empty.h"

#define PRU_SRAM  __far __attribute__((cregister("PRU_SHAREDMEM", near)))
/* NOTE:  Allocating shared_x to PRU Shared Memory means that other PRU cores on
 *        the same subsystem must take care not to allocate data to that memory.
 *		  Users also cannot rely on where in shared memory these variables are placed
 *        so accessing them from another PRU core or from the ARM is an undefined behavior.
 */
PRU_SRAM  volatile uint32_t shared_1;

/* NOTE:  Here we pick where in memory to store shared_5.  The stack and
 *		  heap take up the first 0x200 words, so we must start after that.
 *		  Since we are hardcoding where things are stored we can share
 *		  this between the PRUs and the ARM.
*/
// Skip the first 0x200 bytes of DRAM since the Makefile allocates
// 0x100 for the STACK and 0x100 for the HEAP.


int main(void)
{
  *((&shared_1)+1)=0xbeedbeed;
  *((&shared_1)+2)=(int)&shared_1;
	__halt();
}
