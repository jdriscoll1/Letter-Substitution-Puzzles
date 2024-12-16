main:
	make flwo

flwo: ./src/*.c ./src/*/src/*.c ./src/*/includes/*.h 
	gcc -g -o flwo ./src/*.c  ./src/*/src/*.c -lm
