TOOL_CHAIN = arm-none-eabi-
TOOL_CHAIN_POS = ~/Documents/gcc-arm-none-eabi-7-2017-q4-major

CC = $(TOOL_CHAIN_POS)/bin/$(TOOL_CHAIN)gcc
CCFLAGS = -Wall -c -o
LD = $(TOOL_CHAIN_POS)/arm-none-eabi/bin/ld
LDFLAGS = -M -s -x
LDSCRIPT = PracticeOS.lds
export CC CCFLAGS LD LDFLAGS

COMPOENTS = boot/bootloader.o kernel/kernel.o peripherals/periph.o

all: $(COMPOENTS)
	$(LD) --no-dynamic-linker -L $(TOOL_CHAIN_POS)/lib/gcc/arm-none-eabi/7.2.1/hard -lgcc -mfloat-abi=softfp -pie -T $(LDSCRIPT) $(LDFLAGS) $(COMPOENTS) > PracticeOS.map

boot/bootloader.o:
	cd boot; make

fs/fs.o:

kernel/kernel.o:
	cd kernel; make

peripherals/periph.o:
	cd peripherals; make

clean:
	rm -f *.map
	rm -f *.o
	cd boot; make clean
	cd kernel; make clean
	cd peripherals; make clean
