#ifndef HEADER_HH
#define HEADER_HH
#include<cmath>
#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include <ostream>
#include <istream>
#include <unordered_map>
#include "./project_RISC_V/resources.hh"
#include<deque>
#include<map>
#include<optional>
namespace Conversions
{
    std::string int_64_tohex(int64_t value);    // converts negative numbers also perfectly

    std::string decimal_to_hexa(uint8_t val, int bits);

    std::string binary_to_Hex(const std::string &binary_str, int num_bits);

    uint8_t hex_to_decimal_8(const std::string &hex_string);
    int hex_to_decimal_unsigned(const std::string &hex_string);

    std::string decimal_to_binary(int64_t num, int num_bits);

    int64_t hex_to_decimal_64_bit(std::string);


    uint8_t bin_to_unsigned_8_byte_uinteger(const std::string& binaryStr);

    int64_t bin_to_dec_any_bits(const std::string& binary_str);
    
}


namespace color
{

    const std::string reset = "\033[0m";
    const std::string red = "\033[31m";
    const std::string green = "\033[32m";
    const std::string yellow = "\033[33m";
    const std::string blue = "\033[34m";

    const std::string magenta = "\033[35m";
    const std::string cyan = "\033[36m";

    const std::string white = "\033[37m";
}

namespace memory
{

    // byte class stores the number as 8 bit integer
    class byte
    {
    public:
        uint8_t value;

        byte() : value(0) {}
        byte(uint8_t val) : value(val) {}
        byte(std::string val)  //try to send 2 hexbits always
        {
            size_t pos = val.find("0x");
            if (pos != std::string::npos)
            {
                val = val.substr(2);
            }
            if (val.size() != 2)
                val = "0" + val;



            value = Conversions::hex_to_decimal_8(val);
        }

        void setval_hex(std::string&& hex_bits);
        void setval_int(uint8_t val);

        void print();
        std::string string_rep() const;
        byte& operator=(const byte&other);
        // friend std::ostream & operator<<(std::ostream &os, const byte &b);
    };

    std::ostream &operator<<(std::ostream &os, const byte &b);

    class Register
    {

    public:
        int64_t value;

        // constructors
        Register()
        {
            this->value = 0;
        }

        Register(int64_t val)
        {

            this->value = val;
        }

        // need to write constructor for string and setval for both

        Register(std::string hex_string)
        {
            value = Conversions::hex_to_decimal_64_bit(hex_string);  // send 16hexbits always
        }

        // overload + , - , =
        // getvalue and setvalue that changes the original plae and gives ref to original place

        // + operator in both ways

        // for immediates
        Register operator+(const int64_t &second_operand) const;  

        // for 2 registers
        Register operator+(const Register &second_operand) const;

        // for 2 registers
        Register operator-(const Register &second_operand) const;

        // returning ref is for chaining = ,not that reference need to be assigned to something
        Register &operator=(const Register &second_operand);

        Register &operator=(const int64_t& value_);

        Register operator^(const Register& other) const;
        Register operator^(const int64_t&) const;

        Register operator<<(const Register&other) const;
        Register operator<<(const int64_t& other) const;

        Register operator>>(const Register&other) const;
        Register operator>>(const int64_t& other) const;

        Register r_shift(const Register& other) const;
        Register r_shift(const int64_t& other) const;

        bool operator>(const Register& other) const;
        bool operator>(const int64_t& other) const;

        bool operator<(const Register& other) const;
        bool operator<(const int64_t& other) const;
        
        bool operator==(const Register& other) const;
        bool operator==(const int64_t& other) const;

        bool operator!=(const Register& other) const;
        bool operator!=(const int64_t& other) const;

        Register operator&(const Register& other) const;
        Register operator&(const int64_t& other) const;

        Register operator|(const Register& other) const;
        Register operator|(const int64_t& other) const;

        std::uint64_t unsgnd() const;
        std::int64_t get_value() const;

