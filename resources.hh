#ifndef HEADER_HH
#define HEADER_HH
#include <cmath>
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
#include<stack>
#include <deque>
#include <map>
#include <optional>
#include <algorithm>
#include <cctype>
// #define and &&
// #define or ||
/************************************************************************************************************* */
namespace Conversions
{
    std::string int_64_tohex(int64_t value); // converts negative numbers also perfectly

    std::string decimal_to_hexa(uint8_t val, int bits);

    std::string binary_to_Hex(const std::string &binary_str, int num_bits);

    uint8_t hex_to_decimal_8(const std::string &hex_string);
    int hex_to_decimal_unsigned(const std::string &hex_string);

    std::string decimal_to_binary(int64_t num, int num_bits);

    int64_t hex_to_decimal_64_bit(std::string);

    uint8_t bin_to_unsigned_8_byte_uinteger(const std::string &binaryStr);

    int64_t bin_to_dec_any_bits(const std::string &binary_str);

    std::string print_in_hex_for_tag(uint32_t num,int dig);
    

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
/**************************************************************************************************************************** */

namespace memory
{

    // byte class stores the number as 8 bit integer
    class byte
    {
    public:
        uint8_t value;

        byte() : value(0) {}
        byte(uint8_t val) : value(val) {}
        byte(std::string val) // try to send 2 hexbits always
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

        void setval_hex(std::string &&hex_bits);
        void setval_int(uint8_t val);

        void print();
        std::string string_rep() const;
        byte &operator=(const byte &other);
        // friend std::ostream & operator<<(std::ostream &os, const byte &b);
    };

    std::ostream &operator<<(std::ostream &os, const byte &b);

    /************************************************************************************** */

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
            value = Conversions::hex_to_decimal_64_bit(hex_string); //!!! send 16hexbits always
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

        Register &operator=(const int64_t &value_);

        Register operator^(const Register &other) const;
        Register operator^(const int64_t &) const;

        Register operator<<(const Register &other) const;
        Register operator<<(const int64_t &other) const;

        Register operator>>(const Register &other) const;
        Register operator>>(const int64_t &other) const;

        Register r_shift(const Register &other) const;
        Register r_shift(const int64_t &other) const;

        bool operator>(const Register &other) const;
        bool operator>(const int64_t &other) const;

        bool operator<(const Register &other) const;
        bool operator<(const int64_t &other) const;

        bool operator==(const Register &other) const;
        bool operator==(const int64_t &other) const;

        bool operator!=(const Register &other) const;
        bool operator!=(const int64_t &other) const;

        Register operator&(const Register &other) const;
        Register operator&(const int64_t &other) const;

        Register operator|(const Register &other) const;
        Register operator|(const int64_t &other) const;

        std::uint64_t unsgnd() const;
        std::int64_t get_value() const;

        void set_value(int64_t element);
        void set_value(std::string hex_string);
    };

    std::ostream &operator<<(std::ostream &cp, const Register &to_be_printed);

}

/*************************************************************************** */

// cache line class

namespace cache
{

    enum class validity
    {
        no,
        yes
    }; 
    enum class status
    {
        dirty,
        clean
    }; 

    class cache_line
    {
        // members as now public

    public:
        uint32_t tag_container; // lower bits are ur tag

        std::vector<memory::byte> cache_data;

        validity valid;

        status change;

        int no_of_tag_bits;

        // functions
        // 1 constructor

        cache_line(int cache_line_size, int no_of_tag_bits) // in bytes
        {
            cache_data.resize(cache_line_size,0);
            valid = validity::no;
            change = status::clean;
            this->no_of_tag_bits = no_of_tag_bits;
        }

/************************** */
        void set_block_size(int num_of_bytes, int num_of_tag_bits)
        {
            cache_data.resize(num_of_bytes);
            this->no_of_tag_bits = num_of_tag_bits;
        }
        void set_tag(uint32_t tag_val)
        {
            tag_container = tag_val;
        }

        bool match_line(uint32_t tag_value_to_be_checked)
        {
            return (this->is_valid())&&(tag_value_to_be_checked==this->tag_container);
        }

        cache_line &operator=(const cache_line &other)
        {
            this->tag_container = other.tag_container;
            this->cache_data = other.cache_data;
            this->change = other.change;
            this->valid = other.valid;
            this->tag_container = other.tag_container;
            return *this;
        }

