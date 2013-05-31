#! /bin/sh

#Copied from ps2toolchain.
## Check if $PS2DEV is set.
if test ! $PS2DEV; then { echo "ERROR: Set \$PS2DEV before continuing."; exit 1; } fi

 ## Check if $PS2SDK is set.
if test ! $PS2SDK; then { echo "ERROR: Set \$PS2SDK before continuing."; exit 1; } fi

make setup ps2; make

## Install the library.
mkdir -p $PS2SDK/ports/lib
cp objs/libfreetype.a $PS2SDK/ports/lib

## Install the include files.
mkdir -p $PS2SDK/ports/include
cp -Rv include/* $PS2SDK/ports/include

