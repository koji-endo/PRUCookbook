; This is an example of how to call an assembly routine from C with a retun value.
;	Mark A. Yoder, 9-July-2018
delay .macro
  LDI32 r15, 1
;  LDI32 r15, 100000000 
delaycycle?:
	sub		r15,   r15, 1
  qbne	delaycycle?, r15, 0
     .endm
	.cdecls "pulse_test.c"
	.global spi_transfer
spi_transfer:
  LDI32 r8,0x2;
  LDI32 r9,0x0;
;first cycle
  LDI32  r30, 0x6
  
  nop
;next cycle
  LDI32 r30,0x4
  
  nop
;nextr9 cycle
  LDI32 r30,0x06
  
  nop
;next cycle
  LDI32  r30, 0x4
  
  LSR r6, r14, 23

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 22
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 21
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 20
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 19
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 18
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 17
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 16
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 15
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 14
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 13
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 12
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 11
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 10
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 9
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 8
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 7
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 6
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 5
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 4
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 3
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 2
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 1
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  LSR r6, r14, 0
  OR r30, r9, r7
  

  AND r7,r6,1
  OR r30, r8, r7
  
  nop
  OR r30, r9, r7
  
  JMP r3.w2
