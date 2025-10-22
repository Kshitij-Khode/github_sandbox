#include <iostream>
#include <unordered_map>

class Prototype {
public:
    enum Type {
        Zero,
        One
    };
    virtual void doMajor0() = 0;
    virtual void doMajor1() = 0;
    virtual void doMajor2() = 0;
    virtual Prototype* clone() = 0;
};

class Prototype0: public Prototype {
public:
    Prototype0() {
        doMajor0();
        doMajor1();
        doMajor2();
    }
    void doMajor0() override {
        std::cout << "doMajor00" << std::endl;
    }
    void doMajor1() override {
        std::cout << "doMajor10" << std::endl;
    }
    void doMajor2() override {
        std::cout << "doMajor20" << std::endl;
    }
    Prototype* clone() {
        return new Prototype0(*this);
    }
};

class Prototype1: public Prototype {
public:
    Prototype1() {
        doMajor0();
        doMajor1();
        doMajor2();
    }
    void doMajor0() override {
        std::cout << "doMajor01" << std::endl;
    }
    void doMajor1() override {
        std::cout << "doMajor11" << std::endl;
    }
    void doMajor2() override {
        std::cout << "doMajor21" << std::endl;
    }
    Prototype* clone() {
        return new Prototype1(*this);
    }
};

class PrototypeFactory {
private:
    std::unordered_map<Prototype::Type, Prototype*> store;

public:
    PrototypeFactory() {
        store[Prototype::Type::Zero] = new Prototype0();
        store[Prototype::Type::One] = new Prototype1();
    }

    Prototype* Generate(Prototype::Type t) {
        switch(t) {
        case Prototype::Type::Zero:
            return store[Prototype::Type::Zero]->clone();
        case Prototype::Type::One:
            return store[Prototype::Type::One]->clone();
        default:
            std::cout << "Illegal type of prototype provided";
            return nullptr;
        }
    }
};

int main() {
    PrototypeFactory pf;
    Prototype* p00 = pf.Generate(Prototype::Type::Zero);
    Prototype* p01 = pf.Generate(Prototype::Type::Zero); // Same type as p00 but addrerss is different cause clone.
    Prototype* p1 = pf.Generate(Prototype::Type::One);
    std::cout << p00 << std::endl;
    std::cout << p01 << std::endl;
    std::cout << p1 << std::endl;
    return 0;
};