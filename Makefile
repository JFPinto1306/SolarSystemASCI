# Define variables for the compiler, flags, and libraries
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 # Common C flags for warnings and C99 standard
LIBS = -lcurl # Link with the curl library

# Define your source files and the corresponding object files
SRCS = src/planets.c src/cJSON.c
OBJS = $(SRCS:.c=.o) # This cleverly converts .c files to .o files

# Define the final executable name
TARGET = planets

# The 'all' target is usually the default, building your main executable
all: $(TARGET)

# Rule to build the final executable from object files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(TARGET)

# Generic rule to compile any .c file into a .o object file
# $< is the first dependency (the .c file)
# $@ is the target name (the .o file)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# A 'phony' target to clean up generated files
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)