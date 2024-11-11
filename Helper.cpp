#include "resources.hh"
#include <bitset>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "./project_RISC_V/resources.hh"
#include <variant>
#include <cassert>
#include <deque>
#include <map>
#include <cstdlib>
#include <ctime>
#include <fstream>

/*********************************************************************************************************************** */
void helpers::print_cache_status(cache::cache_table *tab, std::string reqs[5])
{
    std::cout << "Cache Size: " << reqs[0] << std::endl;
    std::cout << "Block Size: " << reqs[1] << std::endl;
    std::cout << "Associativity: " << reqs[2] << std::endl;
    std::cout << "Replacement Policy: " << reqs[3] << std::endl;
    std::cout << "Write Back Policy: " << reqs[4] << std::endl;
}

void helpers::cache_stats(cache::cache_table *tab)
{
    if (tab->HIT_COUNT_ + tab->MISS_COUNT_ == 0)
    {
        std::cout << "no accesses yet!! " << std::endl;
    }

    float hitrate = (float)tab->HIT_COUNT_ / ((float)tab->HIT_COUNT_ + (float)tab->MISS_COUNT_);
    std::cout << "D-cache statistics: " << "Accesses=" << tab->HIT_COUNT_ + tab->MISS_COUNT_ << ", Hit=" << tab->HIT_COUNT_ << ", Miss=" << tab->MISS_COUNT_ << ", Hit Rate=" << hitrate << std::endl;
}

void helpers::cache_to_register(const std::vector<memory::byte> &memory_line, memory::Register &REG, int no_of_bytes, int64_t data_index, bool is_signed, int line_no, cache::cache_table *CACHE)
{

    // our data_index is starting byte off set
    int max = std::pow(2, CACHE->NO_OF_OFFSET_BITS_);

    if (data_index < 0x0 or data_index + no_of_bytes - 1 >= max)
    {
        throw std::runtime_error("Accessing unallocated memory! at line_no " + std::to_string(line_no));
    }

    std::string hex_string_to_be_loaded = "";

    for (int i = 0; i < no_of_bytes; i++)
    {
        hex_string_to_be_loaded = memory_line[i + data_index].string_rep() + hex_string_to_be_loaded;
    }

    // make it to 16hexbits based on signed a=or unsigned

    while (hex_string_to_be_loaded.size() != 16)
    {
        // assert(hex_string_to_be_loaded.size() % 2 == 0);

        if (hex_string_to_be_loaded[0] >= '8' and is_signed)
        {
            hex_string_to_be_loaded = "ff" + hex_string_to_be_loaded;
        }
        else if (hex_string_to_be_loaded[0] < '8' or !is_signed)
        {
            hex_string_to_be_loaded = "00" + hex_string_to_be_loaded;
        }
    }
    hex_string_to_be_loaded = "0x" + hex_string_to_be_loaded;

    REG = memory::Register(hex_string_to_be_loaded);
}

