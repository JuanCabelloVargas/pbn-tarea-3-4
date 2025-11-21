CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -Wundef -Wuninitialized -Winit-self -fPIC
SWIG = swig
PYTHON_INC := $(shell python3-config --includes)
PYTHON_LIB := $(shell python3-config --ldflags)
OBJS = geo.o UTM.o
SWIG_INTERFACE = coordenadas.i
SWIG_WRAPPER = coordenadas_wrap.cxx
SWIG_WRAPPER_OBJ = coordenadas_wrap.o
SWIG_MODULE = _coordenadas.so

all: $(SWIG_MODULE)

$(SWIG_WRAPPER) coordenadas.py: $(SWIG_INTERFACE) geo.h UTM.h
	$(SWIG) -c++ -python $(SWIG_INTERFACE)

$(SWIG_WRAPPER_OBJ): $(SWIG_WRAPPER)
	$(CXX) $(CXXFLAGS) $(PYTHON_INC) -c $(SWIG_WRAPPER)

geo.o: geo.cpp geo.h UTM.h
	$(CXX) $(CXXFLAGS) -c geo.cpp

UTM.o: UTM.cpp UTM.h geo.h
	$(CXX) $(CXXFLAGS) -c UTM.cpp

$(SWIG_MODULE): $(SWIG_WRAPPER_OBJ) $(OBJS)
	$(CXX) -shared $(SWIG_WRAPPER_OBJ) $(OBJS) -o $(SWIG_MODULE) $(PYTHON_LIB)

clean:
	rm -f *.o $(SWIG_WRAPPER) $(SWIG_WRAPPER_OBJ) $(SWIG_MODULE) coordenadas.py
