#include<stdio.h>
#include<string.h>

void memCpy(void *dest, void *src, size_t n) {
   char* csrc = (char*)src;
   char* cdest = (char*)dest;

   for (int i=0; i<n; i++) cdest[i] = csrc[i];
}

void memCpyDriver() {
    char csrc[] = "Test123";
    char cdest[100];

    memCpy(cdest, csrc, strlen(csrc)+1);
    printf("Copied string is %s", cdest);
}

int gcd(int a, int b) {
    if (a == 0 || b == 0)
       return 0;

    if (a == b)
        return a;

    if (a > b)
        return gcd(a-b, b);
    return gcd(a, b-a);
}

void gcdDriver() {
    int a = 98, b = 56;
    printf("GCD of %d and %d is %d ", a, b, gcd(a, b));
}

int main() {
    // memCpyDriver();
    // gcdDriver();
}