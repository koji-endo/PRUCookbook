/*
 *
 *  pwm tester
 *	The on cycle and off cycles are stored in each PRU's Data memory
 *
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h> // for close
#define MAXCH 4

#define PRU_ADDR 0x4B200000 // Start of PRU memory Page 184 am335x TRM
#define PRU_LEN 0x80000     // Length of PRU memory
#define PRU0_DRAM 0x00000   // Offset to DRAM
#define PRU1_DRAM 0x02000
#define PRU_SHAREDMEM 0x10000 // Offset to shared memory

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
  unsigned int *prushare = prusharedMem_32int_ptr2;
  printf("%X\n", x);
  printf("prushare0: %X\n", prushare[0]);
  if(-1<x && x<4096){
    prushare[0] = ~x;
  }else{
    printf("value is not within 0 to 4096\n");
  }
  printf("prushare0: %X\n", prushare[0]);
  printf("prushare1: %X\n", prushare[1]);
}

int main(int argc, char *argv[]) {
  unsigned int *pru; // Points to start of PRU memory.
  if(argc!=2){
    printf("this program requires exactly 1 argument which is 0-1023\n");
    exit(EXIT_FAILURE);
  }
  int fd;
  printf("ARM pulse generator\n");

  fd = open("/dev/mem", O_RDWR | O_SYNC);
  if (fd == -1) {
    printf("ERROR: could not open /dev/mem.\n\n");
    return 1;
  }
  pru = mmap(0, PRU_LEN*2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, PRU_ADDR);
  if (pru == MAP_FAILED) {
    printf("ERROR: could not map memory.\n\n");
    return 1;
  }
  close(fd);
  printf("Using /dev/mem.\n");
  prusharedMem_32int_ptr =
      pru + PRU_SHAREDMEM / 4; // Points to start of shared memory
  prusharedMem_32int_ptr2 =pru+PRU_SHAREDMEM/4+PRU_LEN/4;
  write_one(atoi(argv[1]));
  if (munmap(pru, PRU_LEN*2)) {
    printf("munmap failed\n");
  } else {
    printf("munmap succeeded\n");
  }
}
