# Compiler and flags (missing -Werror)
CXX = g++
CXXFLAGS = -O3 -std=c++20 -Wall -pedantic -I ./include  -I ./external
LDFLAGS = -lglfw -lGL -lassimp

# Source files
SRC_DIR = ./src
EXT_SRC_DIR = ./external
SRCS = $(wildcard $(SRC_DIR)/*.cpp) $(EXT_SRC_DIR)/glad/glad.c

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output executable
TARGET = main.out

# Default target
all: $(TARGET)

# Link the object files to create the executable
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET)  $(LDFLAGS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

# Run the executable
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run