void helpers::mem_to_cache(std::vector<memory::byte> &data_stack__mem, cache::cache_table *CACHE, int line_no, int data_index, int no_of_bytes, uint32_t tag, int cache_set_number) // data_index,no_of_bytes
{

    int max = std::pow(2, CACHE->NO_OF_OFFSET_BITS_);

    if (data_index < 0x0 or data_index + no_of_bytes - 1 >= max)
    {
        throw std::runtime_error("Accessing unallocated memory! at line_no " + std::to_string(line_no));
    }

    int i = 0;
    for (auto &line : CACHE->table[cache_set_number].collection)
    {
        if (!line.is_valid())
            break;
        i++;
    }

    std::vector<memory::byte> req_memory;

    int start_mem_address = ((tag << CACHE->NO_OF_INDEX_BITS_) + cache_set_number) << CACHE->NO_OF_OFFSET_BITS_; // !!think

    start_mem_address -= 0x10000;

    for (int k = 0; k < std::pow(2, CACHE->NO_OF_OFFSET_BITS_); k++)
    {
        req_memory.push_back(data_stack__mem[start_mem_address + k]);
    }
    if (i < CACHE->table[cache_set_number].collection.size()) // no_of_lines_in_association
    {
        CACHE->table[cache_set_number].collection[i].tag_container = tag;
        CACHE->table[cache_set_number].collection[i].change = cache::status::clean; // set clean
        CACHE->table[cache_set_number].collection[i].valid = cache::validity::yes;  // set valid
        CACHE->table[cache_set_number].collection[i].cache_data = req_memory;

        if (CACHE->REP_P_ == cache::REPLACEMENT_POLICY::LRU)
        {
            cache::cache_line temp = CACHE->table[cache_set_number].collection[i];
            CACHE->table[cache_set_number].collection.erase(CACHE->table[cache_set_number].collection.begin() + i);
            CACHE->table[cache_set_number].collection.push_back(temp);
        }
    }
    else // replacement_policy
    {
        if (CACHE->REP_P_ != cache::REPLACEMENT_POLICY::RANDOM) // FIFO // LRU
        {

            if (CACHE->table[cache_set_number].collection[0].change != cache::status::clean)
            {
                uint32_t earlier_tag = CACHE->table[cache_set_number].collection[0].tag_container;
                int earlier_mem_address = ((earlier_tag << CACHE->NO_OF_INDEX_BITS_) + cache_set_number) << CACHE->NO_OF_OFFSET_BITS_; // !!think
                for (int k = 0; k < std::pow(2, CACHE->NO_OF_OFFSET_BITS_); k++)
                {
                    data_stack__mem[earlier_mem_address + k - 0x10000] = CACHE->table[cache_set_number].collection[0].cache_data[k]; // 0th index is being replaced
                }
            }

            CACHE->table[cache_set_number].collection.erase(CACHE->table[cache_set_number].collection.begin());

            // {
            //     cache_data.resize(cache_line_size);
            //     valid = validity::no;
            //     change = status::clean;
            //     this->no_of_tag_bits = no_of_tag_bits;
            // }

            CACHE->table[cache_set_number].collection.emplace_back(std::pow(2, CACHE->NO_OF_OFFSET_BITS_), CACHE->NO_OF_TAG_BITS_);
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].tag_container = tag;
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].change = cache::status::clean; // set clean
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].valid = cache::validity::yes;  // set clean
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].cache_data = req_memory;
        }
        else
        {
            std::srand(static_cast<unsigned int>(std::time(0)));
            int randomNumber = std::rand();
            randomNumber = randomNumber % CACHE->NO_OF_WAYS_;
            if (CACHE->table[cache_set_number].collection[randomNumber].change != cache::status::clean) // handling dirty block replacement
            {
                uint32_t earlier_tag = CACHE->table[cache_set_number].collection[randomNumber].tag_container;
                int earlier_mem_address = ((earlier_tag << CACHE->NO_OF_INDEX_BITS_) + cache_set_number) << CACHE->NO_OF_OFFSET_BITS_; // !!think

                for (int k = 0; k < std::pow(2, CACHE->NO_OF_OFFSET_BITS_); k++)
                {
                    data_stack__mem[earlier_mem_address + k - 0x10000] = CACHE->table[cache_set_number].collection[randomNumber].cache_data[k]; // random index is being replaced
                }
            }

            CACHE->table[cache_set_number].collection.erase(CACHE->table[cache_set_number].collection.begin() + randomNumber);

            // {
            //     cache_data.resize(cache_line_size);
            //     valid = validity::no;
            //     change = status::clean;
            //     this->no_of_tag_bits = no_of_tag_bits;
            // }

            CACHE->table[cache_set_number].collection.emplace_back(std::pow(2, CACHE->NO_OF_OFFSET_BITS_), CACHE->NO_OF_TAG_BITS_);
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].tag_container = tag;
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].change = cache::status::clean; // set clean
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].valid = cache::validity::yes;  // set valid
            CACHE->table[cache_set_number].collection[CACHE->NO_OF_WAYS_ - 1].cache_data = req_memory;
        }
        // first handle dirty block gracefully
    }
}

