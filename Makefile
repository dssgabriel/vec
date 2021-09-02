CC = gcc
CFLAGS = -Wall -Wextra -g
OFLAGS = -O3
SRC = src
LIB = target/build/libvec.o
TEST = test/test.c

libvec.so: $(SRC)/vec.c $(SRC)/vec.h
	@echo -e "\e[32m  Compiling\e[0m libvec v0.1.0"
	@$(CC) $(CFLAGS) $(OFLAGS) -fPIC -shared -o $@ src/vec.c -lc
	@echo -e "   \e[32mFinished\e[0m release (optimized + debugflags)"

$(LIB): $(SRC)/vec.c $(SRC)/vec.h
	@mkdir -p target/build
	@echo -e "\e[32m  Compiling\e[0m vec v0.1.0"
	@$(CC) $(CFLAGS) -c $(SRC)/vec.c -o $@

test: $(LIB)
	@mkdir -p target/test
	@echo -e "\e[32m  Compiling\e[0m test"
	@$(CC) $(CFLAGS) $^ $(TEST) -o target/test/main
	@echo -e "   \e[32mFinished\e[0m debug (unoptimized + debugflags)"
	@echo -e "    \e[32mRunning\e[0m target/test/main"
	@target/test/main

test_release:
	@mkdir -p target/test
	@echo -e "\e[32m  Compiling\e[0m test"
	@$(CC) $(CFLAGS) $(TEST) $^ -o target/test/main -L. -lvec
	@echo -e "   \e[32mFinished\e[0m release (optimized + debugflags)"
	@echo -e "    \e[32mRunning\e[0m target/test/main"
	@target/test/main

clean:
	@rm -Rf target/ *.so
