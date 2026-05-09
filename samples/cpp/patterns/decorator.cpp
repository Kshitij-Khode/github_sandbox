#include <iostream>

class AbstractBase {
public:
    virtual ~AbstractBase() {};
    virtual void action() = 0;
};

class ConcreteBase: public AbstractBase {
public:
    void action() override {
        std::cout << "ConcreteBase" << std::endl;
    }
};

class Decorator: public AbstractBase {
public:
    Decorator(ConcreteBase& cb): _cb(cb) {};
    void action() override {
        _cb.action();
    }

private:
    ConcreteBase& _cb;
};

class Decorator0: public Decorator {
public:

    Decorator0(ConcreteBase& cb): Decorator(cb) {};

    void action() override {
        std::cout << "Decorate0" << std::endl;
        Decorator::action();
    }
};

class Decorator1: public Decorator {
public:

    Decorator1(ConcreteBase& cb): Decorator(cb) {};

    void action() override {
        std::cout << "Decorate1" << std::endl;
        Decorator::action();
    }
};

int main() {
    ConcreteBase cb;
    Decorator0 d0(cb);
    d0.action();
    Decorator1 d1(cb);
    d1.action();
    return 0;
}