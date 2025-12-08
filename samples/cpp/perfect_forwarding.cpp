#include <iostream>
#include <string>
#include <utility>

// ----------------------------------------------------------------
// Inside API library implementation file

void process(const std::string& str) {
    std::cout << "Process called with lvalue: " << str << std::endl;
}

void process(std::string&& str) {
    std::cout << "Process called with rvalue: " << str << std::endl;
}

void process(const int& num) {
    std::cout << "Process called with lvalue: " << num << std::endl;
}

void process(int&& num) {
    std::cout << "Process called with rvalue: " << num << std::endl;
}

struct Param {
    std::string name;

    Param(const std::string& n) : name(n) {
        std::cout << "Param Copy Constructor: " << name << std::endl;
    }

    Param(std::string&& n) : name(std::move(n)) {
        std::cout << "Param Move Constructor: " << name << std::endl;
    }
};

// Implementation of API function done inside private lib repo calling internal non-ambigious lvalue and rvalue overloads available in private repo headers.
template <typename T>
void function_wrapper(T&& arg) {
    // std::forward preserves the original value category
    process(std::forward<T>(arg)); 
}

// Implementation of public API that doesn't need various version based on lvalue/rvalue.
template<typename T, typename S>
T constructor_wrapper(S&& arg) {
    return T(std::forward<S>(arg));
}

// Implementation of factory function which does various things based on args.
template <typename... Args>
void function_wrapper_multi_args(Args&&... args) {
    (process(std::forward<Args>(args)), ...);
}

// // Inside API library implementation file
// // ----------------------------------------------------------------


// ----------------------------------------------------------------
// Public facing headers

// Single public API function with perfect forwarding provided in public header. No need to expose 2 different overloads.
template <typename T>
void function_wrapper(T&& arg);

template<typename T, typename S>
T constructor_wrapper(S&& arg);

template <typename... Args>
void function_wrapper_multi_args(Args&&... args);

int main() {
    std::string s = "really long string";
    function_wrapper(s);
    function_wrapper("really really long string. really.");
    std::string paramName = "TestParamLValue";
    Param p0 = constructor_wrapper<Param>(paramName);
    Param p1 = constructor_wrapper<Param>("TestParamRValue");
    int i = 1;
    function_wrapper_multi_args(s, "test", i, 42);
}

// Public facing headers
// ----------------------------------------------------------------