std::string helpers::To_lower(const std::string &str)
{
    std::string str2 = str;

    for (auto &c : str2)
    {
        c = std::tolower(c);
    }
    return str2;
}

std::string Conversions::print_in_hex_for_tag(uint32_t num, int dig)
{
    uint32_t a = (1U << dig) - 1;
    uint32_t tag = num & a;
    int b = (dig + 3) / 4;
    std::stringstream ss;
    ss  << std::setw(b) << std::setfill('0') << std::hex << tag ;
    std::string temp = "0x";
    return (temp+ helpers::remove_leading_zeros( ss.str()));
}

void helpers::fill_default(std::vector<memory::byte> &text_section_mem, std::vector<memory::byte> data_stack__mem,
                           int &data_ptr, int &st_ptr,
                           std::unordered_map<std::string, memory::Register> &regs, int &pc, std::vector<int> &b_points, std::deque<std::pair<std::string, int>> &call_stack)
{

    if (text_section_mem.size() != 0)
        text_section_mem.resize(0);

    for (auto &val : data_stack__mem)
    {
        val.setval_int(0);
    }

    data_ptr = -1;
    st_ptr = 0x40000 + 1;

    for (int i = 0; i < 32; i++)
    {
        std::string view = std::bitset<5>(i).to_string();
        regs[view] = memory::Register();
    }

    pc = 0x0;
    b_points.clear();
    if (!call_stack.empty())
        call_stack.clear();
}

/*********************************************************************************************************************** */

std::string
helpers::pc_value(const int &pc)
{
    std::stringstream ss;
    ss << "0x" << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << pc;
    return ss.str();
}

/*********************************************************************************************************************** */

void helpers::
    print_regs(std::unordered_map<std::string, memory::Register> &regs)
{
    // std::cout << std::setw(24) << "" << "-------------- " << color::red << " THE REGISTERS " << color::reset << " --------------" << std::endl;
    int i = 0;
    std::cout << "Registers" << std::endl;
    for (; i < 32; i++)
    {

        if (i <= 9)
            // std::cout << std::setw(30) << std::setfill(' ') << "x" << i << " : Value: " << regs[std::bitset<5>(i).to_string()]<< std::endl;
            std::cout << "x" << i << "  = " << regs[std::bitset<5>(i).to_string()] << std::endl;
        else
            // std::cout << std::setw(29) << std::setfill(' ') << "x" << i << " : Value: " << regs[std::bitset<5>(i).to_string()] << std::endl;
            std::cout << "x" << i << " = " << regs[std::bitset<5>(i).to_string()] << std::endl;
    }
    // std::cout << std::setw(24) << "" << "-------------- " << color::red << " THE   END  " << color::reset << " --------------" << std::endl;
}

/*********************************************************************************************************************** */

void helpers::fill_text(std::vector<memory::byte> &text_section_mem, std::vector<std::pair<std::string, int>> &binarylines)
{
    for (auto &line : binarylines)
    {
        int i = 31;

        for (int i = 24; i >= 0; i -= 8)
        {
            text_section_mem.emplace_back(Conversions::bin_to_unsigned_8_byte_uinteger(line.first.substr(i, 8)));
        }
    }
}

/*********************************************************************************************************************** */

void helpers::dispay_section(std::vector<memory::byte> &container)
{
    int i = 0;
    if (container.size() == Conversions::hex_to_decimal_unsigned("0x40001"))
    {

        std::stringstream ss;
        int i = 0x10000;

        for (auto &bys : container)
        {
            ss << std::hex << std::setw(5) << std::setfill('0') << i;
            std::cout << "mem[ 0x" << ss.str() << " ] = 0x" << bys << std::endl;
            i++;
            ss.str("");
        }
    }
    else
    {

        std::cout << std::setw(43) << "" << " ***************** " << color::green << "  TEXT SECTION   " << color::reset << "******************" << std::endl;
        std::cout << std::setw(48) << "" << "b0" << std::setw(12) << "b1" << std::setw(12) << "b2" << std::setw(12) << "b3" << std::endl;

        std::cout << std::setw(42) << ""; // Initial offset for the first row
        for (auto &bys : container)
        {
            std::cout << std::setw(7) << std::setfill(' ') << "0x" << bys << "  |";
            i++;
            if (i % 4 == 0)
                std::cout << std::endl
                          << std::setw(42) << ""; // Reset offset for each new row
        }
        std::cout << std::endl;
        std::cout << std::setw(43) << "" << "------------------------------------------------------------" << std::endl;
    }
}

