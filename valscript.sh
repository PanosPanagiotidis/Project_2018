#!/bin/bash

str="$*"


valgrind --leak-check=full --track-origins=yes --trace-children=yes --show-leak-kinds=all ./$str -v
