#include <iostream>

class AbstractProduct {
public:
    virtual ~AbstractProduct() {};
    virtual void Operation() = 0;
};

class ProductA: public AbstractProduct {
public:
    void Operation() override {
        std::cout << "ProductA Operation" << std::endl;
    }
};

class ProductB: public AbstractProduct {
public:
    void Operation() override {
        std::cout << "ProductB Operation" << std::endl;
    }
};

class AbstractFactory {
public:
    virtual ~AbstractFactory() {};
    virtual std::unique_ptr<AbstractProduct> CreateProduct() = 0;
};

class FactoryA: public AbstractFactory {
public:
    std::unique_ptr<AbstractProduct> CreateProduct() override {
        return std::make_unique<ProductA>();
    }
};

class FactoryB: public AbstractFactory {
public:
    std::unique_ptr<AbstractProduct> CreateProduct() override {
        return std::make_unique<ProductB>();
    }
};

int main() {
    std::unique_ptr<AbstractFactory> factoryA = std::make_unique<FactoryA>();
    std::unique_ptr<AbstractFactory> factoryB = std::make_unique<FactoryB>();
    std::unique_ptr<AbstractProduct> productA = factoryA->CreateProduct();
    std::unique_ptr<AbstractProduct> productB = factoryB->CreateProduct();
    productA->Operation();
    productB->Operation();
    return 0;
};
