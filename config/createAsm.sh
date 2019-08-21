#!/bin/bash

# run the compilation creating an additional file, ending with ".s",
# for each ".o" created, containing the assmbler translation

# set initial position to zero
n=0

# loop on all arguments
for i in $*
do
    
    # if "-o" is found, mark it
    if [ "$i" == "-o" ]
    then
	pos=$(($n+1))
    fi
    
    # increase the position
    n=$(($n+1))
done

# take note of all arguments
args=($*)

# take target
target=${args[$pos]}

# try to replace the ".o"
asm=$(echo $target|sed 's|.o$|.s|')

# if the replace went fine, run the assembler generation
# this avoid linking and other stages
if [ "$asm" != "$target" ]
then
    args[$pos]="${asm}"
    
    ${args[@]} -S    
fi

# run ordinary compilation
($*)
