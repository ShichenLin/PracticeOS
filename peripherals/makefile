MODULES = uart/uart.o

periph.o: $(MODULES)
	$(LD) $(LDFLAGS) -r -o periph.o $(MODULES) > periph.map

uart/uart.o:
	cd uart; make

clean:
	rm -f *.o
	rm -f *.map
	cd uart; make clean
