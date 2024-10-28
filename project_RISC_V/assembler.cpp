#ifndef AS_CPP
#define AS_CPP
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <unordered_map>
#include <sstream>
#include <bitset>
#include <algorithm>
#include <regex>
#include "resources.hh"
// 6 formats
/* R I S B J U  ===> 0 ,1,2,3,4,5*/ /*(enum)*/

using namespace assembler;

std::string assembler ::Rtype(std::vector<std::string> tokens, int line_index, std::string &line_)
{
    // f7 rs2 rs1 funct3  funct3 rd opcode

    /*< addi rd, rs1,  rs2,  >*/
    //             op  rd  rs1 rs2
    std::string f7 = Typemap[tokens[0]].f7.value;
    std::string f3 = Typemap[tokens[0]].f3.value;
    std::string opcode = Typemap[tokens[0]].optcode.value;

    tokens.erase(tokens.begin());

    if (tokens.size() != 3)
    {
        std::stringstream err;
        err << "line no  " << line_index + 1 << ": \n"
            << line_ << "\n"
                        "expected 3 registers given "
            << tokens.size() << " registers";
        throw std::runtime_error(err.str());
    }

    std::string reg[3];
    // 0 for rd
    // 1 for rs1
    // 2 for rs2
    int c = 0;
    for (auto &word : tokens)
    {
        if (word.back() == ',')
            word.pop_back();
        else if ((c == 0 || c == 1) && word.back() != ',')
        {
            std::stringstream error;
            error << "line number :" << line_index + 1 << "\n"
                  << line_ << "\n"
                  << "expected an identifier ,";

            throw std::runtime_error(error.str());
        }

        if (register_to_binary.find(word) == register_to_binary.end())
            throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + word + " available");
        word = helper::removeLeadingSpaces(word);
        word = helper::removeTrailingSpaces(word);
        reg[c] = word;
        c++;
    }
    // std::cout<<"f7 : "<<f7<<std::endl;
    // std::cout<<"rd : "<<reg[0]<<std::endl;
    // std::cout<<""
    return f7 + register_to_binary[reg[2]] + register_to_binary[reg[1]] + f3 +
           register_to_binary[reg[0]] + opcode;
}

std::string assembler::Itype(std::vector<std::string> tokens, int line_index, std::string &line_)
{

    // handel 2 differnet ways

    if (tokens.size() == 3)
    { // f2  ld rd, offset(rs)
        // ld x6, 0(x5)
        std::string f3 = Typemap[tokens[0]].f3.value;
        std::string opcode = Typemap[tokens[0]].optcode.value;
        std::string rd, rs, imm;

        tokens.erase(tokens.begin());

        rd = tokens[0];
        //  rd.pop_back();
        if (rd.back() == ',')
        {
            rd.pop_back();
        }
        else
        {
            std::stringstream error;
            error << "line number :" << line_index + 1 << "\n"
                  << line_ << "\n"
                  << "expected an identifier ,";
        }

        imm = tokens[1];
        // last baracket is last string character
        int ind_1 = imm.find('(');  // index of (
        int ind_2 = imm.size() - 1; // index of )
        rs = imm.substr(ind_1 + 1, ((ind_2 - 1) - (ind_1 + 1) + 1));
        imm = imm.substr(0, ind_1);

        int num = stoi(imm);
        if (num > 2047 || num < -2048)
        {
            std::stringstream err;
            err << "line no: " << line_ << std::endl
                << "the immideate is out of range";

            throw std::out_of_range(err.str());
        }

        imm = helper::give_binary(imm);
        // imm[11:0] rs1 funct3 rd opcode
        return (imm + register_to_binary[rs] +
                f3 + register_to_binary[rd] + opcode);
    }
    else if (tokens.size() == 4)
    {
        // addi x6, x7, imm  (op rd rs imm)
        // 0    1   2   3

        std::string f3 = Typemap[tokens[0]].f3.value;
        std::string opcode = Typemap[tokens[0]].optcode.value;
        std::string rd, rs, imm;

        std::string first_word = tokens[0];
        tokens.erase(tokens.begin());

        int c = 0;
        for (auto &word : tokens)
        {
            if (word.back() == ',')
                word.pop_back();
            else if ((c == 0 || c == 1) && word.back() != ',')
            {
                std::stringstream error;
                error << "line number :" << line_index + 1 << "\n"
                      << line_ << "\n"
                      << "expected an identifier ,";

                throw std::runtime_error(error.str());
            }
            c++;

            

            if (c == 1)
            {
                if (register_to_binary.find(word) == register_to_binary.end())
                    throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + word + " available");
                rd = word;
            }
            else if (c == 2)
            {
                if (register_to_binary.find(word) == register_to_binary.end())
                    throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + word + " available");
                rs = word;
            }
            else if (c == 3)
            {
                imm = word;
            }
        }

        std::string int_num = "^-?0$|^-?[1-9][0-9]*$";
        std::regex pattern(int_num);

        if (!regex_match(imm, pattern))
        {
            std::stringstream err;
            err << "line no : " << line_index + 1 << " " << line_ << "\n"
                << "immediate provided is in incorrect form ";
            throw std::runtime_error(err.str());
        }

        int num = stoi(imm);
        if (num > 2047 || num < -2048)
        {
            std::stringstream err;
            err << "line no: " << line_index + 1 << "c" << line_ << std::endl
                << "the immideate is out of range";

            throw std::out_of_range(err.str());
        }
        std::string num_12_bit = helper::give_binary(imm);

        if (first_word == "srai")
        {
            num_12_bit[1] = '1';
        }
        // imm[11:0] rs1 funct3 rd opcode
        return (num_12_bit + register_to_binary[rs] + f3 +
                register_to_binary[rd] + opcode);
    }

    return "";
}