/*********************************************************************************************************************** */

void helpers::init_data_section(std::vector<memory::byte> &data_sectio__mem, std::string &unified_string, int &ptr)
{

    std::replace(unified_string.begin(), unified_string.end(), ',', ' ');

    std::string type;

    std::stringstream ss(unified_string);

    std::string token;

    while (ss >> token)
    {

        if (token == ".dword" or token == ".word" or token == ".half" or token == ".byte")
        {
            type = token;
            continue;
        }
        else
        {
            if (type == ".dword")
            {
                // handel dedcimal and hex  both cases

                if (token.find("0x") != std::string::npos or token.find("-0x") != std::string::npos)
                {
                    // hex
                    if (token[0] == '-')
                    {
                        token = token.substr(3); // removed -0x
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }

                        int64_t value = (-1 * Conversions::hex_to_decimal_64_bit(token));
                        token = Conversions::int_64_tohex(value);
                    }
                    else
                    {
                        token = token.substr(2);
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }
                    }

                    // now fill memory in place

                    for (int i = 14; i >= 0; i -= 2)
                    {
                        ptr++;
                        data_sectio__mem[ptr] = memory::byte(token.substr(i, 2));
                    }
                }
                else
                {
                    std::string token_ = (token[0] == '-') ? (token.substr(1)) : (token);
                    for (auto &ch : token_)
                        if (!isdigit(ch))
                        {
                            throw std::runtime_error("unknow symbol used in .data .dword section");
                        }

                    // decimal

                    token = Conversions::int_64_tohex(static_cast<int64_t>(std::stoll(token)));
                    // 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15
                    for (int i = 14; i >= 0; i -= 2)
                    {
                        ptr++;
                        data_sectio__mem[ptr] = memory::byte(token.substr(i, 2));
                    }
                }
            }
            else if (type == ".word")
            {

                if (token.find("0x") != std::string::npos or token.find("-0x") != std::string::npos)
                {

                    if (token[0] == '-')
                    {
                        token = token.substr(3); // removed -0x
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }

                        int64_t value = (-1 * Conversions::hex_to_decimal_64_bit(token));
                        token = Conversions::int_64_tohex(value);
                    }
                    else
                    {
                        token = token.substr(2);
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }
                    }
                    token = token.substr(8);
                }
                else
                {
                    std::string token_ = (token[0] == '-') ? (token.substr(1)) : (token);
                    for (auto &ch : token_)
                        if (!isdigit(ch))
                        {
                            throw std::runtime_error("unknow symbol used in .data .word section");
                        }

                    // u have decimal value

                    int32_t val = std::stoi(token);
                    std::stringstream hex_stream;

                    hex_stream << std::setfill('0') << std::setw(8) << std::hex << val;
                    token = hex_stream.str();
                }
                // 0 1 2 3 4 5 6 7
                for (int i = 6; i >= 0; i -= 2)
                {
                    ptr++;
                    data_sectio__mem[ptr] = memory::byte(token.substr(i, 2));
                }
            }
            else if (type == ".half")
            {

                if (token.find("0x") != std::string::npos or token.find("-0x") != std::string::npos)
                {
                    if (token[0] == '-')
                    {
                        token = token.substr(3); // removed -0x
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }

                        int64_t value = (-1 * Conversions::hex_to_decimal_64_bit(token));
                        token = Conversions::int_64_tohex(value); // we are getting actual representation  of negative number
                    }
                    else
                    {
                        token = token.substr(2);
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }
                    }
                    token = token.substr(12);
                }
                else
                {
                    std::string token_ = (token[0] == '-') ? (token.substr(1)) : (token);
                    for (auto &ch : token_)
                        if (!isdigit(ch))
                        {
                            throw std::runtime_error("unknow symbol used in .data .half section");
                        }

                    // u have decimal value

                    int16_t val = std::stoi(token);
                    std::stringstream hex_stream;

                    hex_stream << std::setfill('0') << std::setw(4) << std::hex << val;
                    token = hex_stream.str();
                }
                // 0 1 2 3

                for (int i = 2; i >= 0; i -= 2)
                {
                    ptr++;
                    data_sectio__mem[ptr] = memory::byte(token.substr(i, 2));
                }
            }
            else if (type == ".byte")
            {
                if (token.find("0x") != std::string::npos or token.find("-0x") != std::string::npos)
                {
                    if (token[0] == '-')
                    {
                        token = token.substr(3); // removed -0x
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }

                        int64_t value = (-1 * Conversions::hex_to_decimal_64_bit(token));
                        token = Conversions::int_64_tohex(value); // we are getting actual representation  of negative number
                    }
                    else
                    {
                        token = token.substr(2);
                        while (token.size() != 16)
                        {
                            token = "0" + token; // made to 16 didgits
                        }
                    }
                    token = token.substr(14);

                    for (int i = 0; i >= 0; i -= 2)
                    {
                        ptr++;
                        data_sectio__mem[ptr] = memory::byte(token.substr(i, 2));
                    }
                }
                else
                {
                    std::string token_ = (token[0] == '-') ? (token.substr(1)) : (token);
                    for (auto &ch : token_)
                        if (!isdigit(ch))
                        {
                            throw std::runtime_error("unknow symbol used in .data .byte section");
                        }

                    // u have decimal value

                    int8_t val = (std::stoi(token));
                    std::stringstream hex_stream;

                    hex_stream << std::setfill('0') << std::setw(2) << std::hex << static_cast<int>(val);
                    // casue printing int8_t causes problem
                    token = hex_stream.str();
                    int size = token.size();
                    for (int i = size - 2; i >= size - 2; i -= 2)
                    {
                        ptr++;
                        data_sectio__mem[ptr] = memory::byte(token.substr(i, 2));
                    }
                }
                // 0 1 2 3
            }
            else
            {

                throw std::runtime_error(" un-known tyoe in .data section ");
            }
        }
    }

    // now we only have spaces
}

