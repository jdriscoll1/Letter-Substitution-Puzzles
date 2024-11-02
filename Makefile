main:
	make flwo

flwo: ./src/*.c ./src/*.h ./src/*/src/*.c ./src/*/includes/*.h ./src/flwg/src/*.c ./src/flwc/src/FLWC.cpp
	gcc  -g -c ./src/*.c ./src/*/src/*.c -lm
	g++ -g -c ./src/flwc/src/FLWC.cpp -lm
	gcc ./*.o -o flwo -lm
	clean

clean:
	rm *.o
