#include <stdio.h>

int main() {
    int arr[] = {10,11,12};
    int *ptr = arr; // A pointer variable, with the name ptr, that stores the address of arr[0]
    int *ptr2 = &arr[0]; // A pointer variable, with the name ptr, that stores the address of arr[0]

    printf("%d\n", *ptr); // Output the value of arr[0] (10)
    printf("%p\n", ptr); // Output the memory address of arr[0]

    const size_t size = sizeof(arr) / sizeof(arr[0]); // Calculate the size of the array
    for(int i = 0; i < size; i++) {
        printf("%d\n", arr[i]); // Output the value of arr[i] (10, 11, 12)
    }

    for(int i = 0; i < size; i++) {
        printf("%p\n", (ptr + i)); // Output the memory address of arr[i]
        printf("%d\n", *(ptr + i)); // Output the value of arr[i] (10, 11, 12)
    }

    printf("%p\n", (ptr + 4));
    printf("%d\n", *(ptr + 4)); // Output the value of arr[4] (undefined behavior)

    printf("%p\n", (ptr + 10000));
    printf("%d\n", *(ptr + 10000)); // Output the value of arr[4] (undefined behavior)

    return 0;
}