/*********************************************************************************************************************** */

void helpers::request_data(std::vector<memory::byte> &data_section__mem, std::string address, int count)
{
    int number = std::stoi(address, nullptr, 16);

    int index = number - 0x10000;
    // if u wanna include text section also then  make 2 if conditions based on value

    if (index + count > 0x40001) // because in bottom we are not includiing index+count so index+count can be 40001 because its not included
    {
        std::cout << "address out of range " << std::endl;
        return;
    }

    std::stringstream ss;
    // std::cout << std::setw(25) << "" << "********************" << color::green << "  DATA SECTION  " << color::reset << "**********************" << std::endl;
    for (int i = index; i < index + count; i++)
    {

        ss << std::hex << std::setw(5) << std::setfill('0') << i + 0x10000;
        // std::cout << std::setw(25) << "|";
        // std::cout << std::setw(18) << "" << "mem[ 0x" << ss.str() << " ] = 0x" << data_section__mem[i] << std::setw(19) << "" << "|" << std::endl;
        std::cout << "memory[ 0x" << ss.str() << " ] = 0x" << data_section__mem[i] << std::endl;
        ss.str("");
    }
    // std::cout << std::setw(25) << "" << "********************" << color::green << "  THE END " << color::reset << "*****************************" << std::endl;
}

/*********************************************************************************************************************** */

assembler::Type
helpers::
    format_return(const std::string &opcode)
{
    if (opcode == "0110011")
        return assembler::R_TYPE;
    else if (opcode == "0010011" or opcode == "0000011" or opcode == "1100111")
        return assembler::I_TYPE;
    else if (opcode == "0100011")
        return assembler::S_TYPE;
    else if (opcode == "1100011")
        return assembler::B_TYPE;
    else if (opcode == "1101111")
        return assembler::J_TYPE;
    else if (opcode == "0110111" or opcode == "0010111")
        return assembler::U_TYPE;
    else
        return assembler::NONE;
}

