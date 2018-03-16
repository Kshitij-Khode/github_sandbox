#include<stdio.h>
#include<stdlib.h>

typedef struct node {
    int key;
    struct node *left, *right;
} nodeT;

nodeT* newNode(int key) {
    nodeT* node = (nodeT*)malloc(sizeof(nodeT));
    node->key = key;
    node->left = node->right = NULL;
    return node;
}

nodeT* insert(nodeT* node, int key, int* cnt) {

    if (node == NULL) return newNode(key);
    if (key <= node->key) {
        *cnt = *cnt + 1;
        node->left  = insert(node->left, key, cnt);
    }
    else {
        *cnt = *cnt + 1;
        node->right = insert(node->right, key, cnt);
    }
    return node;
}

void printInorder(nodeT *root) {
    if (root != NULL) {
        printInorder(root->left);
        printf("inorder:%d\n", root->key);
        printInorder(root->right);
    }
}

void printPreorder(nodeT *root) {
    if (root != NULL) {
        printf("preorder:%d\n", root->key);
        printPreorder(root->left);
        printPreorder(root->right);
    }
}

void printPostorder(nodeT *root) {
    if (root != NULL) {
        printPostorder(root->left);
        printPostorder(root->right);
        printf("postorder:%d\n", root->key);
    }
}

void binaryTreeDriver() {
    nodeT *root = NULL;
    int count = 0;

    int input[] = {2, 1, 3};
    int i;
    for (i = 0; i < 3; i++) {
        if (root != NULL) {
            insert(root, input[i], &count);
        }
        else {
            root = newNode(input[i]);
            count++;
        }
        printf("loop:%d\n", count);
    }

    printInorder(root);
    printPreorder(root);
    printPostorder(root);
}

// -----------------------------------------------------------------------------

// O(n^2)
void bubbleSort(int arr[], int n) {
    int i, j;

    for (i = 0; i < n-1; i++) {
        int b = 1;
        for (j = 0; j < n-i-1; j++) {
            if (arr[j] > arr[j+1]) {
                swap(&arr[j], &arr[j+1]);
                b = 0
            }
        }
        if (b) break;
    }
}

// O(n^2)
void bubbleSortR(int arr[], int n) {
    if (n == 1) return;
    for (int i=0; i<n-1; i++) {
        if (arr[i] > arr[i+1]) swap(arr[i], arr[i+1]);
    }
    bubbleSortR(arr, n-1);
}

// O(n^2)
void insertionSort(int arr[], int n) {
    int i, key, j;

    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i-1;

        while (j >= 0 && arr[j] > key) {
            arr[j+1] = arr[j];
            j = j-1;
        }
        arr[j+1] = key;
    }
}

// O(n^2)
void selectionSort(int arr[], int n) {
    int i, j, min_idx, temp;

    for (i = 0; i < n-1; i++) {
        min_idx = i;
        for (j = i+1; j < n; j++) {
            if (arr[j] < arr[min_idx]) min_idx = j;
        }
        temp = arr[min_idx];
        arr[min_idx] = arr[i];
        arr[i] = temp;
    }
}

void merge(int arr[], int l, int m, int r) {
    int i, j, k;
    int n1 = m - l + 1;
    int n2 =  r - m;

    int L[n1], R[n2];

    for (i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (j = 0; j < n2; j++)
        R[j] = arr[m + 1+ j];

    i = 0;
    j = 0;
    k = l;
    while (i < n1 && j < n2) {
        if (L[i] <= R[j]) {
            arr[k] = L[i];
            i++;
        }
        else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

void mergeSort(int arr[], int l, int r) {
    if (l < r) {
        int m = l+(r-l)/2;

        mergeSort(arr, l, m);
        mergeSort(arr, m+1, r);

        merge(arr, l, m, r);
    }
}

void sortDriver() {
    int n = 4;
    int arr[n] = {5, 2, 3, 1};

    insertionSort(arr, n);
    bubbleSort(arr, n);
    bubbleSortR(arr, n);
    selectionSort(arr, n);
}

int main() {
    // binaryTreeDriver();
    // sortDriver();
    return 0;
}