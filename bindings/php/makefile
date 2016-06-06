CC = g++
FLAGS = -fpic -c
INCLUDES = -I. -I../.. `php-config --includes`

all:
	make clean
	swig -php -c++ base.i
	mv base.php elements/
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp ../../utils/*.cpp base_wrap.cpp php.cpp ../../core/*.cpp
	$(CC) -shared *.o -o elements/_base.so
	rm -f base_wrap.cpp
	rm -f *.so *.d *.o *_wrap.*
	
clean:
	rm -f elements/base.php*
	rm -f elements/_base.so
	rm -f base_wrap.cpp
	rm -f php_base.h
	rm -f *.so *.d *.o *_wrap.* 