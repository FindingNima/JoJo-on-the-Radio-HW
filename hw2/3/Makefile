all: prodCon

prodCon: 
	gcc -c producer.c
	gcc -o producer producer.c -lpthread -lm
	gcc -c consumer.c
	gcc -o consumer consumer.c -lpthread