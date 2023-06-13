obj-m += driver.o
KDIR = /lib/modules/5.15.92-rt57-v8+/build
CROSS = ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
all:
	make $(CROSS) CFLAGS="-std=c90" -C $(KDIR) M=$(shell pwd) modules
	/usr/bin/aarch64-linux-gnu-gcc -o hw1 hw1.c
clean:
	make $(CROSS) CFLAGS="-std=c90" -C $(KDIR) M=$(shell pwd) clean
	rm hw1