/************************************************************************************************************************/

void helpers::
    identify(std::unordered_map<std::string, memory::Register> &regs, std::vector<memory::byte> &data_stack__mem,
             int &data_ptr, int &stack_ptr, int &PC, std::vector<std::pair<std::string, int>> &Binary_Lines,
             std::vector<std::pair<std::string, int>> &raw_lines, std::vector<int> &b_points, bool is_step, std::deque<std::pair<std::string, int>> &call_stack, std::map<std::string, int> &__lables, int &e_pc, std::pair<std::string, int> &current_stack,
             bool &cache_is_on, cache::cache_table *CACHE, std::vector<std::string> &final_output_vector, std::string op_file_name)
{

    // identify and distribute to the functions of different formats

    // loop runs here and loop has switch cases and break point checking

    while (PC / 4 < Binary_Lines.size())
    {
        // sentence .first is binary string and second is original line number in actual input file
        auto &sentence = Binary_Lines[PC / 4];
        auto it = std::find(b_points.begin(), b_points.end(), sentence.second);

        if (!is_step and it != b_points.end()) // break point at present line
        {
            std::cout << "stopped execution due to break point " << std::endl;
            break;
        }

        // extract opcode bits
        std::string opcode = sentence.first.substr(25, 7);

        const assembler::Type format = format_return(opcode);

        int earlier_pc = PC;

        switch (format)
        {
        case assembler::R_TYPE:
            e_pc = PC;
            Encode_R(regs, PC, Binary_Lines[PC / 4].first, raw_lines[PC / 4].first);
            break;

        case assembler::I_TYPE:
            e_pc = PC;
            Encode_I(regs, PC, Binary_Lines[PC / 4].first, raw_lines[PC / 4].first, data_stack__mem, data_ptr, stack_ptr, call_stack, e_pc, current_stack, cache_is_on, CACHE, final_output_vector);
            break;
        case assembler::S_TYPE:
            e_pc = PC;
            Encode_S(regs, PC, Binary_Lines[PC / 4].first, raw_lines[PC / 4].first, data_stack__mem, data_ptr, stack_ptr, cache_is_on, CACHE,final_output_vector);
            break;
        case assembler::J_TYPE:
            e_pc = PC;
            Encode_J(regs, PC, Binary_Lines[PC / 4].first, raw_lines[PC / 4].first, call_stack, Binary_Lines[PC / 4].second, current_stack);
            break;
        case assembler::B_TYPE:
            e_pc = PC;
            Encode_B(regs, PC, Binary_Lines[PC / 4].first, raw_lines[PC / 4].first);
            break;
        case assembler::U_TYPE:
            e_pc = PC;
            Encode_U(regs, PC, Binary_Lines[PC / 4].first, raw_lines[PC / 4].first);
            break;
        case assembler::NONE:
            throw std::runtime_error("unknow opcode at line no " + std::to_string(sentence.second));
        }

        std::cout << "Executed " << helpers::removeLeadingSpaces(raw_lines[earlier_pc / 4.].first) << "; PC = " << helpers::pc_value(earlier_pc) << std::endl;

        // PC updated in respective format_functions because jumps change abruptly

        if (is_step)
        {
            if (cache_is_on && PC / 4 == Binary_Lines.size())
            {
                helpers::cache_stats(CACHE);

                std::ofstream file_name(op_file_name);

                if (!file_name)
                {
                    throw std::runtime_error("file for output not opened");
                }
                for (auto &ele : final_output_vector)
                {
                    file_name << ele << std::endl;
                }

                if (file_name.is_open())
                    file_name.close();
            }
            return;
        }

        if (cache_is_on && PC == Binary_Lines.size() * 4)
        {
            // !!!!print statistics
            helpers::cache_stats(CACHE);
            std::ofstream file_name(op_file_name);

            if (!file_name)
            {
                throw std::runtime_error("file for output not opened");
            }
            for (auto &ele : final_output_vector)
            {
                file_name << ele << std::endl;
            }

            if (file_name.is_open())
                file_name.close();
        }
    }
}

