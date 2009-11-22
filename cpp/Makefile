CC=g++
CFLAGS=-I. -Wall -Wextra -Weffc++ -Wold-style-cast -ggdb
CXXFLAGS=$(CFLAGS)
DEPS = GPolyEncoder.h
OBJ = GPolyEncoder.o gpolyencode_test.o

all: test

gpolyencode_test: $(OBJ)
	$(CC) -o $@ $^

test: gpolyencode_test

$(OBJ): $(DEPS)

clean:
	rm -f gpolyencode_test $(OBJ)
