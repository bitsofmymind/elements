CC = g++
FLAGS =  -O0 -g3 -c -fmessage-length=0 -MMD -MP 
INCLUDES = -I/usr/include/python2.7 -I. -I../..

all:
	make clean
	swig -python -c++ core.i
	mv core.py elements/
	swig -python -c++ utils.i
	mv utils.py elements/
	$(CC) $(FLAGS) $(INCLUDES) ../../utils/*.cpp utils_wrap.cxx
	$(CC) -shared *.o -o elements/_utils.so
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp  core_wrap.cxx python.cpp ../../core/*.cpp
	$(CC) -shared *.o -o elements/_core.so
	rm -f *.so *.d *.o *_wrap.* *.pyc
	
clean:
	rm -f elements/core.py*
	rm -f elements/_core.so
	rm -f elements/utils.py*
	rm -f elements/_utils.so
	rm -f *.cxx
	rm -f *.so *.d *.o *_wrap.* *.pyc