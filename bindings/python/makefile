CC = g++
FLAGS =  -O0 -g3 -c -fmessage-length=0 -MMD -MP 
INCLUDES = -I/usr/include/python2.7 -I. -I../..

all:
	make clean
	swig -python -dirprot -c++ core.i
	mv core.py elements/
	swig -python -dirprot -c++ utils.i
	mv utils.py elements/
	swig -python -dirprot -c++ pal.i
	mv pal.py elements/
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp python.cpp pal_wrap.cxx
	$(CC) -shared *.o -o elements/_pal.so
	$(CC) $(FLAGS) $(INCLUDES) ../../utils/*.cpp python.cpp utils_wrap.cxx
	$(CC) -shared *.o -o elements/_utils.so
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp  core_wrap.cxx python.cpp ../../core/*.cpp
	$(CC) -shared *.o -o elements/_core.so
	rm -f *.so *.d *.o *_wrap.* *.pyc
	
clean:
	rm -f elements/core.py*
	rm -f elements/pal.py*
	rm -f elements/utils.py*
	rm -f elements/_*.so
	rm -f *.cxx
	rm -f *.so *.d *.o *_wrap.* *.pyc