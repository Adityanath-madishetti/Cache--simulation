#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include <bitset>
#include "resources.hh"
#include<cmath>

std::string Conversions::decimal_to_hexa(uint8_t val, int bits = 2)
{

    std::stringstream ss;
    ss << std::hex << std::setw(bits) << std::setfill('0') << static_cast<int>(val);
    return ss.str();
}




std::string Conversions::binary_to_Hex(const std::string &binary_str, int num_bits)
{

    if (binary_str.size() == 32)
    {
        std::bitset<32> bits(binary_str);
        uint32_t num = bits.to_ulong();
        std::stringstream ss;
        ss << std::hex << std::setw(8) << std::setfill('0') << num;

        return ss.str();
    }
    std::bitset<64> bits(binary_str);
    uint64_t num = bits.to_ullong();

    std::stringstream ss;
    ss << std::hex << std::setw(16) << std::setfill('0') << num;

    return ss.str();
}



uint8_t Conversions::hex_to_decimal_8(const std::string &hex_string)
{
    uint8_t decimalValue = static_cast<uint8_t>(stoi(hex_string, nullptr, 16));
    return decimalValue;
}




int Conversions::hex_to_decimal_unsigned(const std::string &hex_string)
{
    return std::stoi(hex_string, nullptr, 16);
}




std::string Conversions::decimal_to_binary(int64_t num, int num_bits)
{

    uint64_t binary_rep = static_cast<uint64_t>(num);

    if (num_bits == 32)
    {
        binary_rep = static_cast<uint32_t>(num);
    }

    std::bitset<64> bitset_rep(binary_rep);

    return bitset_rep.to_string().substr(64 - num_bits);
}




std::string Conversions::int_64_tohex(int64_t value)
{
    std::stringstream stream;
    stream << std::hex << std::setw(16) << std::setfill('0') << value;
    return stream.str();
}

int64_t Conversions::hex_to_decimal_64_bit(std::string hex_string)
{
    return static_cast<int64_t>(stoul(hex_string, nullptr, 16));
}


/******************************************************************************************** */
uint8_t Conversions:: bin_to_unsigned_8_byte_uinteger(const std::string &binaryStr)
{

    std::bitset<8> bits(binaryStr);

    return static_cast<uint8_t>(bits.to_ullong());
}




/* **************************************************************************************************** */
int64_t 
Conversions::
bin_to_dec_any_bits(const std::string& binary_str) {
    int n = binary_str.length();

 
    if (binary_str[0] == '0') {
        return std::stoll(binary_str, nullptr, 2);
    }

    
    std::uint64_t u_value = std::stoull(binary_str, nullptr, 2);

    
    int64_t signed_value = u_value - static_cast<uint64_t>(std::pow(2, n));

    return signed_value;
}