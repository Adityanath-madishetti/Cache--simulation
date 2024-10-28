#ifndef H_GUARD
#define H_GUARD
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
// 6 formats

namespace assembler
{
    enum Type
    {
        R_TYPE, //   0
        I_TYPE, //   1
        S_TYPE, //   2
        B_TYPE, //   3
        U_TYPE, //   4
        J_TYPE,  //   5
        NONE //6
    };

    typedef struct funct3
    {
        std::string value;
    } funct3;

    typedef struct funct7
    {
        std::string value;
    } funct7;

    typedef struct opt
    {
        std::string value;
    } opt;
    typedef struct combined_type_info
    {
        Type type;
        opt optcode;
        funct3 f3;
        funct7 f7;

    } cti;


    std::string Rtype(std::vector<std::string> tokens, int line_index, std::string &line_);
    std::string Itype(std::vector<std::string> tokens, int line_index, std::string &line_);
    std::string stype(std::vector<std::string> tokens, int line_index, std::string &line_);
    std::string Btype(std::vector<std::string> tokens, int line_index, std::string &line_, std::map<std::string, int> &label_tabel);
    std::string Jtype(std::vector<std::string> tokens, int line_index, std::string &line_, std::map<std::string, int> &label_tabel);
    std::string Utype(std::vector<std::string> tokens, int line_index, std::string &line_);

}

extern std::map<std::string, assembler::cti> Typemap;
extern std::map<std::string, std::string> register_to_binary;
extern std::map<std::string_view, std::string> binary_to_hex;

namespace helper
{

    std::string hexToBinary(const std::string &hex);
    std::string removeLeadingSpaces(const std::string &str);
    std::string removeTrailingSpaces(const std::string &str);
    std::string give_binary(std::string dec_num);
    std::string give_20_binary(std::string dec_num);
    std::string hexToBinary(const std::string &hex);
    std::string bin_to_hex(std::string bin);
}

    
#endif
