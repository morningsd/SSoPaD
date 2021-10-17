CXX = clang++
LD = clang++

CXXFLAGS = -Wall -Wextra -c

RM = rm -f

demo: main.o
	$(CXX) $? -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $? -o $@

clean:
	$(RM) *.o demo