/**************************************************************************** */

void helpers::
    load_to_register(std::vector<memory::byte> &data__stack_mem, memory::Register &REG, int no_of_bytes, int64_t data_index, bool is_signed, int line_no)
{

    if (data_index < 0x0 or data_index >= 0x40001)
    {
        throw std::runtime_error("Accessing unallocated memory! at line_no " + std::to_string(line_no));
    }

    std::string hex_string_to_be_loaded = "";

    for (int i = 0; i < no_of_bytes; i++)
    {
        hex_string_to_be_loaded = data__stack_mem[i + data_index].string_rep() + hex_string_to_be_loaded;
    }

    // make it to 16hexbits based on signed a=or unsigned

    while (hex_string_to_be_loaded.size() != 16)
    {
        // assert(hex_string_to_be_loaded.size() % 2 == 0);

        if (hex_string_to_be_loaded[0] >= '8' and is_signed)
        {
            hex_string_to_be_loaded = "ff" + hex_string_to_be_loaded;
        }
        else if (hex_string_to_be_loaded[0] < '8' or !is_signed)
        {
            hex_string_to_be_loaded = "00" + hex_string_to_be_loaded;
        }
    }
    hex_string_to_be_loaded = "0x" + hex_string_to_be_loaded;

    REG = memory::Register(hex_string_to_be_loaded);
}

/************************************************************************************************************************ */

void helpers::store_in_mem(std::vector<memory::byte> &data__stack_mem, std::string hex_string, int no_of_bytes, int start_index, int line_no)
{
    int start = 14;

    if ((no_of_bytes != 1 and no_of_bytes % 2 == 1) or no_of_bytes > 8 or no_of_bytes <= 0)
        throw std::runtime_error("allocation gone wrong at " + std::to_string(line_no));

    for (int i = start_index; i < start_index + no_of_bytes; i++)
    {
        data__stack_mem[i].setval_hex(hex_string.substr(start, 2));
        start -= 2;
    }
}

void helpers::store_in_cache(std::vector<memory::byte> &memory_line, std::string hex_string, int no_of_bytes, int data_index, int line_no, cache::cache_table *CACHE)
{
    int start = 14;

    if ((no_of_bytes != 1 and no_of_bytes % 2 == 1) or no_of_bytes > 8 or no_of_bytes <= 0)
        throw std::runtime_error("allocation gone wrong at " + std::to_string(line_no));

    int max = std::pow(2, CACHE->NO_OF_OFFSET_BITS_);
    if (data_index < 0x0 or data_index + no_of_bytes - 1 >= max)
    {
        throw std::runtime_error("Accessing unallocated memory! at line_no " + std::to_string(line_no));
    }
    for (int i = data_index; i < data_index + no_of_bytes; i++)
    {
        memory_line[i].setval_hex(hex_string.substr(start, 2));
        start -= 2;
    }
}

/************************************************** */

std::string helpers::remove_leading_zeros(const std::string &str_)
{
    if (str_.empty())
    {
        return "0";
    }

    size_t i = 0;
    while (i < str_.length() and str_[i] == '0')
    {
        i++;
    }

    if (i == str_.length())
    {
        return "0";
    }

    return str_.substr(i);
}

/******************************************************************* */
bool helpers::reached_end(const int &PC, const int &size)
{
    return PC / 4 >= size;
}

void helpers::delete_break_point(std::vector<int> &b_points, const int &b)
{
    auto it = std::find(b_points.begin(), b_points.end(), b);
    if (it != b_points.end())
    {
        b_points.erase(it);
        std::cout << "deleted break point succesfuly " << std::endl;
    }
    else
    {
        std::cout << "break point not present at that line number " << std::endl;
    }
}

std::string helpers::removeLeadingSpaces(const std::string &str)
{
    size_t start = 0;
    while (start < str.size() && std::isspace(static_cast<unsigned char>(str[start])))
    {
        ++start;
    }
    return str.substr(start);
}
