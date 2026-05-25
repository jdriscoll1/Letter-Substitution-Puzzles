main:
	make flwo

flwo: ./src/*.c ./src/*/src/*.c ./src/*/includes/*.h 
	gcc -O3 -o flwo ./src/*.c  ./src/*/src/*.c -lm
