CXX = gcc
CXXFLAGS = -lpthread
OBJS= obj/main.o

all: $(OBJS) main 

main: obj/main.o
	$(CXX) -o main obj/main.o $(CXXFLAGS)
	mv main bin

obj/main.o: src/main.c
	$(CXX) $(CXXFLAGS) -c src/main.c -o obj/main.o

clean:
	rm -rf obj/*.o
	rm -rf bin/main 
