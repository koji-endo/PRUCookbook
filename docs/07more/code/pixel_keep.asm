; This is an example of how to call an assembly routine from C with a retun value.
;	Mark A. Yoder, 9-July-2018
delay .macro
  mov r15, r14
;  LDI32 r15, 100000000 
delaycycle?:
	sub		r15,   r15, 1
  qbne	delaycycle?, r15, 0
     .endm
	.cdecls "pulse_test.c"
	.global spi_transfer
parallel:
; 18,19は00 (14.15)
  LDI32 r30,0;
  delay
  JMP foronezero
forone:
  sub r30,r30,1048575
  nop
  delay
foronezero:
  add r30,r30,1048576
  delay
  qbbc forone, r30.t14

; 18,19は01 (14.15)
  LDI32 r30,262144; 2^18
  delay
  JMP fortwozero
fortwo:
  sub r30,r30,1048575
  delay
  nop
fortwozero:
  add r30,r30,1048576
  delay
  qbbs fortwo, r30.t14

; 18,19は10 (14.15)
  LDI32 r30,524288; 2^19
  delay
  JMP forthreezero
forthree:
  sub r30,r30,1048575
  delay
  nop
forthreezero:
  add r30,r30,1048576
  delay
  qbbc forthree, r30.t14

; 18,19は11 (14.15)
  LDI32 r30,786432; 2^18
  delay
  JMP forfourzero
forfour:
  sub r30,r30,1048575
  delay
  nop
forfourzero:
  add r30,r30,1048576
  delay
  qbbs forfour, r30.t14

  JMP r3.w2
