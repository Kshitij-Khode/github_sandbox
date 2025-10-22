#include <iostream>

class Descriptor {
public:
    std::string header = "";
    std::string body = "";
    std::string footer = "";

    void print() {
        std::cout << header << body << footer << std::endl;
    }
};

class DescriptorBuilder {
public:
    virtual DescriptorBuilder& Header(std::string s) = 0;
    virtual DescriptorBuilder& Body(std::string s) = 0;
    virtual DescriptorBuilder& Footer(std::string s) = 0;
    virtual std::unique_ptr<Descriptor> Build() = 0;
};

class TensorDescriptorBuilder: public DescriptorBuilder {
private:
    std::unique_ptr<Descriptor> _desc;
public:
    TensorDescriptorBuilder() {
        _desc = std::make_unique<Descriptor>();
    }
    DescriptorBuilder& Header(std::string s) override {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->header = "TensorHeader: " + s + ";";
        return *this;
    }
    DescriptorBuilder& Body(std::string s) override {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->body = "TensorBody: " + s + ";";
        return *this;
    }
    DescriptorBuilder& Footer(std::string s) override {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->footer = "TensorFooter: " + s + ";";
        return *this;
    }
    std::unique_ptr<Descriptor> Build() override {
        return std::move(_desc);
    }
};

class OperatorDescriptorBuilder: public DescriptorBuilder {
private:
    std::unique_ptr<Descriptor> _desc;
public:
    OperatorDescriptorBuilder() {
        _desc = std::make_unique<Descriptor>();
    }
    DescriptorBuilder& Header(std::string s) override {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->header = "OperatorHeader: " + s + ";";
        return *this;
    }
    DescriptorBuilder& Body(std::string s) override {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->body = "OperatorBody: " + s + ";";
        return *this;
    }
    DescriptorBuilder& Footer(std::string s) override {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->footer = "OperatorFooter: " + s + ";";
        return *this;
    }
    std::unique_ptr<Descriptor> Build() override {
        return std::move(_desc);
    }
};

int main() {
    OperatorDescriptorBuilder odb = OperatorDescriptorBuilder();
    std::unique_ptr<Descriptor> d0 = odb.Header("Ham").Body("Salmon").Footer("Jerky").Build();
    d0->print();
    std::unique_ptr<Descriptor> d1 = odb.Header("Sand").Body("Which").Footer("Sandwhich").Build();
    d1->print();
    return 0;
}
