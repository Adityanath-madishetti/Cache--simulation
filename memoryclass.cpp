#include <cstdint>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <bitset>
#include "resources.hh"

/*********************************************************** */
 cache::cache_table::cache_table(int cache_size,int block_size,int associativity,std::string rep_p,std::string w_p){
            HIT_COUNT_=0;
            MISS_COUNT_=0;
            NO_OF_WAYS_ = associativity;
            CACHE_SIZE_=cache_size;

            if(NO_OF_WAYS_==0)
            {
                is_fully_associative = true;
                NO_OF_INDEX_BITS_=0; // only one index
                NO_OF_WAYS_ = (cache_size)/(block_size) ;// bsically equal to no. of lines
            }
            else
            {
                // number of indices = (cache_size)/(associativity*block_size) and bits is log2(number of indices)
                 int no_of_sets  = (cache_size)/(associativity*block_size);
                 NO_OF_INDEX_BITS_ = log2(no_of_sets); 
                 is_fully_associative=false;
            }

            NO_OF_OFFSET_BITS_= log2(block_size);
            
            NO_OF_TAG_BITS_ = 20 - NO_OF_INDEX_BITS_ - NO_OF_OFFSET_BITS_  ;

            helpers::pc_value(23);
            if(helpers::To_lower(w_p)=="wb")
            {
                WRITE_P_HIT_ = WRITE_POLICY_HIT::WB;
                WRITE_P_MISS_ = WRITE_POLICY_MISS::WA;
            }   
            else 
            {
                WRITE_P_HIT_ = WRITE_POLICY_HIT::WT;
                WRITE_P_MISS_ = WRITE_POLICY_MISS::NA;
            }
            

            rep_p=helpers::To_lower(rep_p);

            if(rep_p=="lru")
                REP_P_ = REPLACEMENT_POLICY::LRU;
            else if (rep_p=="fifo")
                REP_P_=REPLACEMENT_POLICY::FIFO;
            else
                REP_P_ = REPLACEMENT_POLICY::RANDOM;
            
            for(int i=0;i<pow(2,NO_OF_INDEX_BITS_);i++)
            {
                // dont send associativity casue associativity might be 0 aslo
                this->table.emplace_back(this->NO_OF_WAYS_,this->NO_OF_TAG_BITS_,block_size);
            }
        }

/**************************************************** */

void
memory::
byte::print()
{

    std::cout << "unsigned val is " << (int)value << " " << std::endl;
    std::cout << "hex-bit rep is " << Conversions::decimal_to_hexa(value, 2) << std::endl;
}

std::string
memory::byte::
string_rep() const 
{
    return Conversions::decimal_to_hexa(value, 2);
}

void 
memory::byte::
setval_hex(std::string&& hex_bits)
{
    size_t pos = hex_bits.find("0x");
    if (pos != std::string::npos)
    {
        hex_bits = hex_bits.substr(2);
    }
    if (hex_bits.size() != 2)
        hex_bits = "0" + hex_bits;
    this->value = Conversions::hex_to_decimal_8(hex_bits);
}

void 
memory::byte::
setval_int(uint8_t val)
{
    this->value = val;
}

memory::byte&
memory::byte::
operator=(const byte&other)
{
    this->value = other.value;
    return *this;
}

std::ostream &
memory::
operator<<(std::ostream &os, const byte &b)
{

    // based on usagae at debusgging time swap 2 commented values and use , but finally u decide which one to put in code

    // os << static_cast<int>(b.value);
    os<< b.string_rep();
    return os;
}
/******************************************************************************************* */

void 
memory::Register::
set_value(int64_t element)
{
    this->value = element;
}

void memory::Register::
set_value(std::string hex_string)
{
    value = Conversions::hex_to_decimal_64_bit(hex_string);
}



std::int64_t
memory::Register::
get_value() const
{
    return this->value;
}

