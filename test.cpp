#include <iostream>


namespace cache {
    class MyClass {
    public:
        MyClass() {
            // Fully qualify the function name with helpers::
            helpers::helperFunction();
        }
    };
}

namespace helpers {
    void helperFunction() {
        std::cout << "Helper function called!" << std::endl;
    }
}
int main() {
    cache::MyClass obj;
    return 0;
}
