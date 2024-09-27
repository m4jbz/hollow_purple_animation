#!/bin/sh

set -xe

clear
gcc -Wall -Wextra -o main main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
# gcc -Wall -Wextra -o test test.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
