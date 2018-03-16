using namespace std;
#include<iostream>

class Animal {
public:
    void move(void) {
        int* myPointer = new int;
        cout << "This animal moves in some way" << std::endl;
    }
    virtual void eat(void) {}
};

class Llama : public Animal {
public:
    void eat(void) {
        std::cout << "Llamas eat grass!" << std::endl;
    }
};

void virtualFunctionDriver() {
    Animal* a = new Animal();
    Llama* l = new Llama();
    l->eat();
}

// -----------------------------------------------------------------------------

class Logger {

public:
    static Logger* Instance() {
        if (!m_pInstance) m_pInstance = new Logger;
        return m_pInstance;
    }

    static Logger* Destroy() {
        if (m_pInstance) delete m_pInstance;
    }

    void print() {
        cout << "Printing from within Logger";
    }

private:
    static Logger* m_pInstance;

    Logger() {};
    Logger(Logger const&) {};
    Logger& operator = (Logger const&) {};

    // ~Logger() {delete m_pInstance};
};

Logger* Logger::m_pInstance = NULL;

// -----------------------------------------------------------------------------

class A {
public:
    void Foo() {
        cout << "A";
    }
};

class B: public A {
public:
    virtual void Foo() {
        cout << "B";
    }
};

class C : public B {
public:
    void Foo() {
        cout << "C";
    }
};

void staticCastDriver() {
    C* bar = new C;
    bar->Foo();
    static_cast<B*>(bar)->Foo();
    static_cast<A*>(bar)->Foo();
}

// -----------------------------------------------------------------------------

template <typename T>
T myMax(T x, T y) {
   return (x > y)? x: y;
}

void templateDriver() {
  cout << myMax<int>(3, 7) << endl;
  cout << myMax<double>(3.0, 7.0) << endl;
  cout << myMax<char>('g', 'e') << endl;
}

int main(int argc, char const *argv[]) {
    // virtualFunctionDriver();
    // Logger::Instance()->print();
    // staticCastDriver();
    // templateDriver();
}