        void set_value(int64_t element);
        void set_value(std::string hex_string);
    };

    std::ostream &operator<<(std::ostream &cp, const Register &to_be_printed);

}

namespace helpers
{

    // edit this function so that registers also go by default values
    void
    fill_default(
        std::vector<memory::byte> &text_section_mem, std::vector<memory::byte> data_stack__mem,
        int &data_ptr, int &st_ptr,std::unordered_map<std::string,memory::Register>&, int&pc
        ,std::vector<int>&b_points,std::deque<std::pair<std::string,int>>&call_stack);
    
    // void fill_data_section(std::vector<memory::byte> &text_section_mem, std::vector<memory::byte> data_stack__mem,
    //     int &data_ptr, int &st_ptr);

    std::string pc_value(const int&pc);
    void print_regs(std::unordered_map<std::string,memory::Register>&);
    void fill_text(std::vector<memory::byte>&text_section_mem,std::vector<std::pair<std::string,int>>&binarylines);
    void dispay_section(std::vector<memory::byte>&container);
    void init_data_section(std::vector<memory::byte>&data_section__mem , std::string& unified_string,int&ptr);
    void request_data(std::vector<memory::byte>&data_section__mem ,std::string address, int count);


// regs datasection dataptr(s) Binary_Lines , Raw_lines PC b_points
    void identify(std::unordered_map<std::string,memory::Register>&, std::vector<memory::byte>& ,
                 int& ,int& ,int&, std::vector<std::pair<std::string, int>>&,
                 std::vector<std::pair<std::string, int>>& , std::vector<int> &,bool is_step,std::deque<std::pair<std::string,int>>&call_stack,std::map<std::string,int>&__lable,int&e_pc,
                 std::pair<std::string,int>&current_stack);

               
    assembler::Type format_return(const std::string& opcode);
    

    // requirments : Binary_Line(only one is enough), raw lines(only one is enough)  , PC, regs ,
    void Encode_R(std::unordered_map<std::string,memory::Register>&,int&,std::string&,std::string&);
    void Encode_I(std::unordered_map<std::string,memory::Register>&,int&,std::string&,std::string&
    , std::vector<memory::byte>&,int&,int&,std::deque<std::pair<std::string,int>>&,int&e_pc,std::pair<std::string,int>&current_stack);


    void load_to_register(std::vector<memory::byte>&,memory::Register&,int,int64_t,bool,int line_no); // memory_section , register , no_of_bytes , index in data_section
    

    void Encode_U(std::unordered_map<std::string,memory::Register>&,int&,std::string&,std::string&);

    void Encode_S(std::unordered_map<std::string,memory::Register>&,int&,std::string&,std::string&
    , std::vector<memory::byte>&,int&,int&);

    void store_in_mem(std::vector<memory::byte>&data__stack_mem,std::string hex_string,int no_of_bytes,int start_index,int line_no);

    void Encode_B(std::unordered_map<std::string,memory::Register>&regs,int&PC,std::string&binary_string,std::string&raw_string);

    void Encode_J(std::unordered_map<std::string,memory::Register>&regs,int&PC,std::string&binary_string,std::string&raw_string,std::deque<std::pair<std::string,int>>&call_stack,int line_no,std::pair<std::string,int>&current_stack);

std::string  remove_leading_zeros(const std::string& str) ;

    bool reached_end(const int&PC, const int& size);

     void delete_break_point(std::vector<int>&b_points,const int& b);

        std::string removeLeadingSpaces(const std::string &str);

    
}

namespace extras
{
    std::string    removespaces(std::string recieved_str );
    float similarity( std::string s1, std::string s2 );
    std::optional<std::string> match(std::string cmd,std::vector<std::string>&deault_cmds,float min_req=1);
    void decision(std::vector<std::string>&input_tokens,std::vector<std::string>& av_cmds);
}

 

#endif