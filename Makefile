main:
	make flwo

flwo: ./src/FLWP-2.c ./src/algs/src/*.c ./src/flwp/src/*.c ./src/flwg/src/*.c ./src/structs/src/*.c ./src/structs/includes/*.h ./src/flwg/includes/*.h ./src/flwg/includes/*.h ./src/algs/includes/*.h ./src/*.h
	gcc -g -o flwo ./src/*.c ./src/algs/src/*.c ./src/flwp/src/*.c ./src/flwg/src/*.c ./src/structs/src/*.c -lm


clean:
	rm *.o
