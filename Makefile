CC = gcc
CFLAGS = -Wall -Wextra -g
OFLAGS = -O3
SRC = src
LIB = target/build
TEST = test/test.c

libvec.so: $(SRC)/vec.c $(SRC)/vec.h
	@echo -e "\e[32m  Compiling\e[0m libvec v0.1.0"
	@$(CC) $(CFLAGS) $(OFLAGS) -fPIC -shared -o $@ src/vec.c -lc

libvec.o: $(SRC)/vec.c $(SRC)/vec.h
	@mkdir -p $(LIB)
	@echo -e "\e[32m  Compiling\e[0m vec v0.1.0"
	@$(CC) $(CFLAGS) -c $(SRC)/vec.c -o $(LIB)/$@

test: libvec.o
	@mkdir -p target/test
	@$(CC) $(CFLAGS) $(LIB)/libvec.o $(TEST) -o target/test/main
	@echo -e "   \e[32mFinished\e[0m debug (unoptimized + debugflags)"
	@echo -e "    \e[32mRunning\e[0m target/test/main"
	@target/test/main

test_release:
	@mkdir -p target/test
	@$(CC) $(CFLAGS) $(TEST) $^ -o target/test/main -L. -lvec
	@echo -e "   \e[32mFinished\e[0m release (optimized + debugflags)"
	@echo -e "    \e[32mRunning\e[0m target/test/main"
	@target/test/main

clean:
	@rm -Rf target/ *.so
