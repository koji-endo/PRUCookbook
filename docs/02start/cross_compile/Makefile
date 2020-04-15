CC := /opt/clpru/bin/clpru
LNK := /opt/clpru/bin/lnkpru
PRU_SUPPORT:= /home/endo/Documents/workdir/PRUCookbook/pru-software-support-package

all:
	$(CC) -version

hello.obj:hello.c
	$(CC) --include_path=/opt/clpru/include --include_path=../../common --include_path=$(PRU_SUPPORT)/include --include_path=$(PRU_SUPPORT)/include/am335x -v3 -O2  --printf_support=minimal --display_error_number --endian=little --hardware_mac=on --obj_directory=/tmp/pru1_0-gen --pp_directory=/tmp/pru1_0-gen --asm_directory=/tmp/pru1_0-gen -ppd -ppa --asm_listing --c_src_interlist -DAI=1  -D=PRUN=1_0 -fe /tmp/pru1_0-gen/hello.obj hello.c

hello:hello.obj
	$(LNK) -i/opt/clpru/lib -i/opt/clpru/include --reread_libs --warn_sections --stack_size=0x100 --heap_size=0x100 -m /tmp/pru1_0-gen/hello.map -o /tmp/pru1_0-gen/hello.out /tmp/pru1_0-gen/hello.obj AM335x_PRU.cmd --library=libc.a --library=$(PRU_SUPPORT)/lib/rpmsg_lib.lib /tmp/pru1_0-gen/hello.obj
