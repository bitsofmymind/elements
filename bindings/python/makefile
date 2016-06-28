SWIG_FLAGS = -python -dirprot -c++
CC = g++
FLAGS = -O0 -g3 -c -fmessage-length=0 -MMD -MP 
INCLUDES = -I/usr/include/python2.7 -I. -I../..

all:
	make clean
	swig $(SWIG_FLAGS) core.i
	mv core.py elements/
	swig $(SWIG_FLAGS) utils.i
	mv utils.py elements/
	swig $(SWIG_FLAGS) pal.i
	mv pal.py elements/
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp python.cpp pal_wrap.cxx
	$(CC) -shared *.o -o elements/_pal.so
	$(CC) $(FLAGS) $(INCLUDES) ../../utils/*.cpp python.cpp utils_wrap.cxx
	$(CC) -shared $(PWD)/elements/_pal.so *.o -o elements/_utils.so
	$(CC) $(FLAGS) $(INCLUDES) ../../pal/*.cpp  core_wrap.cxx python.cpp ../../core/*.cpp
	$(CC) -shared $(PWD)/elements/_pal.so $(PWD)/elements/_utils.so *.o -o elements/_core.so
	rm -f *.so *.d *.o *_wrap.* *.pyc
	
clean:
	rm -f elements/core.py*
	rm -f elements/pal.py*
	rm -f elements/utils.py*
	rm -f elements/_*.so
	rm -f *.cxx
	rm -f *.so *.d *.o *_wrap.* *.pyc
