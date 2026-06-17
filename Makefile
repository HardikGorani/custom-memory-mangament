CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

TARGET = allocator
SRCS   = allocator.cpp main.cpp
OBJS   = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp allocator.h
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
