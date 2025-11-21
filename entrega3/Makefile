CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -Wundef -Wuninitialized -Winit-self
OBJS = geo.o UTM.o
TARGET = salida

all: $(TARGET)

$(TARGET): main.o $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) main.o $(OBJS) -lm

main.o: main.cpp geo.h UTM.h
	$(CXX) $(CXXFLAGS) -c main.cpp

geo.o: geo.cpp geo.h UTM.h
	$(CXX) $(CXXFLAGS) -c geo.cpp

UTM.o: UTM.cpp UTM.h geo.h
	$(CXX) $(CXXFLAGS) -c UTM.cpp

clean:
	rm -f *.o $(TARGET)
