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

class OperatorDescriptorBuilder {
private:
    std::unique_ptr<Descriptor> _desc;
public:
    OperatorDescriptorBuilder() {
        _desc = std::make_unique<Descriptor>();
    }
    OperatorDescriptorBuilder& Header(std::string s) {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->header = "OperatorHeader: " + s + ";";
        return *this;
    }
    OperatorDescriptorBuilder& Body(std::string s) {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->body = "OperatorBody: " + s + ";";
        return *this;
    }
    OperatorDescriptorBuilder& Footer(std::string s) {
        if (_desc == nullptr) _desc = std::make_unique<Descriptor>();
        _desc->footer = "OperatorFooter: " + s + ";";
        return *this;
    }
    std::unique_ptr<Descriptor> Build() {
        return std::move(_desc);
    }
};

class OperatorDirector {
private:
    OperatorDescriptorBuilder& _builder;

public:
    OperatorDirector(OperatorDescriptorBuilder& builder): _builder(builder) {}
    std::unique_ptr<Descriptor> ConstructDefault() {
        return _builder.Header("Default").Body("Default").Footer("Default").Build();
    }
    std::unique_ptr<Descriptor> ConstructAdd() {
        return _builder.Header("Add").Body("Add").Footer("Add").Build();
    }
};

int main() {
    OperatorDescriptorBuilder odb = OperatorDescriptorBuilder();
    OperatorDirector director = OperatorDirector(odb);
    std::unique_ptr<Descriptor> d0 = director.ConstructDefault();
    d0->print();
    std::unique_ptr<Descriptor> d1 = director.ConstructAdd();
    d1->print();
    return 0;
}
