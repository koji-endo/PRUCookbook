; This is an example of how to call an assembly routine from C with a retun value.
;	Mark A. Yoder, 9-July-2018
delay .macro
  mov r15, r14
;  LDI32 r15, 100000000 
delaycycle?:
	sub		r15,   r15, 1
  qbne	delaycycle?, r15, 0
     .endm
	.cdecls "pixel_test.c"
	.global parallel
parallel:

; 18,19,20は000 (13,14.15)
  LDI32 r30,0;
  delay
  JMP foronezero
Forone:
  add r30,r30,1
  nop
  delay
foronezero:
  add r30,r30,1
  delay
  qbbc Forone, r30,13

; 18,19,20は001 (13,14.15)
  LDI32 r30,262144; 2^18
  delay
  JMP fortwozero
fortwo:
  add r30,r30,1
  delay
  nop
fortwozero:
  add r30,r30,1
  delay
  qbbs fortwo, r30, 13

; 18,19,20は010 (13,14.15)
; 18,19は10 (14.15)
  LDI32 r30,524288; 2^19
  delay
  JMP forthreezero
forthree:
  add r30,r30,1
  delay
  nop
forthreezero:
  add r30,r30,1
  delay
  qbbc forthree, r30, 13

; 18,19,20は011 (13,14.15)
  LDI32 r30,786432; 2^18
  delay
  JMP forfourzero
forfour:
  add r30,r30,1
  delay
  nop
forfourzero:
  add r30,r30,1
  delay
  qbbs forfour, r30,13

; 18,19,20は000 (13,14.15)
  LDI32 r30,1048576;
  delay
  JMP tforonezero
tforone:
  add r30,r30,1
  nop
  delay
tforonezero:
  add r30,r30,1
  delay
  qbbc tforone, r30,13

; 18,19,20は001 (13,14.15)
  LDI32 r30,1310720; 2^18
  delay
  JMP tfortwozero
tfortwo:
  add r30,r30,1
  delay
  nop
tfortwozero:
  add r30,r30,1
  delay
  qbbs tfortwo, r30,13

; 18,19,20は010 (13,14.15)
; 18,19は10 (14.15)
  LDI32 r30,1572864; 2^19
  delay
  JMP tforthreezero
tforthree:
  add r30,r30,1
  delay
  nop
tforthreezero:
  add r30,r30,1
  delay
  qbbc tforthree, r30,13

; 18,19,20は011 (13,14.15)
  LDI32 r30,1835008; 2^18
  delay
  JMP tforfourzero
tforfour:
  add r30,r30,1
  delay
  nop
tforfourzero:
  add r30,r30,1
  delay
  qbbs tforfour, r30,13


  JMP r3.w2
