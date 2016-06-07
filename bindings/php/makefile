CC = g++
FLAGS = -fpic -c
INCLUDES = -I. -I../.. `php-config --includes`

all:
	make clean
	swig -php -dirprot -c++ base.i
	mv base.php elements/
	swig -php -c++ -dirprot -outcurrentdir ../python/utils.i
	mv utils.php elements/
	$(CC) $(FLAGS) $(INCLUDES) ../../utils/*.cpp utils_wrap.cpp
	$(CC) -shared *.o -o elements/_utils.so
	rm utils_wrap.o
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp base_wrap.cpp php.cpp ../../core/*.cpp
	$(CC) -shared *.o -o elements/_base.so
	rm -f php_*.h
	rm -f *.so *.d *.o *_wrap.*
	
clean:
	rm -f elements/*.php
	rm -f elements/*.so
	rm -f *_wrap.cpp
	rm -f php_*.h
	rm -f *.so *.d *.o *_wrap.* 