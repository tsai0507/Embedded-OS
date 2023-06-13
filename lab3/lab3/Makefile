obj-m += lab3_driver.o
KDIR = /lib/modules/5.15.92-rt57-v8+/build
CROSS = ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
all:
	make $(CROSS) -C $(KDIR) M=$(shell pwd) modules
	/usr/bin/aarch64-linux-gnu-gcc -o writer writer.c
clean:
	make $(CROSS) -C $(KDIR) M=$(shell pwd) clean
	rm writer