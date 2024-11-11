#include <iostream>

int main() {
    int* ptr = nullptr; // Initialize a null pointer

    if (ptr == nullptr) {
        std::cout << "The pointer is null (doesn't point to any address)."<<std::hex<<ptr << std::endl;
    } else {
        std::cout << "Pointer address: " << ptr << std::endl;
    }

    return 0;
}
