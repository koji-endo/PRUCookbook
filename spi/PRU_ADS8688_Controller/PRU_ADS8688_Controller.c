/*
 * Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
 *
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *	* Redistributions of source code must retain the above copyright
 *	  notice, this list of conditions and the following disclaimer.
 *
 *	* Redistributions in binary form must reproduce the above copyright
 *	  notice, this list of conditions and the following disclaimer in the
 *	  documentation and/or other materials provided with the
 *	  distribution.
 *
 *	* Neither the name of Texas Instruments Incorporated nor the names of
 *	  its contributors may be used to endorse or promote products derived
 *	  from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdint.h>
#include <pru_cfg.h>
#include <pru_ctrl.h>
#include "resource_table_empty.h"

volatile register uint32_t __R30;
volatile register uint32_t __R31;

/* Scratchpad constants */
#define SCRATCHPAD_0	10
#define SCRATCHPAD_1	11
#define SCRATCHPAD_2	12

/* Specific register numbers shared through the Scratchpads */
#define CONFIG_REG	15
#define DELAY_REG	21
#define RESULT_REG	22
#define STATUS_REG	28
#define REQUEST_REG	29

/* Request register bit mapping */
#define START_BIT	(1 << 0)
#define CONTINUOUS_BIT	(1 << 1)
#define RECV_READY_BIT	(1 << 2)
#define RECV_ACK_BIT	(1 << 3)

/* Response register bit mapping */
#define SEND_READY_BIT	(1 << 0)
#define SEND_ACK_BIT	(1 << 1)
#define SEND_DONE_BIT	(1 << 2)

/* Sample/config mode */
#define ONE_SHOT	0
#define CONTINUOUS	1

/* ADS8688 configuration definitions */
/* Command Register formatting */
#define CMD_REG(x)		(x << 8)
#define NOOP			0x00
#define STDBY			0x82
#define PWR_DN			0x83
#define AUTO_RST		0xA0
#define RST			0x85
#define MAN_CH(chan)		(0xC0 | (4 * chan))
#define MAN_AUX			0xE0
/* Program Register formatting */
#define PROG_REG(reg, rw, data)	((reg << 9) | (rw << 8) | data)
#define AUTO_SEQ_EN		(0x01)
#define CHAN_PWR_DN		(0x02)
#define CHAN_INPUT_RANGE(chan)	(0x05 + chan)
#define COMM_READ_BACK		(0x3F)
/* Read or write for the program register formatting */
#define WRITE			1
#define READ			0
/* Channel input range constants */
#define PLUSMINUS25VREF		0
#define PLUSMINUS125VREF	1
#define PLUSMINUS0625VREF	2
#define PLUS25VREF		5
#define PLUS125VREF		6

/* Oversampling constants (must be a power of 2 to achieve timings in the PRU) */
#define OVERSAMPLE		1

/* Frequency estimation constants */
#define FREQ_EST_DEVICE		5	//1 based index to match silkscreen
#define FREQ_EST_CHANNEL	5
#define FREQ_EST_CYCLES		5
#define MIN_DELAY		710	//55Hz
#define DEFAULT_DELAY		781	//50Hz
#define DEFAULT_COMP		2	//50Hz
#define MAX_DELAY		868	//45Hz

/* Information about devices and channels connected */
#define CHANS_PER_DEVICE	8
#define DEVICES			6
#define NUM_CHANS		CHANS_PER_DEVICE * DEVICES
#define SAMPLES_PER_CYCLE	640

/* Ping and Pong buffer information */
#define PING_ADDR	0x9FFC0000
#define PONG_ADDR	0x9FFE0000
#define PING		0
#define PONG		1
#define DATA_READY	0x0000FFFF
#define IN_PROGRESS	0x00000000


uint32_t data_buf[DEVICES];
uint32_t config_buf[DEVICES];

/* Send cycle delay between samples value */
send_delay(uint32_t delay_val) {
	/* Send the cycle delay to the other PRU through Scratchpad 0 */
	__xout(SCRATCHPAD_0, DELAY_REG, 0, delay_val);
}

/* Send configurations or commands */
send_command(uint16_t continuous) {
	uint32_t	request, status = 0;
	uint32_t	temp;

	/* Check to make sure the SPI coprocessor is ready */
	while (!(status & SEND_READY_BIT)) {
		__xin(SCRATCHPAD_1, STATUS_REG, 0, temp);
		status = temp;
	} 
	/* Push the configuration values for each ADC */
	__xout(SCRATCHPAD_0, CONFIG_REG, 0, config_buf);
	
	/* Send the start command to kick off the transfer */
	request = START_BIT;
	if (continuous)
		request |= CONTINUOUS_BIT;
	__xout(SCRATCHPAD_1, REQUEST_REG, 0, request);

	/* Wait for SPI coprocessor to acknowledge the command */
	while (!(status & SEND_ACK_BIT)) {
		__xin(SCRATCHPAD_1, STATUS_REG, 0, temp);
		status = temp;
	} 

	/* Clear the start bit */
	request &= ~START_BIT;
	__xout(SCRATCHPAD_1, REQUEST_REG, 0, request);
}

