#include<stdio.h>
#include<stdlib.h>
#include<string.h>


int** malloc2DArray(int n, int m) {
    int** dArray = (int**)malloc(sizeof(int)*n*m);

    for (int i = 0; i < n; i++) {
        dArray[i] = (int*)malloc(sizeof(int)*m);
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            dArray[i][j] = i;
        }
    }

    return dArray;
}

void malloc2DArrayDriver() {
    int** array = malloc2DArray(3, 4);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d\n", array[i][j]);
        }
    }

}

int main(int argc, char const *argv[]) {
    malloc2DArrayDriver();
    return 0;
}