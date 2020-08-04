; 
; Copyright (C) 2017 Texas Instruments Incorporated - http://www.ti.com/
; 
; 
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
; 
;        * Redistributions of source code must retain the above copyright
;          notice, this list of conditions and the following disclaimer.
; 
;        * Redistributions in binary form must reproduce the above copyright
;          notice, this list of conditions and the following disclaimer in the
;          documentation and/or other materials provided with the
;          distribution.
; 
;        * Neither the name of Texas Instruments Incorporated nor the names of
;          its contributors may be used to endorse or promote products derived
;          from this software without specific prior written permission.
; 
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
; "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
; OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

; Resource table needed for remoteproc Linux driver
	.global	||pru_remoteproc_ResourceTable||
	.sect	".resource_table:retain", RW
	.retain
	.align	1
	.elfsym	||pru_remoteproc_ResourceTable||,SYM_SIZE(20)
||pru_remoteproc_ResourceTable||:
	.bits	1,32			; pru_remoteproc_ResourceTable.base.ver @ 0
	.bits	0,32			; pru_remoteproc_ResourceTable.base.num @ 32
	.bits	0,32			; pru_remoteproc_ResourceTable.base.reserved[0] @ 64
	.bits	0,32			; pru_remoteproc_ResourceTable.base.reserved[1] @ 96
	.bits	0,32			; pru_remoteproc_ResourceTable.offset[0] @ 128

; Pins used to mimic the SPI interfaces
sclk_pin	.set	13
cs12_pin	.set	6
cs34_pin	.set	6
cs56_pin	.set	6
mosi1_pin	.set	7
mosi2_pin	.set	8
mosi3_pin	.set	9
mosi4_pin	.set	10
mosi5_pin	.set	11
mosi6_pin	.set	12
miso1_pin	.set	0
miso2_pin	.set	1
miso3_pin	.set	2
miso4_pin	.set	3
miso5_pin	.set	4
miso6_pin	.set	5	

; Request/response register bit mapping
send_ready_bit	.set	0
send_ack_bit	.set	1
send_done_bit	.set	2

start_bit	.set	0
continuous_bit	.set	1
recv_ready_bit	.set	2
recv_ack_bit	.set	3

; Register to contain the worst case delay to check for overflow
worst_delay_reg	.set	r6

; Specific register numbers shared through the Scratchpads */
status_reg	.set	r28
request_reg	.set	r29
adc1_config_reg	.set	r15
adc2_config_reg	.set	r16
adc3_config_reg	.set	r17
adc4_config_reg	.set	r18
adc5_config_reg	.set	r19
adc6_config_reg	.set	r20
cycle_count_reg	.set	r21
adc1_result_reg	.set	r22
adc2_result_reg	.set	r23
adc3_result_reg	.set	r24
adc4_result_reg	.set	r25
adc5_result_reg	.set	r26
adc6_result_reg	.set	r27

; Macros to take the clock pin high and low
	.sect	".text"
clock_high	.macro
	set r30, r30, sclk_pin
		.endm
clock_low	.macro
	clr r30, r30, sclk_pin
		.endm

; Macro to poke the ADC configurations into specific bits (pins)
config_bit	.macro reg, bit, pin
	qbbs label1?, reg, bit
	jmp label2?
label1?:	
	set r8, r8, pin
label2?:
		.endm
; Macro to handle a configuration cycle
config_cycle	.macro bit 
	mov	r8, r9
	config_bit adc1_config_reg, bit, mosi1_pin
	config_bit adc2_config_reg, bit, mosi2_pin
	config_bit adc3_config_reg, bit, mosi3_pin
	config_bit adc4_config_reg, bit, mosi4_pin
	clock_high
	config_bit adc5_config_reg, bit, mosi5_pin
	config_bit adc6_config_reg, bit, mosi6_pin
	mov	r30, r8
	nop
	nop
	nop
	nop
	clock_low
		.endm

; Macro to pick the ADC data from each specific bit (pin)
read_bit	.macro reg, bit, pin
	qbbs label3?, r11, pin
	jmp label4?
label3?:	
	set reg, reg, bit
label4?:
		.endm
; Macro to handle a read cycle
read_cycle	.macro bit 
	mov r11, r31
	clock_low
	read_bit adc1_result_reg, bit, miso1_pin
	read_bit adc2_result_reg, bit, miso2_pin
	read_bit adc3_result_reg, bit, miso3_pin
	read_bit adc4_result_reg, bit, miso4_pin
	nop
	clock_high
	read_bit adc5_result_reg, bit, miso5_pin
	read_bit adc6_result_reg, bit, miso6_pin
	nop
	nop
	nop
	nop
		.endm

	.sect	".text:main"
	.clink
	.global	||main||

