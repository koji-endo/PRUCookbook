CC := /opt/clpru/bin/clpru
LNK := /opt/clpru/bin/lnkpru
PRU_SUPPORT:= /home/endo/Documents/workdir/PRUCookbook/pru-software-support-package

all:
	$(CC) -version

%.obj:%.c
	$(CC) --include_path=/opt/clpru/include --include_path=../../common --include_path=$(PRU_SUPPORT)/include --include_path=$(PRU_SUPPORT)/include/am335x -v3 -O2  --printf_support=minimal --display_error_number --endian=little --hardware_mac=on --obj_directory=/tmp/pru1_0-gen --pp_directory=/tmp/pru1_0-gen --asm_directory=/tmp/pru1_0-gen -ppd -ppa --asm_listing --c_src_interlist -DAI=1  -D=PRUN=1_0 -fe $@ $^

%.out:%.obj
	$(LNK) -i/opt/clpru/lib -i/opt/clpru/include --reread_libs --warn_sections --stack_size=0x100 --heap_size=0x100 -m /tmp/pru1_0-gen/hello.map -o $@ $^ AM335x_PRU.cmd --library=libc.a --library=$(PRU_SUPPORT)/lib/rpmsg_lib.lib $^

scp:
ifdef target
	echo "scp $(target).out beagle65:/tmp/pru/"
	scp $(target).out beagle65:/tmp/pru/
else
	echo "no target is set"
endif
