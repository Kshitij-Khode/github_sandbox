// #include <iostream>

// class Base {
// public:
//     explicit Base(int i) {
//         std::cout << "Base" << i << std::endl;
//     };
// };

// class Allowed {
// public:
//     Allowed(int i) {
//         std::cout << "Allowed" << i << std::endl;
//     };
// };

// int main(int argc, char** argv) {
//     Base b = 9; // Error: conversion from 'int' to 'Base' requested
//     Allowed a = 9; // OK: Allowed(int) is not explicit
//     return 0;
// }