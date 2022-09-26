# Feito por Guilherme Carbonari Boneti, GRR20196478, 02/2021.

CFLAGS = -Wall  # flag de compilacao
LDLIBS = -lm -std=c99

# arquivos-objeto
objects = image.o linked-list.o

mosaico: mosaico.c $(objects)
	gcc mosaico.c -o mosaico $(objects) $(CFLAGS) $(LDLIBS)
image.o: image.c linked-list.o image.h linked-list.h
	gcc -c image.c $(CFLAGS) $(LDLIBS)
linked-list.o: linked-list.c linked-list.h
	gcc -c linked-list.c $(CFLAGS) $(LDLIBS)
purge: clean
	-rm -f mosaico
clean:
	-rm -f $(objects)