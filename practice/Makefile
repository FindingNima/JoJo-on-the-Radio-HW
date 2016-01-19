testmake: test.o
	gcc $(CFLAGS) -o testmake test.o
test.o: test.c
	gcc $(CFLAGS) -c test.c
clean: 
	rm -f *.o testmake