||main||:
	; Set all pins high
	LDI32	r30, 0xffffffff
	; Load the values used to set and clear all CS pins in a single cycle
	ZERO	&r13, 4
	OR	r13, r13, (1 << cs12_pin)
	OR	r13, r13, (1 << cs34_pin)
	OR	r13, r13, (1 << cs56_pin)
	NOT	r12, r13

	; r9 is used as a known starting point for creating the configuration cycle
	; need the SCLK pin to be high becuase when the data is pushed to the pins
	; the clock needs to be high
	LDI32	r9, (1 << sclk_pin)

	; Load r6 with the worst case delay to check for overflow
	LDI32	r6, 0x00ffffff

	; Configure cycle counter
	; Load r0 with PRU_CTRL registers offset
	LDI32	r0, 0x24000
	; Clear the CTR_EN bit
	LBBO	&r1, r0, 0, 4
	CLR	r1, r1, 0x3
	SBBO	&r1, r0, 0, 4
	; Reset the COUNTER register to 0
	LDI32	r3, 0x0
	SBBO	&r3, r0, 12, 4
	; Set the CTR_EN bit to kick off the cycle counter
	LBBO	&r1, r0, 0, 4
	SET	r1, r1, 0x3
	SBBO	&r1, r0, 0, 4

	; zero out the result registers
	zero	&adc1_result_reg, 24

	; clear the status register
	ZERO	&status_reg, 4
	XOUT	11, &status_reg.b0, 4

||$C$L1||:    
        .newblock
	; Set ready bit and clear ack bit and push to the scratchpad
	CLR	status_reg, status_reg, send_ack_bit
	SET	status_reg, status_reg, send_ready_bit
	XOUT	11, &status_reg.b0, 4

	; Tight loop to check to check for start or continuous bit
check_start:
	XIN	11, &request_reg.b0, 4
	QBBS	continuous, request_reg, continuous_bit 	
	QBBS	start, request_reg, start_bit 	
        JMP	check_start

	; Continuous bit is set so we wait for the cycles between sample to expire
continuous:
	; If continuous AND start bits are set then we reset the cycle counter
	; because this is the first iteration and we don't know where the cycle
	; counter is. If the start bit is clear then we jump down past the cycle
	; counter reset becuase the cycle counter contains the number of cycles
	; since the last sample
	QBBC	no_reset, request_reg, start_bit
	; reset cycle counter
	zero	&r3, 4
	SBBO	&r3, r0, 12, 4
no_reset:
	; Pull in the new cycle count between samples value from the scratchpad
	XIN	10, &cycle_count_reg.b0, 4
	; Get current cycle count
	LBBO	&r4, r0, 12, 4
	; Subtract current elapsed cycles from cycles between samples
	SUB	cycle_count_reg, cycle_count_reg, r4
	; Subtract cycles between getting the count and checking it
	SUB	cycle_count_reg, cycle_count_reg, 8
	; Check if the remaining count is even or odd
	QBBC	even, cycle_count_reg, 0
	; Subtract 2 more cycles if the remainging count is odd
	SUB	cycle_count_reg, cycle_count_reg, 2
even:
	; Skip the wait if the counter has rolled over, otherwise we would wait
	; for more than 20 seconds for the counter to expire
	QBLT	reached, cycle_count_reg, worst_delay_reg
	; Wait until cycle count is reached
not_reached:
	SUB	cycle_count_reg, cycle_count_reg, 2
	QBLT	not_reached, cycle_count_reg, 2
reached:
	; Reset_cycle_counter
	zero	&r3, 4
	SBBO	&r3, r0, 12, 4

start:
	; Pull the configurations for each ADC from the scratchpad registers
	XIN	10, &adc1_config_reg.b0, 24

	; Clear the ready bit and set the ack bit
	CLR	status_reg, status_reg, send_ready_bit
	SET	status_reg, status_reg, send_ack_bit
	XOUT	11, &status_reg.b0, 4

	; Take all CS low to start the acquisition
        AND	r30, r30, r12
	; Send the configuration data from the scratchpad registers one cycle at a time
	config_cycle 15
	config_cycle 14
	config_cycle 13
	config_cycle 12
	config_cycle 11
	config_cycle 10
	config_cycle 9
	config_cycle 8
	config_cycle 7
	config_cycle 6
	config_cycle 5
	config_cycle 4
	config_cycle 3
	config_cycle 2
	config_cycle 1
	config_cycle 0
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	clock_high
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	nop
	; Read the data from the ADCs and place it into the result registers one cycle at a time
	read_cycle 15
	read_cycle 14
	read_cycle 13
	read_cycle 12
	read_cycle 11
	read_cycle 10
	read_cycle 9
	read_cycle 8
	read_cycle 7
	read_cycle 6
	read_cycle 5
	read_cycle 4
	read_cycle 3
	read_cycle 2
	read_cycle 1
	read_cycle 0

	; Take all CS high
	OR        r30, r30, r13

	; Wait until the controller is ready for data
check_ready:
	XIN	11, &request_reg.b0, 4
	QBBC	check_ready, request_reg, recv_ready_bit

	; Controller is ready, put the result data in the scratchpad register
	XOUT	12, &adc1_result_reg.b0, 24

	; Set the done bit
	SET	status_reg, status_reg, send_done_bit
	XOUT	11, &status_reg.b0, 4

	; Wait until the controller acknowledges the data
check_ack:
	XIN	11, &request_reg.b0, 4
	QBBC	check_ack, request_reg, recv_ack_bit

	; Clear the done bit
	CLR	status_reg, status_reg, send_done_bit
	XOUT	11, &status_reg.b0, 4

	; Clear the local result register for the next set of results
	zero	&adc1_result_reg, 24

	; Jump to the top of the loop
	JMP       ||$C$L1||
