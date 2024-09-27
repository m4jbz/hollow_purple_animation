#!/bin/sh

set -xe

clear
gcc -Wall -Wextra -o main main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
