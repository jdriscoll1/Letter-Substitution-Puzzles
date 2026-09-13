main:
	make flwo

# -flto lets the hot one-line accessors (checkIfUsed_WordSet, getConnections, ...)
# inline across files; the game searches call them millions of times per move.
flwo: ./src/*.c ./src/*/src/*.c ./src/*/includes/*.h
	gcc -O3 -flto -o flwo ./src/*.c  ./src/*/src/*.c -lm

# Unit tests. src/main.c is left out because the suite brings its own main().
# Run from the repository root: the tests load docs/ by relative path.
flwo_tests: ./tests/*.c ./tests/*.h ./src/*/src/*.c ./src/*/includes/*.h
	gcc -g -O0 -o flwo_tests ./tests/*.c ./src/*/src/*.c -lm

test: flwo_tests
	./flwo_tests

# Same suite under valgrind, which is what catches leaks
test-memcheck: flwo_tests
	valgrind --leak-check=full --show-leak-kinds=definite,indirect --error-exitcode=1 ./flwo_tests

clean:
	rm -f flwo flwo_tests

.PHONY: main test test-memcheck clean
