#!/usr/bin/env python3

import sys
import random


def main():
    if len(sys.argv) == 1:
        print('Invalid number of arguments. Pass filter size.')
        return -1

    size = int(sys.argv[1])
    f = [random.uniform(0, 1) for _ in range(size * size)]
    rv = map(lambda x: x/sum(f), f)
    print(size)
    print(*rv)


if __name__ == '__main__':
    main()
