#include <stdio.h>

int arr[] = {10,11,12};
int arr2[10000];

int main() {
    int N = 3;

    for(int i = 0; i < N; i++) {
        printf("%d\n", arr[i]); // Output the value of arr[i] (10, 11, 12)
    }

    N = 4;
    arr[3] = 14; // This is undefined behavior, as arr has only 3 elements

    for(int i = 0; i < N; i++) {
        printf("%d\n", arr[i]); // Output the value of arr[i] (10, 11, 12)
    }

    for(int i = 0; i < 100000; i++) {
        arr[i] = 10+i; // This is undefined behavior, as arr has only 3 element
        printf("%d\n", arr[i]); // Output the value of arr[i] (10, 11, 12)
    }



    return 0;
}
