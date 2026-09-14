#include <stdio.h>
#include <stdlib.h>
#define MAX_SIZE 1000

int arr[MAX_SIZE];
int size = 0;

int arr_value(int index) {
    if (index < 0 || index >= size) {
        printf("Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    return arr[index];
}

void arr_set_value(int index, int value) {
    if (index < 0 || index >= size) {
        printf("Index out of bounds\n");
        exit(EXIT_FAILURE);
    }
    arr[index] = value;
}

void arr_print(void) {
    for (int i = 0; i < size; i++) {
        printf("%d\n", arr[i]);
    }
}

void append_value(int value) {
    if (size >= MAX_SIZE) {
        printf("Array is full\n");
        exit(EXIT_FAILURE);
    }
    arr[size] = value;
    size++;
}

int main(void) {

    // Append some values to the array
    append_value(10);
    append_value(11);
    append_value(12);
    arr_print();

    printf("Value at index 1: %d\n", arr_value(1)); // Should print 11
    arr_set_value(1, 20);
    printf("Value at index 1 after update: %d\n", arr_value(1)); // Should print 20

    printf("Value at index 10: %d\n", arr_value(10)); // Out of bounds: exits with status 1

    return 0;
}
