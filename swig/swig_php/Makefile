flwi: 
	swig -php7 flwg_php.i
	gcc `php-config --includes` -fPIC -c flwg_php_wrap.c php_wrapper.c ../../src/flwg/src/*.c ../../src/flwp/src/*.c ../../src/algs/src/*.c ../../src/structs/src/*.c ../../src/FLWP-2.c -lm
	gcc -shared *.o -o _flwg_php.so
	rm *.o

flwg: 
	gcc -o php_wrapper_test php_wrapper.c ../../src/flwg/src/*.c ../../src/flwp/src/*.c ../../src/algs/src/*.c ../../src/structs/src/*.c ../../src/FLWP-2.c -lm

clean:
	rm -f *.o *.so