        uint32_t get_tag()
        {
            return this->tag_container;
        }

        void set_status(status stat)
        {
            change = stat;
        }

        bool is_clean()
        {
            return (change == status::clean);
        }

        void set_validity(validity val)
        {
            this->valid = val;
        }

        bool is_valid()
        {
            return (this->valid == validity::yes);
        }

        memory::byte get_byte(int byte_off_set)
        {
            if (byte_off_set >= log2(cache_data.size()))
            {
                throw std::runtime_error("wrong acces  pattern. out of cache line");
            }

            return cache_data[byte_off_set];
        }

        void clear_line()
        {   
            
            this->valid=validity::no;
        }
    };

    class cache_association
    {

        //
        /*
              vector of cache lines
              no. of associations  or assosiation  size


        */

    public:
        std::vector<cache::cache_line> collection;
        int no_of_ways;
        int no_of_tag_bits;

        cache_association(int no_of_ways_, int no_of_tag_bits_, int line_size)
        {
            this->no_of_tag_bits = no_of_tag_bits_;
            this->no_of_ways = no_of_ways_;

            for(int i=0;i<no_of_ways;i++)
            {
                collection.emplace_back(line_size,no_of_tag_bits_);
            }
        }

        cache_association &operator=(const cache_association &other)
        {
            this->collection = other.collection;
            this->no_of_tag_bits = other.no_of_tag_bits;
            this->no_of_ways = other.no_of_ways;
            return *this;
        }

        cache_line& operator[](const  int& index)
        {
            return this->collection[index];
        }

        void clear_association()
        {
            for(int i=0 ; i<this->no_of_ways;i++)
            {
                collection[i].clear_line();
            }
        }

        int find_line(uint32_t tag)
        {
            int line_in_ass=0;
            for(auto& line : this->collection)
            {  
                if(line.match_line(tag))
                    return line_in_ass;

                        line_in_ass++;
            }
            return -1;
            
        }

    };

    enum class WRITE_POLICY_HIT { WB, WT};
    enum class WRITE_POLICY_MISS { WA , NA};
    enum  class REPLACEMENT_POLICY {LRU,FIFO,RANDOM};


    class cache_table
{
    public:
        std::vector<cache::cache_association> table; // its .size() gives no. of sets/indices so noneed of another variable
        int HIT_COUNT_;
        int MISS_COUNT_;

        int NO_OF_WAYS_; // associativity
        int CACHE_SIZE_; // BYTES
        int NO_OF_TAG_BITS_; 
        int NO_OF_OFFSET_BITS_;  
        int NO_OF_INDEX_BITS_; 


        WRITE_POLICY_HIT    WRITE_P_HIT_;
        WRITE_POLICY_MISS  WRITE_P_MISS_;
        REPLACEMENT_POLICY  REP_P_;

        bool is_fully_associative ;
        

        
        /** some more regarding policies */

        // constructor 
                                    // line size
        cache_table(int cache_size,int block_size,int associativity,std::string rep_p,std::string w_p);
        

        void print_policy(){
            std::cout<<"Write Policy on Hit : ";
            if(WRITE_P_HIT_==WRITE_POLICY_HIT::WB)
                std::cout<<"Write Back"<<std::endl;
            else
                std::cout<<"Write Through"<<std::endl;

            std::cout<<"Write Policy on Miss : ";
            if(WRITE_P_MISS_==WRITE_POLICY_MISS::WA)
                std::cout<<"Write Allocate"<<std::endl;
            else
                std::cout<<"No Write Allocate"<<std::endl;

            std::cout<<"Replacement Policy : ";
            if(REP_P_==REPLACEMENT_POLICY::LRU)
                std::cout<<"LRU"<<std::endl;
            else if(REP_P_==REPLACEMENT_POLICY::FIFO)
                std::cout<<"FIFO"<<std::endl;
            else
                std::cout<<"RANDOM"<<std::endl;
        }

        void invalidate()
        {
            for(int i=0;i<std::pow(2,this->NO_OF_INDEX_BITS_);i++)
            {
                this->table[i].clear_association();
            }

        }

        uint32_t index_find(uint32_t address); // gives set number
       

        cache_association& operator[](const int& index)
        {
            return table[index];
        }


    };  

}
/***************************************************************************************************************** */

/* these are some helping intermediate functions and also some of the inportant function that user need not to know*/

