CROSS_COMPILER = arm-none-eabi-gcc

CC = $(CROSS_COMPILER)
CCFLAGS = -Wall -c -o
LD = ld
LDFLAGS = -M -s -x
LDSCRIPT = PracticeOS.lds
AS = as
ASFLAGS = -o
export CC CCFLAGS LD LDFLAGS AS ASFLAGS

COMPOENTS = boot/bootloader.o fs/fs.o include/lib.o kernel/kernel.o peripherals/periph.o

all: $(COMPOENTS)
	$(LD) -pie -T $(LDSCRIPT) $(LDFLAGS) $(COMPOENTS) > PracticeOS.map

boot/bootloader.o:
	cd boot; make

fs/fs.o:

include/lib.o:
	cd include; make

kernel/kernel.o:
	cd kernel; make

peripherals/periph.o:
	cd peripherals; make

clean:
	rm -f *.map
	rm -f *.o
	cd boot; make clean
	cd include; make clean
	cd kernel; make clean
	cd peripherals; make clean
