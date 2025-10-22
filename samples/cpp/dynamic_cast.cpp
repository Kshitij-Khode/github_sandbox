#include <iostream>

class Base {
public:
    virtual void print() {};
};

class Derived0: public Base {
public:
    void print() override {
        std::cout << "Derived0" << std::endl;
    }
    void shout() {
        std::cout << "Shout0" << std::endl;
    }
};

class Derived1: public Base {
public:
    void print() override {
        std::cout << "Derived1" << std::endl;
    }
    void talk() {
        std::cout << "Talk1" << std::endl;
    }
};

class DDerived: public Derived0, public Derived1 {
public:
    void print() override {
        std::cout << "DDerived" << std::endl;
    }
    void Greet() {
        std::cout << "Greet" << std::endl;
    }
};

int main(int argc, char** argv) {
    DDerived* dd = new DDerived();
    Derived0* d0 = dynamic_cast<Derived0*>(dd);
    Derived1* d1 = dynamic_cast<Derived1*>(dd);
    d0->print(); //Will keep printing DDerived because of virtual inheritance
    d0->shout();
    d1->print(); //Will keep printing DDerived because of virtual inheritance
    d1->talk();
    DDerived* ddb = dynamic_cast<DDerived*>(d0);
    ddb->Greet();
    return 0;
}
