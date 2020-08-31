#include "resource_table_0.h"
#include <pru_cfg.h>
#include <pru_intc.h>
#include <pru_rpmsg.h>
#include <rsc_types.h>
#include <stdint.h>
#include <stdio.h>

volatile register uint32_t __R30;
volatile register uint32_t __R31;
volatile unsigned int *shared = (unsigned int *)(0x10000);
/* Host-0 Interrupt sets bit 30 in register R31 */
#define HOST_INT ((uint32_t)1 << 30)

/* The PRU-ICSS system events used for RPMsg are defined in the Linux device
 * tree PRU0 uses system event 16 (To ARM) and 17 (From ARM) PRU1 uses system
 * event 18 (To ARM) and 19 (From ARM)
 */
#define TO_ARM_HOST 16
#define FROM_ARM_HOST 17

/*
 * Using the name 'rpmsg-client-sample' will probe the RPMsg sample driver
 * found at linux-x.y.z/samples/rpmsg/rpmsg_client_sample.c
 *
 * Using the name 'rpmsg-pru' will probe the rpmsg_pru driver found
 * at linux-x.y.z/drivers/rpmsg/rpmsg_pru.c
 */
//#define CHAN_NAME			"rpmsg-client-sample"
#define CHAN_NAME "rpmsg-pru"

#define CHAN_DESC "Channel 30"
#define CHAN_PORT 30
#define SCAN_SIZE 0
#define SCAN_SPEED 1
#define SCAN_SHAPE 2
#define SCAN_INVERT 3
#define SCAN_WIDTH_GO 4
#define SCAN_WIDTH_COME 5
#define SCAN_HEIGHT_GO 6
#define SCAN_HEIGHT_COME 7
#define SCAN_REPEAT_NUM 8
#define KILO 1000
/*
 * Used to make sure the Linux drivers are ready for RPMsg communication
 * Found at linux-x.y.z/include/uapi/linux/virtio_config.h
 */
#define VIRTIO_CONFIG_S_DRIVER_OK 4

uint8_t payload[RPMSG_BUF_SIZE];
int16_t *u16p;
int32_t *u32p;

void OnePulseBase(int count, int dir, int offset) {
  uint32_t one_flat_or = ((0 << u16p[offset + 0]) | (dir << u16p[offset + 1]) |
                          (0 << u16p[offset + 2]) | (dir << u16p[offset + 3]));
  uint32_t one_pulse_or = (1 << u16p[offset + 0]) | one_flat_or;
  if (u32p[SCAN_INVERT]) {
    one_flat_or = (~one_flat_or) & (15 << offset);
    one_pulse_or = (~one_pulse_or) & (15 << offset);
  }
  // const uint32_t one_flat_and = (~(15 << offset)) | one_flat_or;
  int c_i = 0;
  for (c_i = 0; c_i < count; c_i++) {
    uint32_t R30_keep = __R30 & (~(15 << offset));
    __R30 = R30_keep | one_pulse_or;
    __delay_cycles(400); // 2us;
    R30_keep = __R30 & (~(15 << offset));
    __R30 = R30_keep | one_flat_or;
    int w_i = 0;
    // u32p[SCAN_SPEED] millisec each
    for (w_i = 0; w_i < u32p[SCAN_SPEED] * KILO - 2; w_i++) {
      __delay_cycles(200); // 1us;
    }
  }
}

void OnePulse(int count, int dir) { OnePulseBase(count, dir, 0); }

void OneLineStep(int count, int dir) { OnePulseBase(count, dir, 4); }

void xyscan() {
  int r_i = 0;
  int j = 0;
  int i = 0;
  while (1) {
    for (i = 0; i < u32p[SCAN_HEIGHT_GO]; i++) {
      OnePulse(u32p[SCAN_WIDTH_GO], 1);
      OnePulse(u32p[SCAN_WIDTH_COME], 0);
      OneLineStep(1, 1);
    }
    OneLineStep(u32p[SCAN_HEIGHT_COME], 0);
    if (u32p[SCAN_REPEAT_NUM] > 0) {
      r_i++;
      if (r_i == u32p[SCAN_REPEAT_NUM]) {
        break;
      }
    }
  }
}

/*
 * main.c
 */
void main(void) {
  struct pru_rpmsg_transport transport;
  uint16_t src, dst, len;
  volatile uint8_t *status;

  /* Allow OCP master port access by the PRU so the PRU can read external
   * memories */
  CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

  /* Clear the status of the PRU-ICSS system event that the ARM will use to
   * 'kick' us */
  CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;

  /* Make sure the Linux drivers are ready for RPMsg communication */
  status = &resourceTable.rpmsg_vdev.status;
  while (!(*status & VIRTIO_CONFIG_S_DRIVER_OK))
    ;

  /* Initialize the RPMsg transport structure */
  pru_rpmsg_init(&transport, &resourceTable.rpmsg_vring0,
                 &resourceTable.rpmsg_vring1, TO_ARM_HOST, FROM_ARM_HOST);

  /* Create the RPMsg channel between the PRU and ARM user space using the
   * transport structure. */
  while (pru_rpmsg_channel(RPMSG_NS_CREATE, &transport, CHAN_NAME, CHAN_DESC,
                           CHAN_PORT) != PRU_RPMSG_SUCCESS)
    ;
  while (1) {
    /* Check bit 30 of register R31 to see if the ARM has kicked us */
    if (__R31 & HOST_INT) {
      /* Clear the event status */
      CT_INTC.SICR_bit.STS_CLR_IDX = FROM_ARM_HOST;
      /* Receive all available messages, multiple messages can be sent per kick
       */
      while (pru_rpmsg_receive(&transport, &src, &dst, payload, &len) ==
             PRU_RPMSG_SUCCESS) {
        /* Echo the message back to the same address from which we just received
         */
        pru_rpmsg_send(&transport, dst, src, payload, len);
        u16p = (int16_t *)payload;
        u32p = (int32_t *)(payload + 24);
        __R30 = 254;
        shared[0] = 0xbeefbeef;
        shared[1] = __R30;
        shared[2] = 0xbeefbeef;
        shared[3] = u32p[0];
        shared[4] = u32p[1];
        shared[5] = u32p[2];
        shared[6] = u32p[3];
        shared[7] = u32p[4];
        shared[8] = u32p[5];
        shared[9] = u32p[6];
        shared[10] = u32p[7];
        shared[11] = u32p[8];
        xyscan();
      }
    }
  }
}
