# Define the compiler and flags
CC = g++
CFLAGS = -c -std=c++20 -O2 -Wall -Wextra -pedantic
LDFLAGS = -lboost_json -lboost_serialization

# Define the source files and object files
SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

# Define the target executable
TARGET = dns_server

# Default target
all: $(TARGET)

# Rule to compile .cpp files into .o files
%.o: %.cpp
	$(CC) $(CFLAGS) $< -o $@

# Rule to link the object files into the target executable
$(TARGET): $(OBJS)
	$(CC) $^ $(LDFLAGS) -o $@

# Rule to clean the object files and the target executable
clean:
	rm -f $(OBJS) $(TARGET)

# Debug target
debug: CFLAGS += -g -D_DEBUG -O0
debug: all

.PHONY: clean
