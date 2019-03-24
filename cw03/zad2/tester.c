#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv) {

    if(argc < 5) {
        fprintf(stderr, "Too few arguments!\nUsage: monitor <file> <pmin> <pmax> <bytes>");
        exit(1);
    }