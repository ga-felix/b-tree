arvoreBmais: main.o btree.o
	gcc -o arvoreBmais main.o btree.o

main.o: main.c btree.h
	gcc -c main.c

btree.o: btree.c btree.h
	gcc -c btree.c