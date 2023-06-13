obj-m += mydev.o
KDIR = /lib/modules/5.15.92-rt57-v8+/build
CROSS = ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu-
all:
	make $(CROSS) -C $(KDIR) M=$(shell pwd) modules
	/usr/bin/aarch64-linux-gnu-gcc -o writer writer.c
	/usr/bin/aarch64-linux-gnu-g++ -o reader reader.cpp
clean:
	make $(CROSS) -C $(KDIR) M=$(shell pwd) clean
	rm writer
	rm reader