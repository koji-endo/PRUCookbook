# uname -mで判定できる.
#
CROSS_COMPILE?=arm-linux-gnueabihf-

CC = $(CROSS_COMPILE)gcc
adc.out:adc.c
	$(CC) -o $@ $^ -L/home/endo/Documents/workdir/am335x_pru_package/pru_sw/app_loader/lib/ -lprussdrv
	# scp /home/debian/am335x_pru_package/pru_sw/app_loader/lib/libprussdrv.so 192.168.0.213:interrupt
	# scp $@ 192.168.0.213:interrupt
readdev:readdev.c
	gcc -o $@ $^

.PHONY:run

run:
	sudo LD_LIBRARY_PATH=/home/debian/am335x_pru_package/pru_sw/app_loader/lib ./adc hoge.txt

