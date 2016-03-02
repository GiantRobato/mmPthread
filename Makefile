CXX = g++
CXXFLAGS = -lpthread -O0
OBJS= obj/main.o

all: $(OBJS) main 

main: obj/main.o
	$(CXX) -o main obj/main.o $(CXXFLAGS)
	mkdir bin
	mv main bin/.

obj/main.o: src/main.c
	$(CXX) $(CXXFLAGS) -c src/main.c -o obj/main.o

clean:
	rm -rf obj/*.o
	rm -rf bin/main
	rm -rf bin/

run:
	clear
	./bin/main
