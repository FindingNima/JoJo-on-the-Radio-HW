.PHONY: clean all tarball

all: prodCon


# These targets take care of the compilation process.
prodCon: 
	gcc -c hw2_2.c
	gcc -o producer hw2_2.c -lpthread -lm
	gcc -c hw_2cons.c
	gcc -o consumer hw_2cons.c -lpthread