/* Receive response */
receive_data() {
	uint32_t	request, status = 0;
	uint32_t	temp;

	/* Read the current request to get start and continuous */
	__xin(SCRATCHPAD_1, REQUEST_REG, 0, request);

	/* Set the ready bit, clear the ack bit */
	request |= RECV_READY_BIT;
	request &= ~RECV_ACK_BIT;
	__xout(SCRATCHPAD_1, REQUEST_REG, 0, request);

	/* Clear the ready bit, set the ack bit in the temp variable 
	 * Wait until after done signal is received to send 
	 */
	request &= ~RECV_READY_BIT;
	request |= RECV_ACK_BIT;

	/* Wait for the done signal */
	while (!(status & SEND_DONE_BIT)) {
		__xin(SCRATCHPAD_1, STATUS_REG, 0, temp);
		status = temp;
	} 

	/* Send ack bit one cycle before pulling data to save time */
	__xout(SCRATCHPAD_1, REQUEST_REG, 0, request);

	/* Pull the data into data_buf */
	__xin(SCRATCHPAD_2, RESULT_REG, 0, data_buf);
}

/* Send intial ADC configurations */
adc_init() {
	uint32_t status = 0;
	uint32_t temp;

	/* Check to make sure the SPI coprocessor is ready */
	while (!(status & SEND_READY_BIT)) {
		__xin(SCRATCHPAD_1, STATUS_REG, 0, temp);
		status = temp;
	}
 
	/* Send reset command to all devices in parallel */
	config_buf[0] = CMD_REG(RST);
	config_buf[1] = CMD_REG(RST);
	config_buf[2] = CMD_REG(RST);
	config_buf[3] = CMD_REG(RST);
	config_buf[4] = CMD_REG(RST);
	config_buf[5] = CMD_REG(RST);
	send_command(ONE_SHOT);
	receive_data();

	/* Set device 0 channel 0's input range to 0-2.5vRef */
	/*
	config_buf[0] = PROG_REG(CHAN_INPUT_RANGE(0), WRITE, PLUS125VREF);
	send_command(ONE_SHOT);
	receive_data();
	*/

	/* Enable all 8 channels on all devices */
	config_buf[0] = PROG_REG(AUTO_SEQ_EN, WRITE, 0xFF);
	config_buf[1] = PROG_REG(AUTO_SEQ_EN, WRITE, 0xFF);
	config_buf[2] = PROG_REG(AUTO_SEQ_EN, WRITE, 0xFF);
	config_buf[3] = PROG_REG(AUTO_SEQ_EN, WRITE, 0xFF);
	config_buf[4] = PROG_REG(AUTO_SEQ_EN, WRITE, 0xFF);
	config_buf[5] = PROG_REG(AUTO_SEQ_EN, WRITE, 0xFF);
	send_command(ONE_SHOT);
	receive_data();

	__delay_cycles(200);

	/* Put devices into auto reset mode in parallel */
	config_buf[0] = CMD_REG(AUTO_RST);
	config_buf[1] = CMD_REG(AUTO_RST);
	config_buf[2] = CMD_REG(AUTO_RST);
	config_buf[3] = CMD_REG(AUTO_RST);
	config_buf[4] = CMD_REG(AUTO_RST);
	config_buf[5] = CMD_REG(AUTO_RST);
	send_command(ONE_SHOT);
	receive_data();

	/* Initial delay set for 50Hz */
	send_delay(DEFAULT_DELAY);

	/* Send the NOOP command to kick off the sampling */
	config_buf[0] = CMD_REG(NOOP);
	config_buf[1] = CMD_REG(NOOP);
	config_buf[2] = CMD_REG(NOOP);
	config_buf[3] = CMD_REG(NOOP);
	config_buf[4] = CMD_REG(NOOP);
	config_buf[5] = CMD_REG(NOOP);
	send_command(CONTINUOUS);

	/* Enable the cycle counter */
	PRU0_CTRL.CTRL_bit.CTR_EN = 1;
	PRU0_CTRL.CYCLE = 0;

}

typedef struct {
	int32_t chan_data[CHANS_PER_DEVICE];
} adc_device;

#pragma DATA_SECTION(debug, ".shared_mem")
volatile far int32_t debug[10];

uint32_t chan_data_temp[NUM_CHANS];

