objects = src/scanner.o src/parser.tab.o src/aux.o src/cmm.o src/analyze.o src/symbol_table.o
LDFLAGS = -lfl
PFLAGS  = -d
PARSER  = bison
LEX     = flex
CC      = gcc
EXAMPLE_FILES = test/examples/* 
EXAMPLE_PREFIX = test/examples/
EXAMPLE_FILENAME   = debug.c

# Ctest variables:
ctest_a     = ext/ctest_lib/lib/ctest.a
ctest_ref   = https://github.com/ArturAssisComp/ctest.git;
ctest_files = ext/ctest_lib/lib/ctest.a ext/ctest_lib/include/ctest.h  ext/ctest_lib/include/ctest_functions.h ext/ctest_lib/include/globals.h ext/ctest_lib/include/assert.h ext/ctest_lib/include/std_assert_macro.h

main: $(objects) build 
	$(CC) -o build/cmm $(objects) $(LDFLAGS)

build: 
	mkdir build

src/scanner.o: src/scanner.c src/parser.tab.c
	$(CC) $(CFLAGS) -o $@ -c src/scanner.c

src/scanner.c: src/scanner.l
	$(LEX) $(LFLAGS) -o $@ $^

src/parser.tab.c: src/parser.y
	$(PARSER) $(PFLAGS) -o $@ $^


build_test: build
	mkdir -p build/test


.PHONY: clean test run_examples

clean:
	rm -fr build src/*.o test/*.o src/scanner.c src/parser.tab.*

test: build_test $(objects) test/test_scanner.o
	$(CC) -o build/test/test_scanner test/test_scanner.o src/aux.o $(ctest_a) src/scanner.o src/parser.tab.o $(LDFLAGS)

run_examples: main  
	for filename in $(EXAMPLE_FILES) ; do \
		echo ;                        \
		echo "File" $${filename} ;         \
		cat $${filename} ;                \
	    echo "Executing" $${filename} ;   \
		build/cmm $${filename} ;          \
	done

run_example_file: main
	echo ;                        \
	echo "File" $(EXAMPLE_PREFIX)$(EXAMPLE_FILENAME) ;        \
	cat $(EXAMPLE_PREFIX)$(EXAMPLE_FILENAME) ;                \
	echo "Executing" $(EXAMPLE_PREFIX)$(EXAMPLE_FILENAME) ;   \
	build/cmm $(EXAMPLE_PREFIX)$(EXAMPLE_FILENAME) ;          




.PHONY: install
install: build_test
	cd build/test; git clone $(ctest_ref) 
	cd build/test/ctest; git checkout version1_0; make
	mkdir -p ext/ctest_lib/include
	mkdir -p ext/ctest_lib/lib
	cd build/test/ctest/build/ctest_lib/include; mv ctest.h ctest_functions.h globals.h assert.h std_assert_macros.h ../../../../../../ext/ctest_lib/include
	mv build/test/ctest/build/ctest_lib/lib/ctest.a ext/ctest_lib/lib
	rm -fr build/test/ctest