namespace helpers
{

    // edit this function so that registers also go by default values
    void
    fill_default(
        std::vector<memory::byte> &text_section_mem, std::vector<memory::byte> data_stack__mem,
        int &data_ptr, int &st_ptr, std::unordered_map<std::string, memory::Register> &, int &pc, std::vector<int> &b_points, std::deque<std::pair<std::string, int>> &call_stack);

    // void fill_data_section(std::vector<memory::byte> &text_section_mem, std::vector<memory::byte> data_stack__mem,
    //     int &data_ptr, int &st_ptr);

    std::string pc_value(const int &pc);
    void print_regs(std::unordered_map<std::string, memory::Register> &);
    void fill_text(std::vector<memory::byte> &text_section_mem, std::vector<std::pair<std::string, int>> &binarylines);
    void dispay_section(std::vector<memory::byte> &container);
    void init_data_section(std::vector<memory::byte> &data_section__mem, std::string &unified_string, int &ptr);
    void request_data(std::vector<memory::byte> &data_section__mem, std::string address, int count);

    // regs datasection dataptr(s) Binary_Lines , Raw_lines PC b_points
    void identify(std::unordered_map<std::string, memory::Register> &, std::vector<memory::byte> &,
                  int &, int &, int &, std::vector<std::pair<std::string, int>> &,
                  std::vector<std::pair<std::string, int>> &, std::vector<int> &, bool is_step, std::deque<std::pair<std::string, int>> &call_stack, std::map<std::string, int> &__lable, int &e_pc,
                  std::pair<std::string, int> &current_stack,bool& cache_is_on, cache::cache_table* CACHE,std::vector<std::string>&final_output_vector,std::string file_name);

    assembler::Type format_return(const std::string &opcode);

    // requirments : Binary_Line(only one is enough), raw lines(only one is enough)  , PC, regs ,
    void Encode_R(std::unordered_map<std::string, memory::Register> &, int &, std::string &, std::string &);
    void Encode_I(std::unordered_map<std::string, memory::Register> &, int &, std::string &, std::string &, std::vector<memory::byte> &, int &, int &, std::deque<std::pair<std::string, int>> &, int &e_pc, std::pair<std::string, int> &current_stack,
    bool chehe_is_on , cache::cache_table*CACHE,std::vector<std::string>&final_output_vector);

    void load_to_register(std::vector<memory::byte> &, memory::Register &, int, int64_t, bool, int line_no); // memory_section , register , no_of_bytes , index in data_section

    void cache_to_register(const std::vector<memory::byte> &, memory::Register &, int, int64_t, bool, int line_no,cache::cache_table*); // memory_section , register , no_of_bytes , index in data_section

    void mem_to_cache(std::vector<memory::byte> & , cache::cache_table* ,int, int,int,uint32_t,int); //data_index,no_of_bytes
    
    void store_in_cache(std::vector<memory::byte> &, std::string ,int,int,int,cache::cache_table*);

    void Encode_U(std::unordered_map<std::string, memory::Register> &, int &, std::string &, std::string &);

    void Encode_S(std::unordered_map<std::string, memory::Register> &, int &, std::string &, std::string &, std::vector<memory::byte> &, int &, int &,bool,cache::cache_table*,std::vector<std::string>&final_output_vector);

    void store_in_mem(std::vector<memory::byte> &data__stack_mem, std::string hex_string, int no_of_bytes, int start_index, int line_no);

    void Encode_B(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string);

    void Encode_J(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string, std::deque<std::pair<std::string, int>> &call_stack, int line_no, std::pair<std::string, int> &current_stack);

    std::string remove_leading_zeros(const std::string &str);

    bool reached_end(const int &PC, const int &size);

    void delete_break_point(std::vector<int> &b_points, const int &b);

    std::string removeLeadingSpaces(const std::string &str);

    std::string To_lower(const std::string& str);
    void print_cache_status(cache::cache_table* tab,std::string reqs[5]);
    void cache_stats(cache::cache_table*tab);
    

}
/******************************************************************************************** */
namespace extras
{
    std::string removespaces(std::string recieved_str);
    float similarity(std::string s1, std::string s2);
    std::optional<std::string> match(std::string cmd, std::vector<std::string> &deault_cmds, float min_req = 1);
    void decision(std::vector<std::string> &input_tokens, std::vector<std::string> &av_cmds);
}

#endif