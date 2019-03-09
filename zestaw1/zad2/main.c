#include "../zad1/lib.c"

int main(void) {

    Array* array = create(10);
    setDir(array, ".");
    setTargetFile(array, "main.c");
    find(array);
    read(array);
    printf(array->blockArr[0]);
    return 0;
}