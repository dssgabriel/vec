CC = gcc
CFLAGS = -Wall -Wextra -g
OFLAGS = -O3
SRC = src
LIB = target/build/libvec.o
TEST = test/test.c

libvec.so: $(SRC)/vec.c $(SRC)/vec.h
	@printf "\e[32m  Compiling\e[0m libvec v0.1.0\n"
	@$(CC) $(CFLAGS) $(OFLAGS) -fPIC -shared -o $@ src/vec.c -lc
	@printf "   \e[32mFinished\e[0m release (optimized + debugflags)\n"

install: libvec.so
	@printf "\e[32m Installing\e[0m libvec v0.1.0\n"
	@cp libvec.so /usr/lib/

uninstall:
	@printf "\e[32mUninstalling\e[0m libvec v0.1.0\n"
	@rm -f /usr/lib/libvec.so

$(LIB): $(SRC)/vec.c $(SRC)/vec.h
	@mkdir -p target/build
	@printf "\e[32m  Compiling\e[0m vec v0.1.0\n"
	@$(CC) $(CFLAGS) -c $(SRC)/vec.c -o $@

test: $(LIB)
	@mkdir -p target/test
	@printf "\e[32m  Compiling\e[0m test\n"
	@$(CC) $(CFLAGS) $^ $(TEST) -o target/test/main
	@printf "   \e[32mFinished\e[0m debug (unoptimized + debugflags)\n"
	@printf "    \e[32mRunning\e[0m target/test/main\n"
	@target/test/main

test_release: libvec.so $(TEST)
	@mkdir -p target/test
	@printf "\e[32m  Compiling\e[0m test\n"
	@$(CC) $(CFLAGS) -L. $^ -o target/test/main -lvec
	@printf "   \e[32mFinished\e[0m release (optimized + debugflags)\n"
	@printf "    \e[32mRunning\e[0m target/test/main\n"
	@LD_LIBRARY_PATH=. target/test/main

clean:
	@rm -Rf target/ *.so
