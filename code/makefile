main:
	make flwo

flwo: FLWP-2.c ./algs/src/*.c ./flwp/src/*.c ./flwg/src/*.c ./structs/src/*.c ./structs/includes/*.h ./flwg/includes/*.h ./flwg/includes/*.h ./algs/includes/*.h
	gcc -g -o flwo FLWP-2.c ./algs/src/*.c ./flwp/src/*.c ./flwg/src/*.c ./structs/src/*.c -lm


clean:
	rm *.o
