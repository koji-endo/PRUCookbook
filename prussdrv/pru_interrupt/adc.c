/*
 * adc.c
 */
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "pruss_intc_mapping.h"
#include "prussdrv.h"

#define PRU_NUM 0

#define OFFSET_MEM0 0x00000000
#define OFFSET_MEM1 0x00002000
#define OFFSET_SHAREDRAM 0x00010000
#define SIZE_OF_BUFFER 0x2000

/******************************************************************************
 * Global Variable Definitions                                                 *
 ******************************************************************************/

static void *pruDataMem;
static unsigned int *pruDataMem_int0; /*AM33XX_DATA 8KB RAM0*/
static unsigned int *pruDataMem_int1; /*AM33XX_DATA 8KB RAM1*/
static unsigned int *sharedMem_int;
static unsigned int *currentBuffer_int;

/******************************************************************************
 * Global Function Definitions                                                 *
 ******************************************************************************/

int main(int argc, char *argv[]) {
  unsigned int ret;
  struct sched_param sp;
  int policy;
  int result;

  int i = 0;
  FILE *pfile;
  tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;
  unsigned short sampling_data[2048 * 16];

  if (argc < 2) /*2nd argc is file name for output sampling data.*/
  {
    printf("There is no output file name.\n");
    return (0);
  }
  pfile = fopen(argv[1], "w");
  if (pfile == NULL) {
    printf("Unable to open output file.\n");
    return (0);
  }

  /* change schduling policy to real-time */
  policy =
      SCHED_FIFO; /* we can select either SCHED_RR or SCHED_FIFO as policy. */
  sp.sched_priority = 1; /* we can select priority value within 1 to 99. */
  result = sched_setscheduler(0, policy, &sp);
  if (result) {
    printf("Unable to change schduling policy: sched_setscheduler = %d\n",
           result);
    return (0);
  }

  /* Initialize the PRU */
  prussdrv_init();
  printf("PRU_EVTOUT:%d\n", PRU_EVTOUT_0);

  printf("try to open driver\n");
  /* Open PRU Interrupt */
  ret = prussdrv_open(PRU_EVTOUT_0);
  if (ret) {
    printf("prussdrv_open open failed\n");
    return (ret);
  }
  printf("driver was opened\n");
  /* Get the interrupt initialized */
  prussdrv_pruintc_init(&pruss_intc_initdata);

  /* Execute example on PRU */
  printf("\tINFO: Executing example.\r\n");
  int pru_exec_result = prussdrv_exec_program(PRU_NUM, "./pru2arm_int21.out");
  printf("pru_exec_result is :%d\n", pru_exec_result);
  prussdrv_map_prumem(PRUSS0_PRU0_DATARAM, &pruDataMem);
  /*assign data RAM addresses to two pointers*/
  pruDataMem_int0 =
      (unsigned int *)(pruDataMem + OFFSET_MEM0); /*AM33XX_DATA 8KB RAM1,Glocal
                                                     Memory Address*/
  pruDataMem_int1 =
      (unsigned int *)(pruDataMem + OFFSET_MEM1); /*AM33XX_DATA 8KB RAM0,Glocal
                                                     Memory Address*/
  sharedMem_int = (unsigned int *)(pruDataMem + OFFSET_SHAREDRAM);

  /* endless loop*/
  while (1) {
    int j;
    unsigned int sample;
    unsigned int startstep = 0x10;

    /* Wait PRU0 interrupt */
    printf("before wait\n");
    prussdrv_pru_wait_event(PRU_EVTOUT_0);
    printf("after wait\n");
    /* clear event */
    prussdrv_pru_clear_event(PRU_EVTOUT_0, PRU0_ARM_INTERRUPT);
    printf("sharedMem_int[0]=%8x , sharedMem_int[1]=%8x , ", sharedMem_int[0],
           sharedMem_int[1]);

    currentBuffer_int = (sharedMem_int[1]) ? pruDataMem_int0 : pruDataMem_int1;
    printf("%8x , %8x\n", currentBuffer_int[0], currentBuffer_int[1]);
    if (startstep == 0x10)
      startstep = (currentBuffer_int[0] & 0xf000) >> 12;
    for (j = 0; j < SIZE_OF_BUFFER / sizeof(unsigned int); j++) {
      sample = currentBuffer_int[j];
      if (((sample & 0xf000) >> 12) == startstep) {
        if (i == 2048)
          break;
        fprintf(pfile, "\n%d", (sample >> 16) >> 1);
        i++;
      }
      fprintf(pfile, "\t%d", sample & 0xfff);
    }
    if (i == 2048) {
      break;
    }
  }
  fprintf(pfile, "\n");
  fclose(pfile); /*close the output file*/
  printf("sharedMem_int[3]=%8x\n", sharedMem_int[3]);
  sharedMem_int[2] = 0x00000001; /*turn off the ADC*/
  while (sharedMem_int[3] ==
         0x00) /*wait untile Command has been executed by PRU0.*/
  {
    usleep(1000);
  }
  printf("sharedMem_int[3]=%8x\n", sharedMem_int[3]);

  /* Disable PRU and close memory mapping. */
  prussdrv_pru_disable(PRU_NUM);
  prussdrv_exit();
  return (0);
}
