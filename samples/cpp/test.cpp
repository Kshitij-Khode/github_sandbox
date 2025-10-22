// #include <iostream>

// struct Node {
//     int i = 9;
//     Node* next;
// };

// void assignNode(Node** n) {
//     *n = new Node();
// }

// int main(int argc, char** argv) {
//     Node* head;
//     assignNode(&head);
//     std::cout << head->i << std::endl;
//     assignNode(&(head->next));
//     std::cout << head->next->i << std::endl;
//     return 0;
// }


// #include <iostream>

// int main(int argc, char** argv) {
//     int a = 5;
//     int& b = a;
//     b = 10;
//     std::cout << a << std::endl;
//     return 0;
// }


// #include <iostream>

// class Base {
// public:
//     int i = 9;
//     Base() {}
// };

// int main(int argc, char** argv) {
//     Base b0;
//     Base b1 = b0;
//     Base& b2 = b0;
//     b1.i = 10;
//     b2.i = 11;
//     std::cout << b0.i << std::endl;
//     std::cout << b1.i << std::endl;
//     std::cout << b2.i << std::endl;
//     return 0;
// }


// #include <iostream>

// constexpr int sum_expr(int a, int b) {
//     return a + b;
// }

// consteval int sum_eval(int a, int b) {
//     return a + b;
// }

// int main(int argc, char** argv) {
//     int i = 4;
//     constexpr int j = 5;
//     std::cout << sum_expr(i, j) << std::endl;
//     std::cout << sum_eval(j, j) << std::endl;
//     return 0;
// }


// #include <iostream>

// int main(int argc, char** argv) {
//     const int& p = 5;
//     const_cast<int&>(p) = 10;
//     std::cout << p << std::endl;
//     return 0;
// }


// #include <iostream>

// class Base {
// public:
//     virtual void print() {};
// };

// class Derived0: public Base {
// public:
//     void print() override {
//         std::cout << "Derived0" << std::endl;
//     }
//     void shout() {
//         std::cout << "Shout0" << std::endl;
//     }
// };

// class Derived1: public Base {
// public:
//     void print() override {
//         std::cout << "Derived1" << std::endl;
//     }
//     void talk() {
//         std::cout << "Talk1" << std::endl;
//     }
// };

// class DDerived: public Derived0, public Derived1 {
// public:
//     void print() override {
//         std::cout << "DDerived" << std::endl;
//     }
//     void Greet() {
//         std::cout << "Greet" << std::endl;
//     }
// };

// int main(int argc, char** argv) {
//     DDerived* dd = new DDerived();
//     Derived0* d0 = dynamic_cast<Derived0*>(dd);
//     Derived1* d1 = dynamic_cast<Derived1*>(dd);
//     d0->print(); //Will keep printing DDerived because of virtual inheritance
//     d0->shout();
//     d1->print(); //Will keep printing DDerived because of virtual inheritance
//     d1->talk();
//     DDerived* ddb = dynamic_cast<DDerived*>(d0);
//     ddb->Greet();
//     return 0;
// }


// #include <iostream>

// int main(int argc, char** argv) {
//     std::string i = "";
//     int j = 9;
//     std::cout << std::is_same_v<decltype(i), decltype(j)> << std::endl;
//     return 0;
// }


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


// #include <iostream>

// class Base {
// public:
//     friend class Friend;
// private:
//     int i = 9;
// };

// class Friend {
// public:
//     void modify(Base& b) {
//         b.i = 10;
//         std::cout << b.i << std::endl;
//     }
// };

// int main() {
//     Friend f;
//     Base b;
//     f.modify(b);
//     return 0;
// };


#include <iostream>

class Base {
public:
    int i = 0;
    mutable int j = 0;
};


int main() {
    const Base b;
    b.j = 1;
    std::cout << b.j << std::endl;
    return 0;
};