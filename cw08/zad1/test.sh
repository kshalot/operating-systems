#!/bin/bash

test_thread() {
    printf "$1 threads\n"
    ./main "$1" "$2" assets/imgs/baboon.ascii.pgm assets/filters/33 test.pgm
}

test_partitioning() {
    test_thread 1 "$1"
    printf "\n"
    test_thread 2 "$1"
    printf "\n"
    test_thread 4 "$1"
    printf "\n"
    test_thread 8 "$1"
}

printf "Interleaved:\n"
test_partitioning "interleaved"
printf "\nBlock:\n"
test_partitioning "block"