/** restricting the range for shifting is still pending */

std::string assembler::stype(std::vector<std::string> tokens, int line_index, std::string &line_)
{

    // sd x6, 9(x5)
    // sd rs2, offset(rs1)

    if (tokens.size() != 3)
    {
        std::stringstream err;
        err << "line no  " << line_index + 1 << ": \n"
            << line_ << "\n"
                        "expected 2 arguments given "
            << tokens.size();
        throw std::runtime_error(err.str());
    }
    std::string f3 = Typemap[tokens[0]].f3.value;
    std::string opcode = Typemap[tokens[0]].optcode.value;
    std::string rs1, rs2, imm;

    tokens.erase(tokens.begin());

    rs2 = tokens[0];
    rs2.pop_back();
    if (register_to_binary.find(rs2) == register_to_binary.end())
        throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + rs2 + " available");
    imm = tokens[1];
    // last baracket is last string character
    int ind_1 = imm.find('(');  // index of (
    int ind_2 = imm.size() - 1; // index of )
    rs1 = imm.substr(ind_1 + 1, ((ind_2 - 1) - (ind_1 + 1) + 1));
    imm = imm.substr(0, ind_1);

    if (register_to_binary.find(rs1) == register_to_binary.end())
        throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + rs1 + " available");

    int num = stoi(imm);
    if (num > 2047 || num < -2048)
    {
        std::stringstream err;
        err << "line no: " << line_ << std::endl
            << "the immideate is out of range";

        throw std::out_of_range(err.str());
    }

    imm = helper::give_binary(imm);
    // imm[11:5] rs2 rs1 funct3 imm[4:0] opcode

    return (imm.substr(0, 7) + register_to_binary[rs2] + register_to_binary[rs1] + f3 + imm.substr(7, 5) + opcode);
}

