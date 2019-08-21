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

# mark all arguments
args=($*)

# try to replace the ".o"
echo ${args[$pos]}|sed 's|.o$|.s|'|read asm

# if the replace went fine, run the assembler generation
# this avoid linking and other stages
if [ $PIPESTATUS[1] == 0 ]
then
    args[$pos]=${asm}
    
    ${args[@]} -S    
fi

# run ordinary compilation
($*)
