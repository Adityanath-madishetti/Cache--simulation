#include <iostream>
#include <iomanip>
#include <cstdint>

void printLowerHexBits(uint32_t number, int x) {
    uint32_t mask = (1U << x) - 1;  // Create a mask to keep only the lower x bits
    uint32_t tag = number & mask;   // Isolate the lower x bits
    int hexWidth = (x + 3) / 4;     // Calculate the required hex width (4 bits per hex digit)

    // Print the tag in hex, padded to the required width
    std::cout << "0x" 
              << std::setw(hexWidth) << std::setfill('0') 
              << std::hex << tag 
              << std::endl;
}

int main() {
    uint32_t number = 0xABCDEF12;
    int x = 14;  // Number of lower bits to print in hex
    printLowerHexBits(number, x);
    return 0;
}
