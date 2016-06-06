CC = g++
FLAGS =  -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP 
INCLUDES = -I/usr/include/python2.7 -I. -I../..

all:
	make clean
	swig -python -c++ core.i
	mv core.py elements/
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp ../../utils/*.cpp core_wrap.cxx python.cpp ../../core/*.cpp
	$(CC) -shared *.o -o elements/_core.so
	rm -f core_wrap.cxx
	rm -f *.so *.d *.o *_wrap.* *.pyc
	
clean:
	rm -f elements/core.py*
	rm -f elements/_core.so
	rm -f core_wrap.cxx
	rm -f *.so *.d *.o *_wrap.* *.pyc