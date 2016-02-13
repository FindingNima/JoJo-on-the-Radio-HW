prodcon: prodcon.o
	gcc $(CFLAGS) -o prodcon prodcon.o -lpthread
prodcon.o: prodcon.c
	gcc $(CFLAGS) -c prodcon.c -lpthread
clean: 
	rm -f *.o prodcon