std::string assembler::Btype(std::vector<std::string> tokens, int line_index, std::string &line_, std::map<std::string, int> &label_tabel)
{
    // tokens
    //  imm[12|10:5] rs2 rs1 funct3 imm[4:1|11] opcode
    //  beq x0, x1, Label // rs1 rs2 imm

    std::string f3 = Typemap[tokens[0]].f3.value;
    std::string opcode = Typemap[tokens[0]].optcode.value;
    std::string rs1, rs2, label;

    tokens.erase(tokens.begin());
    int c = 0;
    for (auto &word : tokens)
    {
        if (word.back() == ',')
            word.pop_back();

        else if ((c == 0 || c == 1) && word.back() != ',')
        {

            std::stringstream error;
            error << "line number :" << line_index + 1 << "\n"
                  << line_ << "\n"
                  << "expected an identifier ,";
            throw std::runtime_error(error.str());
        }
        c++;
    }

    // commas removed
    rs1 = tokens[0];
    if (register_to_binary.find(rs1) == register_to_binary.end())
        throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + rs1 + " available");
    rs2 = tokens[1];

    if (register_to_binary.find(rs2) == register_to_binary.end())
        throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + rs2 + " available");

    label = tokens[2];

    // std::cout<<label<<" "<<label.size();
    if (label_tabel.find(label) == label_tabel.end())
    {
        std::stringstream err;
        err << "line no: " << line_index + 1 << " : " << line_ << std::endl
            << "reference to unknow label " << label;
        throw std::runtime_error(err.str());
    }
    int abs_label_number = label_tabel[label];
    int offset_imm = (abs_label_number - line_index) * 4;

    std::string bin_offset = helper::give_binary(std::to_string(offset_imm));

    bin_offset.pop_back();
    bin_offset = bin_offset[0] + bin_offset;

    return bin_offset.substr(0, 1) + bin_offset.substr(2, 6) + register_to_binary[rs2] + register_to_binary[rs1] + f3 + bin_offset.substr(8, 4) + bin_offset.substr(1, 1) + opcode;
}

std::string assembler::Jtype(std::vector<std::string> tokens, int line_index, std::string &line_, std::map<std::string, int> &label_tabel)
{
    // jal rd, label
    std::string opcode = Typemap[tokens[0]].optcode.value;
    std::string rd, label;

    tokens.erase(tokens.begin());

    rd = tokens[0];
    if (rd.back() != ',')
    {
        std::stringstream error;
        error << "line number :" << line_index + 1 << "\n"
              << line_ << "\n"
              << "expected an identifier ,";
        throw std::runtime_error(error.str());
    }
    rd.pop_back();

    if (register_to_binary.find(rd) == register_to_binary.end())
        throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + rd + " available");

    label = tokens[1];
    if (label_tabel.find(label) == label_tabel.end())
    {
        std::stringstream err;
        err << "line no: " << line_index + 1 << " : " << line_ << std::endl
            << "reference to unknow label " << label;
        throw std::runtime_error(err.str());
    }

    int abs_dist = label_tabel[label];
    int off_set = (abs_dist - line_index) * 4;
    std::string bin_offset = helper::give_20_binary(std::to_string(off_set));
    bin_offset.pop_back();

    // imm[19|9:0|10|18:11] rd opcode
    return bin_offset.substr(0, 1) + bin_offset.substr(10, 10) + bin_offset.substr(9, 1) + bin_offset.substr(1, 8) + register_to_binary[rd] + opcode;
}


std::string assembler::Utype(std::vector<std::string> tokens, int line_index, std::string &line_)
{
    std::string opcode = Typemap[tokens[0]].optcode.value;
    std::string rd, uimm;
    tokens.erase(tokens.begin());

    /// lui x3, 0x100  or lui x3, 100
    rd = tokens[0];
    if (rd.back() != ',')
    {
        std::stringstream error;
        error << "line number :" << line_index + 1 << "\n"
              << line_ << "\n"
              << "expected an identifier ,";
        throw std::runtime_error(error.str());
    }
    rd.pop_back();

    if (register_to_binary.find(rd) == register_to_binary.end())
        throw std::runtime_error("line number " + std::to_string(line_index + 1) + " : no register " + rd + " available");

    uimm = tokens[1];

    if (uimm[0] == '0' && uimm[1] == 'x')
    {
        uimm.erase(0, 2); // remove 0x
        if (uimm.size() < 5)
        {
            while (uimm.size() < 5)
            {
                uimm = '0' + uimm;
            }
        }

        uimm = helper::hexToBinary(uimm);
    }
    else
    {
        // convert to binary
        if (uimm[0] == '-')
        {
            int num = stoi(uimm);
            std::bitset<20> bin_val(static_cast<unsigned long long int>(1048576 + num));
            uimm = bin_val.to_string();
        }
                else
                {
                     int num = std::stoi(uimm);
        // Directly create a bitset for non-negative numbers
                    std::bitset<20> bin_val(static_cast<unsigned long long int>(num));
                  uimm= bin_val.to_string();
                }
    }

    return uimm + register_to_binary[rd] + opcode;
}
#endif