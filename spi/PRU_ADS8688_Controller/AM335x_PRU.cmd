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
 *
 *
 * AM335x_PRU.cmd
 * Description: This file is a linker command file that can be used for
 *              linking PRU programs built with the C compiler and the
 *              resulting .out file on an AM335x device.
 */

/* Link using C conventions */
-cr

/* Specify the System Memory Map */
MEMORY
{
      PAGE 0:
	PRU_IMEM		: org = 0x00000000 len = 0x00002000  /* 8kB PRU0 Instruction RAM */

      PAGE 1:

	/* RAM */

	PRU_DMEM_0_1	: org = 0x00000000 len = 0x00002000 CREGISTER=24 /* 8kB PRU Data RAM 0_1 */
	PRU_DMEM_1_0	: org = 0x00002000 len = 0x00002000 CREGISTER=25 /* 8kB PRU Data RAM 1_0 */

	  PAGE 2:
	PRU_SHAREDMEM	: org = 0x00010000 len = 0x00003000 CREGISTER=28 /* 12kB Shared RAM */

	DDR			: org = 0x80000000 len = 0x1F800000	CREGISTER=31
	PRUADC_DDR_CARVEOUT	: org = 0x9F800000 len = 0x00800000	
	L3OCMC			: org = 0x40000000 len = 0x00010000	CREGISTER=30


	/* Peripherals */

	PRU_CFG			: org = 0x00026000 len = 0x00000044	CREGISTER=4
	PRU_ECAP		: org = 0x00030000 len = 0x00000060	CREGISTER=3
	PRU_IEP			: org = 0x0002E000 len = 0x0000031C	CREGISTER=26
	PRU_INTC		: org = 0x00020000 len = 0x00001504	CREGISTER=0
	PRU_UART		: org = 0x00028000 len = 0x00000038	CREGISTER=7

	DCAN0			: org = 0x481CC000 len = 0x000001E8	CREGISTER=14
	DCAN1			: org = 0x481D0000 len = 0x000001E8	CREGISTER=15
	DMTIMER2		: org = 0x48040000 len = 0x0000005C	CREGISTER=1
	PWMSS0			: org = 0x48300000 len = 0x000002C4	CREGISTER=18
	PWMSS1			: org = 0x48302000 len = 0x000002C4	CREGISTER=19
	PWMSS2			: org = 0x48304000 len = 0x000002C4	CREGISTER=20
	GEMAC			: org = 0x4A100000 len = 0x0000128C	CREGISTER=9
	I2C1			: org = 0x4802A000 len = 0x000000D8	CREGISTER=2
	I2C2			: org = 0x4819C000 len = 0x000000D8	CREGISTER=17
	MBX0			: org = 0x480C8000 len = 0x00000140	CREGISTER=22
	MCASP0_DMA		: org = 0x46000000 len = 0x00000100	CREGISTER=8
	MCSPI0			: org = 0x48030000 len = 0x000001A4	CREGISTER=6
	MCSPI1			: org = 0x481A0000 len = 0x000001A4	CREGISTER=16
	MMCHS0			: org = 0x48060000 len = 0x00000300	CREGISTER=5
	SPINLOCK		: org = 0x480CA000 len = 0x00000880	CREGISTER=23
	TPCC			: org = 0x49000000 len = 0x00001098	CREGISTER=29
	UART1			: org = 0x48022000 len = 0x00000088	CREGISTER=11
	UART2			: org = 0x48024000 len = 0x00000088	CREGISTER=12

	RSVD10			: org = 0x48318000 len = 0x00000100	CREGISTER=10
	RSVD13			: org = 0x48310000 len = 0x00000100	CREGISTER=13
	RSVD21			: org = 0x00032400 len = 0x00000100	CREGISTER=21
	RSVD27			: org = 0x00032000 len = 0x00000100	CREGISTER=27

}

/* Specify the sections allocation into memory */
SECTIONS {
	/* Forces _c_int00 to the start of PRU IRAM. Not necessary when loading
	   an ELF file, but useful when loading a binary */
	.text:_c_int00*	>  0x0, PAGE 0

	.text		>  PRU_IMEM, PAGE 0
	.stack		>  PRU_DMEM_0_1, PAGE 1
	.bss		>  PRU_DMEM_0_1, PAGE 1
	.cio		>  PRU_DMEM_0_1, PAGE 1
	.data		>  PRU_DMEM_0_1, PAGE 1
	.switch		>  PRU_DMEM_0_1, PAGE 1
	.sysmem		>  PRU_DMEM_0_1, PAGE 1
	.cinit		>  PRU_DMEM_0_1, PAGE 1
	.rodata		>  PRU_DMEM_0_1, PAGE 1
	.rofardata	>  PRU_DMEM_0_1, PAGE 1
	.farbss		>  PRU_DMEM_0_1, PAGE 1
	.fardata	>  PRU_DMEM_0_1, PAGE 1

	.shared_mem	>  PRU_SHAREDMEM, PAGE 2
	.pru1_mem	>  PRU_DMEM_1_0, PAGE 1
	.pruadc_ddr_carveout	>  PRUADC_DDR_CARVEOUT, PAGE 2
	.resource_table > PRU_DMEM_0_1, PAGE 1
}
