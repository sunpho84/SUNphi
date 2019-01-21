#!/bin/bash

if [ -f "$1/lib/gitInfo.hpp" ]
then
    cat "$1/lib/gitInfo.hpp"
else
    
    echo '#ifndef _GIT_INFO_HPP'
    echo '#define _GIT_INFO_HPP'
    echo
    
    echo -n ' #define GIT_HASH "'
    git rev-parse HEAD|tr -d "\n"
    echo '"'
    
    echo -n ' #define GIT_TIME "'
    git log -1 --pretty=%ad|tr -d "\n"
    echo '"'
    
    echo -n ' #define GIT_COMMITTER "'
    git log -1 --pretty=%cn|tr -d "\n"
    echo '"'
    
    echo -n ' #define GIT_LOG "'
    git log -1 --pretty=%B|tr -d "\n"|sed 's|\\|\\\\|g'
    echo '"'
    
    echo
    echo '#endif'

fi
