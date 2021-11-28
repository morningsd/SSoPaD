CXX = g++
LD = g++

CXXFLAGS = -Wall -Wextra -c

RM = rm -f

demo: main.o
	$(CXX) -pthread $? -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $? -o $@

clean:	
	$(RM) *.o demo .auth
