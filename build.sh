#!/bin/bash

gcc -fsanitize=address -g ./src/main.c ./src/builtins.c -o dish -lreadline
