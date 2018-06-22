OBJS = start.o boot.o

all: $(OBJS)
	$(LD) $(LDFLAGS) -r -o bootloader.o $(OBJS) > bootloader.map

.s.o:
	$(AS) $(ASFLAGS) $*.o $<

.c.o:
	$(CC) $(CCFLAGS) $*.o $<

clean:
	rm -f *.o
	rm -f *.map
