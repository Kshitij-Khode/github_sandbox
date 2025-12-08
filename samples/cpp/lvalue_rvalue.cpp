#include <iostream>
#include <string>

void print(std::string&& directRef) {
    std::cout << directRef << std::endl;
}

int main() {
    const int i = 10;
    int j = 20;
    int k = i + j; // Here i + j is an rvalue
    int a[10+i]; // Here 100+i is an rvalue
    std::string test0 = "really long string";
    std::string test1 = test0; // This would require us to memcpy test0 to temp rvalue then copy to lvalue test1.
    std::string&& test2 = test0 + test1; // Here temp rvalue created by copy+concat, will simply be referred to by test2 rather than copied into test2.
    std::cout << test0 << std::endl;
    std::cout << test1 << std::endl;
    std::cout << test2 << std::endl;
    std::string test3 = std::move(test0); // Here ownership of test0 will be gone to test3, hence test0 will be empty.
    print(std::move(test0));
    print(std::move(test3)); // test3 will not be copied to be printed, instead a temp rvalue reference will be created to print it out.
    std::cout << test3 << std::endl; // Since only temp rvalue reference created temp3 retains ownership of string.
    std::string&& test4 = std::move(test2); // Here ownership of test2 will be shared with test4, since they're pointing to the same thing, hence test2 will not be empty.
    std::cout << test2 << std::endl;
    std::cout << test4 << std::endl;
    return 0;
}