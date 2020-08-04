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

#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <sched.h>
#include <pthread.h>
#include <limits.h>
#include <ncurses.h>

#define PING_ADDR		0x9FFC0000
#define PONG_ADDR		0x9FFE0000
#define PING			0
#define PONG			1
#define DATA_READY		0x0000FFFF
#define IN_PROGRESS		0x00000000
#define NUM_CHANNELS		48
#define NUM_DEVICES		6
#define NUM_SAMPLES_PER_CYCLE	640
#define MAX_CSV_SAMPLES		65536

volatile float min[NUM_CHANNELS];
volatile float max[NUM_CHANNELS];
volatile float result[NUM_CHANNELS];
volatile uint16_t compensation, delay;
volatile int16_t csv_samples[MAX_CSV_SAMPLES];
volatile uint16_t csv_channel = 0;
volatile uint32_t csv_sample_num = 0;
volatile uint32_t csv_counter = 0;

struct buffer {
	uint32_t flags;
	uint32_t comp_delay;
	uint32_t samples[NUM_CHANNELS*NUM_SAMPLES_PER_CYCLE];
};

void *capture_func(void *data)
{
        int i, j;
	struct buffer *ping_ptr, *pong_ptr;
	uint32_t samples[NUM_CHANNELS*NUM_SAMPLES_PER_CYCLE];
	uint64_t cur_square_sum = 0;
	int16_t cur_min = INT16_MAX;
	int16_t cur_max = INT16_MIN;
	int16_t signed_val;
	struct timespec sleep;
	uint8_t curr_buff = PING;

	/* Open the /dev/mem device to access the shared buffers */
        int fd = open("/dev/mem",O_RDWR|O_SYNC);
        if(fd < 0)
        {
                printf("Can't open /dev/mem\n");
                return NULL;
        }
	/* mmap 128kB ping and pong buffers */
        ping_ptr = (struct buffer *) mmap(0, getpagesize()*32, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PING_ADDR);
        pong_ptr = (struct buffer *) mmap(0, getpagesize()*32, PROT_READ|PROT_WRITE, MAP_SHARED, fd, PONG_ADDR);
        if(ping_ptr == NULL || pong_ptr == NULL)
        {
                printf("Can't mmap\n");
                return NULL;
        }

	/* put sine wave in memory */
	/*
	for(i = 0; i < NUM_SAMPLES_PER_CYCLE; i++) {
		for(j = 0; j < NUM_CHANNELS; j++) {
			ping_ptr->samples[i*NUM_CHANNELS+j] = sine_wave[i];
			pong_ptr->samples[i*NUM_CHANNELS+j] = sine_wave[i];
		}
	}
	*/

	/* Configure the sleep delay to 250us */
	sleep.tv_sec = 0;
	sleep.tv_nsec = 250000L;

	/* Wait until PING buffer is in progress before beginning */
	while (ping_ptr->flags != IN_PROGRESS)
		nanosleep(&sleep, NULL);

	while(1) {
		/* Wait for the buffers to be ready and then memcpy the sample
		 * data to local memory. If the buffer is not ready yet, go to
		 * sleep for 250us before checking again. This thread is
		 * running at the highest priority so it needs to sleep when
		 * not in use or it will starve all other threads.
		 */
		if (curr_buff == PING) {
			while (ping_ptr->flags != DATA_READY)
				nanosleep(&sleep, NULL);
			compensation = ping_ptr->comp_delay >> 16;
			delay = ping_ptr->comp_delay & 0xFFFF;
			memcpy(samples, ping_ptr->samples, sizeof(samples));
			curr_buff = PONG;
		}
		else {
			while (pong_ptr->flags != DATA_READY)
				nanosleep(&sleep, NULL);
			compensation = pong_ptr->comp_delay >> 16;
			delay = pong_ptr->comp_delay & 0xFFFF;
			memcpy(samples, pong_ptr->samples, sizeof(samples));
			curr_buff = PING;
		}

		/* Iterate through a full cycle for each channel */
		for (j = 0; j < NUM_CHANNELS; j++) {
			for(i = 0; i < NUM_SAMPLES_PER_CYCLE; i++) {
				/* Convert the ADC code to it's signed equivalent */
				signed_val = (uint16_t)samples[i*NUM_CHANNELS+j] - 0x8000;
				if (j == csv_channel && csv_counter < csv_sample_num)
					csv_samples[csv_counter++] = signed_val;
				/* Check for min and max values throughout the cycle */
				if(signed_val < cur_min)
					cur_min = signed_val;
				if(signed_val > cur_max)
					cur_max = signed_val;
				/* Accumulate the squares to be used in the RMS calc */
				cur_square_sum += signed_val * signed_val;
			}
			/* Convert the min and max from ADC codes to voltages and store them */
			min[j] = (float)cur_min * 0.0003125;
			max[j] = (float)cur_max * 0.0003125;
			/* Complete the Root Mean Square (RMS) calculation for this channel and store it */
			result[j] = (sqrt(cur_square_sum / 640.0)) * 0.0003125;
			/* Reset the current variables to defaults for the next channel iteration */
			cur_min = INT16_MAX;
			cur_max = INT16_MIN;
			cur_square_sum = 0;
		}
	}

        return NULL;
}

