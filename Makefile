main:
	make flwo

flwo: ./src/*.c ./src/*.h ./src/*/src/*.c ./src/*/includes/*.h ./src/flwg/src/*.c
	gcc -g -o flwo ./src/*.c ./src/*/src/*.c -lm


clean:
	rm *.o
