arvoreBmais: main.o GabrielFelixDeSouzaLopes.o
	gcc -o arvoreBmais main.o GabrielFelixDeSouzaLopes.o

main.o: main.c btree.h
	gcc -c main.c -Wall

GabrielFelixDeSouzaLopes.o: GabrielFelixDeSouzaLopes.c btree.h
	gcc -c GabrielFelixDeSouzaLopes.c -Wall