void main(void)
{
	uint32_t i, j, idx;
	uint32_t oversample = 1;
	int32_t last_pol = 1;
	uint32_t zero_crossings = 0;
	uint32_t start, stop, elapsed, delay, compensation = 0;
	uint32_t zero = 0;
	uint32_t curr_addr;
	uint8_t curr_buff;
	uint32_t num_captured = 0;

	delay = DEFAULT_DELAY;
	compensation = DEFAULT_COMP;

	/* Select the ping buffer first */
	curr_addr = PING_ADDR + 8;
	curr_buff = PING;

	/* This PRU comes up first, zero out the status registers */
	__xout(SCRATCHPAD_1, STATUS_REG, 0, zero);
	__xout(SCRATCHPAD_1, REQUEST_REG, 0, zero);

	/* Clear SYSCFG[STANDBY_INIT] to enable OCP master port */
	CT_CFG.SYSCFG_bit.STANDBY_INIT = 0;

	/* ADC init */
	adc_init();

	while (1) {
		for (i = 0; i < CHANS_PER_DEVICE; i++) {
			/* receive parallel data from the ADCs */
			if (i < compensation)
				send_delay(delay + 1);
			else
				send_delay(delay);
			receive_data();
			for (j = 0; j < DEVICES; j++) {
				idx = j*CHANS_PER_DEVICE+i;

				/* Clear the accumulated value if first sample */
				if (oversample == 1)
					chan_data_temp[idx] = 0;

				/* Accumulate samples until oversample value is reached */
				chan_data_temp[idx] += (uint16_t)data_buf[j];

				/* Perform oversampling */
				if (oversample == OVERSAMPLE) {
					chan_data_temp[idx] = (uint16_t)((chan_data_temp[idx] + (OVERSAMPLE/2)) / OVERSAMPLE);
					/* Frequency estimation using zero crossings */
					if (j == (FREQ_EST_DEVICE - 1) && i == FREQ_EST_CHANNEL) {
						if (((last_pol > 0) && ((int16_t)(chan_data_temp[idx] - 0x8000) < 0)) || 
						    ((last_pol < 0) && ((int16_t)(chan_data_temp[idx] - 0x8000) > 0))) {
							zero_crossings++;
							last_pol = ~last_pol;

							/* First crossing, get the start timestamp */
							if (zero_crossings == 1) {
								start = PRU0_CTRL.CYCLE;
							}
							/* Last crossing, get the stop timestamp */
							else if (zero_crossings == ((FREQ_EST_CYCLES * 2) + 1)) {
								stop = PRU0_CTRL.CYCLE;
								elapsed = stop - start;
								delay = elapsed/(SAMPLES_PER_CYCLE * FREQ_EST_CYCLES);
								compensation = delay % CHANS_PER_DEVICE;
								delay = delay / CHANS_PER_DEVICE;
								if (delay < MIN_DELAY) {
									delay = MIN_DELAY;
									compensation = 0;
								}
								else if (delay > MAX_DELAY) {
									delay = MAX_DELAY;
									compensation = CHANS_PER_DEVICE - 1;
								}
								PRU0_CTRL.CYCLE = 0;
								zero_crossings = 0;
							}
						}
					}
				}
			}
		}
		if (oversample++ == OVERSAMPLE) {
			oversample = 1;
			/* Copy the oversampled data from each channel to the current buffer */
			memcpy((void *)curr_addr, chan_data_temp, CHANS_PER_DEVICE * DEVICES * 4);
			/* Increment the address for the next set of channel data */
			curr_addr += CHANS_PER_DEVICE * DEVICES * 4;

			/* A full sample has been captured, switch buffers */
			if (++num_captured == SAMPLES_PER_CYCLE) {
				if (curr_buff == PING) {
					/* Mark the first 4 bytes of the ping buffer with the ready flag */
					*(volatile uint32_t *)PING_ADDR = DATA_READY;
					/* Mark the next 4 bytes of the ping buffer with delay and comp values */
					*(volatile uint32_t *)(PING_ADDR + 4) = ((uint16_t)compensation) << 16 | ((uint16_t) delay);
					/* Mark the first 4 bytes of the pong buffer with the in progress flag */
					*(volatile uint32_t *)PONG_ADDR = IN_PROGRESS;
					/* Set the current address for samples to the address after the flag */
					curr_addr = PONG_ADDR + 8;
					/* Update the current buffer */
					curr_buff = PONG;
				}
				else {
					/* Mark the first 4 bytes of the pong buffer with the ready flag */
					*(volatile uint32_t *)PONG_ADDR = DATA_READY;
					/* Mark the next 4 bytes of the pong buffer with delay and comp values */
					*(volatile uint32_t *)(PONG_ADDR + 4) = ((uint16_t)compensation) << 16 | ((uint16_t) delay);
					/* Mark the first 4 bytes of the ping buffer with the in progress flag */
					*(volatile uint32_t *)PING_ADDR = IN_PROGRESS;
					/* Set the current address for samples to the address after the flag */
					curr_addr = PING_ADDR + 8;
					/* Update the current buffer */
					curr_buff = PING;
				}
				num_captured = 0;
			}
		}
	}
}

