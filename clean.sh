#!/bin/bash

if ! [ -x "$(command -v make)" ]
then
echo "Make is not installed. Please install make before doing any compiler actions."
exit
fi

make -f Makefile.linux.mak clean