std::uint64_t 
memory::Register::
unsgnd() const
{
    return static_cast<uint64_t>(this->value);
}


std::ostream &
memory::
operator<<(std::ostream &cp, const Register &to_be_printed) 
{
    // cp<<"0x"<<helpers::remove_leading_zeros( Conversions::int_64_tohex(to_be_printed.value))<<" "
    // << to_be_printed.value;

    cp<<"0x"<<helpers::remove_leading_zeros( Conversions::int_64_tohex(to_be_printed.value));
    return cp;
}
/******************************************** */

memory::Register
memory::Register::
operator+(const Register &second_operand) const 
{
    return Register(this->value + second_operand.value);
}

memory::Register
memory::Register::
operator+(const int64_t &second_operand) const 
{
    return Register(this->value + second_operand);
}

memory::Register
memory::Register::
operator-(const Register &second_operand) const 
{
    return Register(this->value - second_operand.value);
}


/******************************************** */
// returning ref is for chaining = ,not that reference need to be assigned to something
memory::Register &
memory::Register::
operator=(const Register &second_operand)
{
    this->value = second_operand.value;
    return *this;
}

memory::Register &
memory::Register::
operator=(const int64_t &value_)
{
    this->value = value_;
    return *this;
}
/**************************** */
memory::Register
memory::Register::
 operator^(const Register& other) const
 {
    return memory::Register(this->value ^ other.value);
 }

memory::Register
memory::Register::
 operator^(const int64_t& other) const
 {
    return memory::Register(this->value^other);
 }

/********************* */
 memory::Register
memory::Register::
 operator<<(const Register& other) const //arithmetic shift slli
 {
    return memory::Register(this->value<<other.value);
 }

 memory::Register
memory::Register::
 operator<<(const int64_t& other) const //arithmetic shift slli
 {
    return memory::Register(this->value<<other);
 }

/************************************* */

 memory::Register
memory::Register::
 operator>>(const Register& other) const   //srli
 {
        return memory::Register(this->value>>other.value);
 }

 memory::Register
memory::Register::
 operator>>(const int64_t& other) const   //srli
 {
        return memory::Register(this->value>>other);
 }



/******************************** */
memory::Register
memory::Register::
 r_shift(const Register& other) const //srai
 {
   return Register( static_cast<int64_t>(static_cast<uint64_t>(this->value) >> other.value));
 }

memory::Register
memory::Register::
r_shift(const int64_t& other) const //srai
{
   return Register( static_cast<int64_t>(static_cast<uint64_t>(this->value) >> other));
}



/******************************************** */
 bool
 memory::Register::
operator>(const Register& other) const
{
    return (this->value>other.value);
}

bool
 memory::Register::
operator>(const int64_t& other) const
{
    return (this->value>other);
}


/************************************************** */

 bool
 memory::Register::
operator<(const Register& other) const
{
    return (this->value<other.value);
}

bool
 memory::Register::
operator<(const int64_t& other) const
{
    return (this->value<other);
}


/******************************************************* */

 bool
 memory::Register::
operator==(const Register& other) const
{
    return (this->value==other.value);
}

bool
 memory::Register::
operator==(const int64_t& other) const
{
    return (this->value==other);
}
/************************************************************* */

 bool
 memory::Register::
operator!=(const Register& other) const
{
    return (this->value!=other.value);
}

bool
 memory::Register::
operator!=(const int64_t& other) const
{
    return (this->value!=other);
}

/******************************************************************* */
memory::Register
memory::Register::
operator|(const Register& other) const
{
    return Register(this->value|other.value);
}

memory::Register
memory::Register::
operator|(const int64_t& other) const
{
    return Register(this->value|other);
}

/******************************************************************* */
memory::Register
memory::Register::
operator&(const Register& other) const
{
    return Register(this->value&other.value);
}

memory::Register
memory::Register::
operator&(const int64_t& other) const
{
    return Register(this->value&other);
}
/**************************************************************************** */