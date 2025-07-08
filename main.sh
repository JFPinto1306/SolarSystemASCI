#!/bin/bash

# Compile the C source files using make
echo "Attempting to build planets project using Makefile..."
make

# Check if compilation was successful (make's exit status)
if [ $? -eq 0 ]; then
    echo "Build successful. Running planets..."
    # Execute the compiled program
    ./planets
else
    echo "Build failed! Please check for errors above."
    exit 1
fi