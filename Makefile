CC = gcc
CFLAGS = -Wall -Wextra -g
BUILD = target/build
TESTS = target/test

$(BUILD)/vec.o: $(BUILD) src/vec.c
	@echo "  Compiling vec v0.1.0"
	@$(CC) $(CFLAGS) -c src/vec.c -o $@

$(TESTS)/test: $(TESTS) $(BUILD)/vec.o
	@$(CC) $(CFLAGS) $(BUILD)/vec.o test/test.c -o target/test/main
	@echo "   Finished debug (unoptimized + debugflags)"

test: $(TESTS)/test
	@echo "    Running target/test/main"
	@target/test/main

$(BUILD):
	@mkdir -p $(BUILD)
	
$(TESTS):
	@mkdir -p target/test

clean:
	@rm -Rf target/
