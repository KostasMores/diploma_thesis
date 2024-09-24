# Name of the executable
TARGET = rand_benchmark
TARGET2 = serial
TARGET3 = deter

# Compiler
CC = gcc

# Default target (builds the program)
all: $(TARGET) $(TARGET2) $(TARGET3)

# Build the executable
$(TARGET): micro.c
	$(CC) -o $(TARGET) micro.c

$(TARGET2): serial.c
	$(CC) -o $(TARGET2) serial.c

$(TARGET3): deter.c
	$(CC) -o $(TARGET3) deter.c

# Clean target to remove compiled files
clean:
	rm -f $(TARGET)
	rm -f $(TARGET2)
	rm -f $(TARGET3)

# Phony targets to avoid conflicts with files of the same name
.PHONY: all clean

