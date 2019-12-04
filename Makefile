SOURCES=spline.cpp
OBJECTS=$(SOURCES:.cpp=.o)
CXX=g++



all: spline

spline: spline.o
	$(CXX) -o $@ $^ -lgsl -lgslcblas -lm

%.o: %.cpp
	$(CXX) -fpic -c -Wall -o $@ $<

