objects = src/scanner.o
LDFLAGS = -lfl
LFLAGS  = --header-file=src/scanner.h 
LEX     = flex
CC      = gcc

main: $(objects) build 
	$(CC) -o build/scanner $(objects) $(LDFLAGS)

build: 
	mkdir build

src/scanner.o: src/scanner.c 
	$(CC) $(CFLAGS) -o $@ -c $^

src/scanner.c: src/scanner.l
	$(LEX) $(LFLAGS) -o $@ $^


.PHONY: clean

clean:
	rm -fr build src/*.o src/scanner.c src/scanner.h
