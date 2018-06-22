OBJS = interrupts.o

all: $(OBJS)
	$(LD) $(LDFLAGS) -r -o kernel.o $(OBJS) > kernel.map

.c.o：
	$(CC) $(CCFLAGS) $*.o $<

clean:
	rm -f *.o
	rm -f *.map
