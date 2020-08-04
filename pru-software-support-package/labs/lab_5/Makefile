
# uname -mで判定できる.
#
CROSS_COMPILE?=arm-linux-gnueabihf-

CC = $(CROSS_COMPILE)gcc
%.out:%.c
	$(CC) -o $@ $^ 
