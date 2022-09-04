objects = src/scanner.o src/aux.o src/cmm.o
LDFLAGS = -lfl
LEX     = flex
CC      = gcc

main: $(objects) build 
	$(CC) -o build/cmm $(objects) $(LDFLAGS)

build: 
	mkdir build

src/scanner.o: src/scanner.c 
	$(CC) $(CFLAGS) -o $@ -c $^

src/scanner.c: src/scanner.l
	$(LEX) $(LFLAGS) -o $@ $^


.PHONY: clean

clean:
	rm -fr build src/*.o src/scanner.c 
