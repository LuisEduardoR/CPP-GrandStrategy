#!/bin/sh

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'

cd src
rm *.o *.hpp.gch

if !( g++ main.cpp main.hpp map_data.cpp map_data.hpp -c); then 
    echo "${RED}Could not proceed with compilation due to previous errors!"; 
else
    if !(g++ main.o map_data.o -o cppgrandstrategy -lsfml-graphics -lsfml-window -lsfml-system ); then
        echo "${RED}Final compilation step failed!";
    else
        echo "${GREEN}Compilation was a success!";
        if !( mv cppgrandstrategy ../build/cppgrandstrategy ); then
            echo "${YELLOW}Failed to move result to build directory!";
        fi
    fi
fi
