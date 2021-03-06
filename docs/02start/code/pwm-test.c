/*
 *
 *  pwm tester
 *	The on cycle and off cycles are stored in each PRU's Data memory
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/mman.h>

#define MAXCH 4

#define PRU_ADDR 0x4B280000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000     // Length of PRU memory
#define PRU0_DRAM 0x00000   // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000 // Offset to shared memory

unsigned int *pru0DRAM_32int_ptr; // Points to the start of local DRAM
unsigned int *pru1DRAM_32int_ptr; // Points to the start of local DRAM
unsigned int
    *prusharedMem_32int_ptr; // Points to the start of the shared memory
unsigned int
    *prusharedMem_32int_ptr2; // Points to the start of the shared memory

/*******************************************************************************
 * int start_pwm_count(int ch, int countOn, int countOff)
 *
 * Starts a pwm pulse on for countOn and off for countOff to a single channel
 *(ch)
 *******************************************************************************/
/* int start_pwm_count(int ch, int countOn, int countOff) { */
/* 	unsigned int *pruDRAM_32int_ptr = pru0DRAM_32int_ptr; */
/* 	 */
/* 	printf("countOn: %d, countOff: %d, count: %d\n",  */
/* 		countOn, countOff, countOn+countOff); */
/* 	// write to PRU shared memory */
/* 	pruDRAM_32int_ptr[2*(ch)+0] = countOn;	// On time */
/* 	pruDRAM_32int_ptr[2*(ch)+1] = countOff;	// Off time */
/* 	return 0; */
/* } */
int write_one(int x) {
  unsigned int *prushare = prusharedMem_32int_ptr;
  printf("%X\n", x);
  printf("prushare0: %X\n", prushare[0]);
  printf("prushare1: %X\n", prushare[1]);
  printf("prushare2: %X\n", prushare[2]);
  printf("prushare3: %X\n", prushare[3]);
  printf("prushare4: %X\n", prushare[4]);
  printf("prushare5: %X\n", prushare[5]);
  printf("prushare6: %X\n", prushare[6]);
  printf("prushare7: %X\n", prushare[7]);
  if(x){
  prushare[0] = 0xbeefbeef;
  prushare[1] = 0xbeefbeef;
  prushare[2] = 0xbeefbeef;
  prushare[3] = 0xbeefbeef;
  prushare[4] = 0xbeefbeef;
  prushare[5] = 0xbeefbeef;
  prushare[6] = 0xbeefbeef;
  prushare[7] = 0xbeefbeef;
  }else{
  prushare[0] = 0xfeedfeed;
  prushare[1] = 0xfeedfeed;
  prushare[2] = 0xfeedfeed;
  prushare[3] = 0xfeedfeed;
  prushare[4] = 0xfeedfeed;
  prushare[5] = 0xfeedfeed;
  prushare[6] = 0xfeedfeed;
  prushare[7] = 0xfeedfeed;
  }
  printf("prushare0: %X\n", prushare[0]);
  printf("prushare1: %X\n", prushare[1]);
  printf("prushare2: %X\n", prushare[2]);
  printf("prushare3: %X\n", prushare[3]);
  printf("prushare4: %X\n", prushare[4]);
  printf("prushare5: %X\n", prushare[5]);
  printf("prushare6: %X\n", prushare[6]);
  printf("prushare7: %X\n", prushare[7]);
}

int main(int argc, char *argv[]) {
  unsigned int *pru; // Points to start of PRU memory.
  int fd;
  printf("Servo tester\n");

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd == -1) {
    printf("ERROR: could not open /dev/mem.\n\n");
    return 1;
  }
  pru = mmap(0, PRU_LEN, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
  if (pru == MAP_FAILED) {
    printf("ERROR: could not map memory.\n\n");
    return 1;
  }
  close(fd);
  printf("Using /dev/mem.\n");

  pru0DRAM_32int_ptr =
      pru + PRU0_DRAM / 4 + 0x200 / 4; // Points to 0x200 of PRU0 memory
  pru1DRAM_32int_ptr =
      pru + PRU1_DRAM / 4 + 0x200 / 4; // Points to 0x200 of PRU1 memory
  prusharedMem_32int_ptr =
      pru + PRU_SHAREDMEM / 4; // Points to start of shared memory

  prusharedMem_32int_ptr2 =pru+PRU_SHAREDMEM/4+PRU_LEN/4;
  int i;
  unsigned int *prupru = pru0DRAM_32int_ptr;
  prupru[0] = 0;
  /* for(i=0; i<MAXCH; i++) { */
  /* 	start_pwm_count(i, i+1, 20-(i+1)); */
  /* } */
  write_one(atoi(argv[1]));
  if (munmap(pru, PRU_LEN)) {
    printf("munmap failed\n");
  } else {
    printf("munmap succeeded\n");
  }
  if (munmap(pru, PRU_LEN)) {
    printf("munmap failed\n");
  } else {
    printf("munmap succeeded\n");
  }
}
