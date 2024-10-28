#ifndef HF_CPP
#define HF_CPP
#include "resources.hh"
#include <bitset>
#include <algorithm>
#include <sstream>


using namespace assembler;


std::string helper::hexToBinary(const std::string &hex)
{
    std ::string hex_ =hex;

    if(hex_.find("0x")!=std::string::npos)
    {
       hex_= hex_.substr(2);
    }
    
    std::unordered_map<char, std::string> hex_to_bin = {
        {'0', "0000"}, {'1', "0001"}, {'2', "0010"}, {'3', "0011"}, {'4', "0100"}, {'5', "0101"}, {'6', "0110"}, {'7', "0111"}, {'8', "1000"}, {'9', "1001"}, {'a', "1010"}, {'b', "1011"}, {'c', "1100"}, {'d', "1101"}, {'e', "1110"}, {'f', "1111"}};

    std::string binary = "";
    for (char c : hex_)
    {
        binary += hex_to_bin[c];
    }

    return binary;
}
/****************************************************************************** */
std::string helper::removeLeadingSpaces(const std::string &str)
{
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start]))) {
        ++start;
    }
    return str.substr(start);
}   


/****************************************************************************** */

std::string helper::removeTrailingSpaces(const std::string &str)
{
      size_t end = str.size();
    while (end > 0 && std::isspace(static_cast<unsigned char>(str[end - 1]))) {
        --end;
    }
    return str.substr(0, end);
}
/****************************************************************************** */

std::string helper::give_binary(std::string dec_num)
{
    // return 12 bit binary
    if (dec_num[0] == '-')
    {
        int num = std::stoi(dec_num);
        // Compute two's complement for negative numbers
        std::bitset<12> bin_val(static_cast<unsigned long long int>(4096 + num)); // 4096 is 2^12
        return bin_val.to_string();
    }

    int num = std::stoi(dec_num);
    // Directly create a bitset for non-negative numbers
    std::bitset<12> bin_val(static_cast<unsigned long long int>(num));
    return bin_val.to_string();
}
/****************************************************************************** */

std::string helper::give_20_binary(std::string dec_num)
{
    // return 12 bit binary
    if (dec_num[0] == '-')
    {
        int num = std::stoi(dec_num);
        // Compute two's complement for negative numbers
        std::bitset<21> bin_val(static_cast<unsigned long long int>(2097152 + num)); // 2^21+num
        return bin_val.to_string();
    }

    int num = std::stoi(dec_num);
    // Directly create a bitset for non-negative numbers
    std::bitset<21> bin_val(static_cast<unsigned long long int>(num));
    return bin_val.to_string();
}
/****************************************************************************** */


std::string helper::bin_to_hex(std::string bin)
{
    int start = 0;
    int len = 4;

    std::string ans = "";
    while (start <= 28)
    {
        // std::string_view vw=std::string_view(bin).substr(start,4);
        // c++17 (string _view)
        // std::string vw = bin.substr(start,4);
        ans += binary_to_hex[std::string_view(bin).substr(start, 4)];
        start += 4;
    }
    return ans;
}
#endif