CC=g++
CFLAGS=-I. -Wall -ggdb
DEPS = GPolyEncoder.h
OBJ = GPolyEncoder.o gpolyencode_test.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

gpolyencode_test: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

test: gpolyencode_test

clean:
	rm -f gpolyencode_test GPolyEncoder.o gpolyencode_test.o
