#!/bin/bash

# Check if clang is supported, and abort if it is not.
if ! [ -x "$(command -v clang++)" ]
then
echo "Clang++ is not installed on your operating system. Please install from the LLVM projects' website (https://releases.llvm.org/download.html) in order to be able to continue with compilation. Build cannot proceed, exiting..." && exit
fi

echo "Building executable..."

if ! [ -x "$(command -v make)" ]
then 
    echo "Make is not supported on your machine. Please install a valid version of Make in order to compile properly." && exit
fi

make -f Makefile.linux.mak all IS_DEBUG=yes

ERRORLEVEL=$?
if [ $ERRORLEVEL -ne 0 ]
then
echo "ERROR: "$ERRORLEVEL && exit
fi 

echo "All binaries built successfully."