void *print_func(void *data)
{
	int i;
	float frequency;

        /* Resize the terminal to display all lines */
        system("resize -s 30 80");

	/* Initialize the screen */
	initscr();
	/* Set the timeout to 250ms for waiting for user input */
	timeout(250);
	curs_set(0);

	/* Print each channel's min, max, and RMS values to the screen */
	while (1) {
		for(i = 0; i < NUM_CHANNELS/2; i++) {
                        mvprintw(i, 0, "D%d:C%d Vpp:[%.3f,%.3f] ", i/8+1, i%8, min[i], max[i]);
			mvprintw(i, 28, "RMS:%.3f", result[i]);
			mvprintw(i, 39, "| D%d:C%d Vpp:[%.3f,%.3f] ", (i+NUM_CHANNELS/2)/8+1, i%8, min[i+NUM_CHANNELS/2], max[i+NUM_CHANNELS/2]);
			mvprintw(i, 69, "RMS:%.3f", result[i+NUM_CHANNELS/2]);
		}

		frequency = (1.0 / ((delay * NUM_CHANNELS / NUM_DEVICES + compensation) * 0.000000005)) / NUM_SAMPLES_PER_CYCLE;
		
		mvprintw(NUM_CHANNELS/2 + 1, 0, "Delay:%d, Compensation:%d", delay, compensation);
		mvprintw(NUM_CHANNELS/2 + 2, 0, "Estimated Frequency:%.3f Hz", frequency);
		mvprintw(NUM_CHANNELS/2 + 4, 0, "Press any key to exit");
		/* Refresh the screen with the latest data */
		refresh();
		/* If a character is received then exit the while(1) loop */
		if(getch() != -1)
			break;
	}

	/* Close the window */
	endwin();

	return NULL;
}

int main(int argc, char* argv[])
{
        struct sched_param param;
        pthread_attr_t attr;
        pthread_t capture_thread, print_thread;
        int ret;
	uint32_t i;
	FILE *fp;

	/* Check for command line arguments requesting to create a CSV file
	 * argv[1] - CSV file name
	 * argv[2] - Channel number to store results from
	 * argv[3] - Number of samples to store (max of 32000)
	 * example given:
	 * 	./ARM_User_Space_App.out chan3_2048samples.csv 3 2048
	 */ 
	if(argc == 4) {
		csv_sample_num = atoi(argv[3]);
		if (csv_sample_num > MAX_CSV_SAMPLES)
			csv_sample_num = MAX_CSV_SAMPLES;
		csv_channel = atoi(argv[2]);
		if (csv_channel > NUM_CHANNELS)
			csv_channel = 0;
	}
	

        /* Lock memory */
        if(mlockall(MCL_CURRENT|MCL_FUTURE) == -1) {
                printf("mlockall failed: %m\n");
                exit(-2);
        }
        /* Initialize pthread attributes (default values) */
        ret = pthread_attr_init(&attr);
        if (ret) {
                printf("init pthread attributes failed\n");
                goto out;
        }
        /* Set the stack size, need at least 0x1E000 for the samples array */
        ret = pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN + 0x20000);
        if (ret) {
            printf("pthread setstacksize failed\n");
            goto out;
        }
        /* Set scheduler policy and priority of pthread */
        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if (ret) {
                printf("pthread setschedpolicy failed\n");
                goto out;
        }
        param.sched_priority = 99;
        ret = pthread_attr_setschedparam(&attr, &param);
        if (ret) {
                printf("pthread setschedparam failed\n");
                goto out;
        }
        /* Use scheduling parameters of attr */
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if (ret) {
                printf("pthread setinheritsched failed\n");
                goto out;
        }
        /* Create a pthread with specified attributes (priority 99) */
        ret = pthread_create(&capture_thread, &attr, capture_func, NULL);
        if (ret) {
                printf("create capture_thread failed\n");
                goto out;
        }
        /* Create a pthread with lower priority to print the results */
        ret = pthread_create(&print_thread, NULL, print_func, NULL);
        if (ret) {
                printf("create capture_thread failed\n");
                goto out;
        }
        /* Join the print thread and wait until it is done (key press) */
        ret = pthread_join(print_thread, NULL);
        if (ret)
                printf("join capture_thread failed: %m\n");

	/* Write the samples to a csv file if requested */
	if(argc == 4) {
		fp = fopen(argv[1], "w+");
		for (i = 0; i < csv_sample_num; i++) {
			fprintf(fp, "%d\n", csv_samples[i]);
		}
		fclose(fp);

		printf("\n%s file created with %d sample(s) from channel %d\n\n", argv[1], csv_sample_num, csv_channel);
	}
	
out:
        return ret;
}
