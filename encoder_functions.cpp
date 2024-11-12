#include "resources.hh"
#include <bitset>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include "./project_RISC_V/resources.hh"
#include <variant>
#include <cstdint>
#include <deque>

/***************************************************************************************************************************/
void helpers::
    Encode_R(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string)
{
    std::string rd = binary_string.substr(20, 5);
    std::string rs1 = binary_string.substr(12, 5);
    std::string rs2 = binary_string.substr(7, 5);
    std::string f3 = binary_string.substr(17, 3);
    std::string f7 = binary_string.substr(0, 7);

    auto s3 = [](int i) -> std::string
    {
        return std::bitset<3>(i).to_string();
    };
    auto s7 = [](int i) -> std::string
    {
        return std::bitset<7>(i).to_string();
    };

    if (f3 == s3(0x0) and f7 == s7(0x00)) // add rs1, rs2 ,rd
    {
        regs[rd] = regs[rs1] + regs[rs2];
    }
    else if (f3 == s3(0x0) and f7 == s7(0x20)) // sub
    {
        regs[rd] = regs[rs1] - regs[rs2];
    }
    else if (f3 == s3(0x4) and f7 == s7(0x00)) // XOR
    {
        regs[rd] = regs[rs1] ^ regs[rs2];
    }
    else if (f3 == s3(0x6) and f7 == s7(0x00)) // OR
    {
        regs[rd] = regs[rs1] | regs[rs2];
    }
    else if (f3 == s3(0x7) and f7 == s7(0x00)) // AND
    {
        regs[rd] = regs[rs1] & regs[rs2];
    }
    else if (f3 == s3(0x1) and f7 == s7(0x00)) // sll
    {
        regs[rd] = regs[rs1] << (regs[rs2]);
    }
    else if (f3 == s3(0x5) and f7 == s7(0x00)) // srl
    {
        regs[rd] = regs[rs1].r_shift(regs[rs2]);
    }
    else if (f3 == s3(0x5) and f7 == s7(0x20)) // sra
    {
        regs[rd] = regs[rs1] >> regs[rs2];
    }
    else if (f3 == s3(0x2) and f7 == s7(0x00)) // slt
    {
        regs[rd] = (regs[rs1] < regs[rs2]) ? 1 : 0;
    }
    else if (f3 == s3(0x3) and f7 == s7(0x0)) // sltu
    {
        regs[rd] = (regs[rs1].unsgnd() < regs[rs2].unsgnd()) ? 1 : 0;
    }
    regs["00000"] = 0;
    PC += 4;
}
/***************************************************************************************************************************/
void helpers::
    Encode_I(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string,
             std::vector<memory::byte> &data_stack__mem, int &data_ptr, int &stack_ptr, std::deque<std::pair<std::string, int>> &call_stack, int &e_pc, std::pair<std::string, int> &current_stack, bool cache_is_on, cache::cache_table *CACHE,std::vector<std::string>&final_output_vector)
{

    std::string rd = binary_string.substr(20, 5);
    std::string f3 = binary_string.substr(17, 3);
    std::string rs1 = binary_string.substr(12, 5);
    std::string imm_str = binary_string.substr(0, 12);
    int64_t imm = Conversions::bin_to_dec_any_bits(imm_str);
    // if we extend sign bit or consider that that is the bit rep and calculate we get same answer so therfore bin_to_dec_any_bits work correctly (i.e due to msb extension)
    std::string rs2;
    std::string f7;
    // non-load type fist
    bool opcode_dec1 = (binary_string.substr(25, 7) == "0010011");
    bool opcode_dec2 = (binary_string.substr(25, 7) == "0000011");
    bool opcode_dec3 = binary_string.substr(25, 7) == "1100111";
    auto s3 = [](int i) -> std::string
    {
        return std::bitset<3>(i).to_string();
    };

    if (opcode_dec1 and f3 == s3(0x0)) // addi
    {
        regs[rd] = regs[rs1] + imm;
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x4)) // XORi
    {
        regs[rd] = regs[rs1] ^ imm;
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x6)) // ORi
    {
        regs[rd] = regs[rs1] | imm;
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x7)) // ANDi
    {
        regs[rd] = regs[rs1] & imm;
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x1) and imm_str.substr(0, 6) == "000000") // slli
    {
        regs[rd] = regs[rs1] << imm;
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x5) and imm_str.substr(0, 6) == "000000") // srl
    {
        regs[rd] = regs[rs1].r_shift(imm);
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x5) and imm_str.substr(0, 6) == "010000") // sra
    {
        imm_str = imm_str.substr(6, 6);

        int64_t imm = Conversions::bin_to_dec_any_bits(imm_str);
        regs[rd] = regs[rs1] >> imm;

        PC += 4;
    }
    else if ((opcode_dec1) and (f3 == s3(0x2))) // slt
    {
        regs[rd] = (regs[rs1] < imm) ? 1 : 0;
        PC += 4;
    }
    else if (opcode_dec1 and f3 == s3(0x3)) // sltu
    {
        std::uint64_t val = static_cast<uint64_t>(imm);
        regs[rd] = (regs[rs1].unsgnd() < imm) ? 1 : 0;
        PC += 4;
    }

    // load type functions

    else if (opcode_dec2)
    {
        int64_t adress_index = (regs[rs1] + imm).get_value() - 0x10000; // index in memory
        int no_of_bytes = 8;
        bool is_signed = true;
        if (f3 == s3(0x0)) // lb
        {

            no_of_bytes = 1;
            is_signed = true;
        }
        else if (f3 == s3(0x1)) // lh
        {

            no_of_bytes = 2;
            is_signed = true;
        }
        else if (f3 == s3(0x2)) // lw
        {

            no_of_bytes = 4;
            is_signed = true;
        }
        else if (f3 == s3(0x3)) // ld
        {
            no_of_bytes = 8;
            is_signed = true;
        }
        else if (f3 == s3(0x4)) // lbu
        {

            no_of_bytes = 1;
            is_signed = false;
        }
        else if (f3 == s3(0x5)) // lhu
        {

            no_of_bytes = 2;
            is_signed = false;
        }
        else if (f3 == s3(0x6)) // lwu
        {

            no_of_bytes = 4;
            is_signed = false;
        }

        // this is load so nothing related to write policy is used here but use replacement policy
        if (cache_is_on)
        {
            uint32_t actual_address = adress_index + 0x10000; // i.e earlier we subracted for indesx, but we need mem so added it back

            int cache_set_number = CACHE->index_find(actual_address);


            uint32_t tag = ((actual_address) << 12) >> (12 + CACHE->NO_OF_INDEX_BITS_ + CACHE->NO_OF_OFFSET_BITS_);


            // std::cout<<cache_set_number<<" "<<tag<<std::endl;
            int line_in_ass = CACHE->table[cache_set_number].find_line(tag);


            CACHE[cache_set_number];

            // if hit (includes validation)
            if (line_in_ass != -1) // hit
            {
                CACHE->HIT_COUNT_++;
                // take bytes from cache
                const std::vector<memory::byte> &memory_line = CACHE->table[cache_set_number].collection[line_in_ass].cache_data;

                // int data_index = (actual_address << (32 - CACHE->NO_OF_OFFSET_BITS_)) >> (32 - CACHE->NO_OF_OFFSET_BITS_);

                int data_index = actual_address%((int)std::pow(2,CACHE->NO_OF_OFFSET_BITS_));

                helpers::cache_to_register(memory_line, regs[rd], no_of_bytes, data_index, is_signed, PC / 4, CACHE);

                if (CACHE->REP_P_ == cache::REPLACEMENT_POLICY::LRU)
                {
                    cache::cache_line temp = CACHE->table[cache_set_number].collection[line_in_ass];
                    CACHE->table[cache_set_number].collection.erase(CACHE->table[cache_set_number].collection.begin() + line_in_ass);
                    CACHE->table[cache_set_number].collection.push_back(temp);
                }

                std::stringstream ss;
                ss<<"R: Address: 0x";
                ss<<std::hex<< actual_address<<std::dec;
                ss<<", Set: 0x"<<std::hex<<cache_set_number<<std::dec<<", Hit, Tag: "<< Conversions::print_in_hex_for_tag(tag,CACHE->NO_OF_TAG_BITS_)<<", ";
                if(CACHE->table[cache_set_number].collection[line_in_ass].is_clean())
                    ss<<"Clean";
                else
                    ss<<"Dirty";

                final_output_vector.push_back(ss.str());
                

            }
            // else go to memory (here comes replacement policy (one of case))
            else
            {

                CACHE->MISS_COUNT_++;
                // int data_index = (actual_address << (32 - CACHE->NO_OF_OFFSET_BITS_)) >> (32 - CACHE->NO_OF_OFFSET_BITS_);

                                int data_index = actual_address%((int)std::pow(2,CACHE->NO_OF_OFFSET_BITS_));


                load_to_register(data_stack__mem, regs[rd], no_of_bytes, adress_index, is_signed, PC / 4);
                
                helpers::mem_to_cache(data_stack__mem, CACHE, PC / 4, data_index, no_of_bytes, tag, cache_set_number);

                std::stringstream ss;
                ss<<"R: Address: 0x";
                ss<<std::hex<< actual_address<<std::dec;
                ss<<", Set: 0x"<<std::hex<<cache_set_number<<std::dec<<", Miss, Tag: "<<Conversions::print_in_hex_for_tag(tag,CACHE->NO_OF_TAG_BITS_) <<", ";

                                line_in_ass = CACHE->table[cache_set_number].find_line(tag);

                if(CACHE->table[cache_set_number].collection[line_in_ass].is_clean()) //ofc it will be clean beacuse read miss has nothing to do with any policy cause it always brings to cache the new and freash memory  
                {
                    ss<<"Clean";
                    
                }
                else
                {
                    ss<<"Dirty"; // never entwers this case (so if u want u can remove it, its put to check any error)
                   
                }
                    final_output_vector.push_back(ss.str());
            }
        }
        else
        {

            load_to_register(data_stack__mem, regs[rd], no_of_bytes, adress_index, is_signed, PC / 4);
        }
        PC += 4;
    }
    else if (opcode_dec3 and f3 == s3(0x0)) // should include jalr
    {

        regs[rd] = PC + 4;

        if (!call_stack.empty())
        {
            current_stack.first = call_stack.back().first;
            call_stack.pop_back();

            // PC that to be siplayed is done when show- stack is called
        }

        PC = static_cast<int>(regs[rs1].get_value() + imm);
        e_pc = PC - 4;
    }

    regs["00000"] = 0;
    return;
}

/***************************************************************************************************************************/

void helpers::
    Encode_U(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string)
{

    std::string opcode = binary_string.substr(25, 7);
    std::string rd = binary_string.substr(20, 5);
    std::string imm_str = binary_string.substr(0, 20);

    // convert to 32 because it was actually 20 bits but acc to u type u shift imm by 12 bits so thats y .
    // now after conversion the below conversion function converts ito to decimal absed on 2s complement rules
    while (imm_str.size() != 32)
    {
        imm_str += "0";
    }
    // size now became  32

    int64_t imm = Conversions::bin_to_dec_any_bits(imm_str);

    if (opcode == "0110111")
    {
        regs[rd] = imm;
    }
    else if (opcode == "0010111")
    {
        // code here

        uint64_t imm_for_auipc = static_cast<uint64_t>(imm);
        uint64_t pc_ = static_cast<uint64_t>(PC);
        int64_t value = static_cast<int64_t>(imm_for_auipc + PC);

        regs[rd] = value;
    }

    PC += 4;
    regs["00000"] = 0;

    return;
}

/***************************************************************************************************************************/

void helpers::Encode_S(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string, std::vector<memory::byte> &data__stack_mem, int &data_ptr, int &stack_ptr, bool cache_is_on, cache::cache_table *CACHE,
std::vector<std::string>&final_output_vector)
{
    auto s3 = [](int i) -> std::string
    {
        return std::bitset<3>(i).to_string();
    };

    // you need to identify index by substracting imm+rs1 with 0x10000
    std::string imm_str = binary_string.substr(0, 7) + binary_string.substr(20, 5); // 0-6 bits are msb and 20-24 are lsb
    int64_t imm = Conversions::bin_to_dec_any_bits(imm_str);

    std::string f3 = binary_string.substr(17, 3);
    std::string rs1 = binary_string.substr(12, 5);
    std::string rs2 = binary_string.substr(7, 5);

    int address_index = (regs[rs1] + imm).get_value() - 0x10000;

    // start from address_index and operate on no_of_bytes (based on instruction) and fill from the LSB ofregister

    std::string hex_string = Conversions::int_64_tohex(regs[rs2].get_value()); // sd rs2 imm(rs1)

    int no_of_bytes = 8;

    if (f3 == s3(0x0))
    {
        no_of_bytes = 1;
    }
    else if (f3 == s3(0x1))
    {
        no_of_bytes = 2;
    }
    else if (f3 == s3(0x2))
    {
        no_of_bytes = 4;
    }
    else if (f3 == s3(0x3))
    {
        no_of_bytes = 8;
    }
    if (cache_is_on)
    {

        uint32_t actual_address = address_index + 0x10000; // i.e earlier we subracted for indesx, but we need mem so added it back

        int cache_set_number = CACHE->index_find(actual_address);

        uint32_t tag = ((actual_address) << 12) >> (12 + CACHE->NO_OF_INDEX_BITS_ + CACHE->NO_OF_OFFSET_BITS_);
        // std::cout<<cache_set_number<<" "<<tag<<std::endl;
        int line_in_ass = CACHE->table[cache_set_number].find_line(tag);
        CACHE[cache_set_number];

        if (line_in_ass != -1) //hit
        {
            CACHE->HIT_COUNT_++;
            std::vector<memory::byte> &memory_line = CACHE->table[cache_set_number].collection[line_in_ass].cache_data;

            int data_index =   actual_address%((int)std::pow(2,CACHE->NO_OF_OFFSET_BITS_));
            // (actual_address << (32 - CACHE->NO_OF_OFFSET_BITS_)) >> (32 - CACHE->NO_OF_OFFSET_BITS_);

            helpers::store_in_cache(memory_line, hex_string, no_of_bytes, data_index, PC / 4, CACHE);

            if (CACHE->WRITE_P_HIT_ == cache::WRITE_POLICY_HIT::WT)
            {
                store_in_mem(data__stack_mem, hex_string, no_of_bytes, address_index, PC / 4);
            }
            else
            {
                CACHE->table[cache_set_number].collection[line_in_ass].change = cache::status::dirty;
            }

            /**** writing into vector */

            std::stringstream ss;
            ss<<"W: Address: 0x"<<std::hex<<actual_address<<std::dec<<", Set: 0x"<<std::hex<<cache_set_number<<std::dec<<", Hit, Tag: "
            << Conversions::print_in_hex_for_tag(tag,CACHE->NO_OF_TAG_BITS_)<<", ";

            if(CACHE->table[cache_set_number].collection[line_in_ass].is_clean())
                    ss<<"Clean";
                else
                    ss<<"Dirty";

                final_output_vector.push_back(ss.str());
                /************************ */

        }
        else //miss
        {
            CACHE->MISS_COUNT_++;
            // int data_index = (actual_address << (32 - CACHE->NO_OF_OFFSET_BITS_)) >> (32 - CACHE->NO_OF_OFFSET_BITS_);

            int data_index = actual_address%((int)std::pow(2,CACHE->NO_OF_OFFSET_BITS_));
           
            if(CACHE->WRITE_P_MISS_ == cache::WRITE_POLICY_MISS::NA)
            {
                // std::cout<<"entered in NA"<<std::endl;
                store_in_mem(data__stack_mem, hex_string, no_of_bytes, address_index, PC / 4);
            }
            else // allocate  (WA)
            {
                helpers::mem_to_cache(data__stack_mem, CACHE, PC / 4, data_index, no_of_bytes, tag, cache_set_number);

                line_in_ass = CACHE->table[cache_set_number].find_line(tag);// you brought that to cache in above line of code so you will get it in cache for sure

                if(line_in_ass==-1)
                {
                    throw std::runtime_error("unwanted brach entry in encodes function 's cache_miss case");
                }

                std::vector<memory::byte> &memory_line = CACHE->table[cache_set_number].collection[line_in_ass].cache_data;
                helpers::store_in_cache(memory_line, hex_string, no_of_bytes, data_index, PC / 4, CACHE);

                if (CACHE->WRITE_P_HIT_ == cache::WRITE_POLICY_HIT::WT) // acc to lab7 rules it will not go into it; because wt is only with na
                {
                    store_in_mem(data__stack_mem, hex_string, no_of_bytes, address_index, PC / 4);
                }
                else // goes into this only acc to lab7 rules
                {
                    CACHE->table[cache_set_number].collection[line_in_ass].change = cache::status::dirty;
                }

                std::stringstream ss;
                ss<<"W: Address: 0x"<<std::hex<<actual_address<<std::dec<<", Set: 0x"<<std::hex<<cache_set_number<<std::dec<<", Miss, Tag: "
                <<Conversions::print_in_hex_for_tag(tag,CACHE->NO_OF_TAG_BITS_)<<", ";
                ss<<"Dirty";

                final_output_vector.push_back(ss.str());

            }
            
             
            
            if(CACHE->WRITE_P_MISS_ == cache::WRITE_POLICY_MISS::NA)
            {
                
                std::stringstream ss;
                ss<<"W: Address: 0x"<<std::hex<<actual_address<<std::dec<<", Set: 0x"<<std::hex<<cache_set_number<<std::dec<<", Miss, Tag: "
                <<Conversions::print_in_hex_for_tag(tag,CACHE->NO_OF_TAG_BITS_)<<", ";
                ss<<"Clean";
                final_output_vector.push_back(ss.str());
            }
            

            
            

        }
    }
    else
    {
        store_in_mem(data__stack_mem, hex_string, no_of_bytes, address_index, PC / 4);
    }

    PC += 4;
    regs["00000"] = 0;
}

void helpers::Encode_B(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string)
{
    std::string rs1 = binary_string.substr(12, 5);
    std::string f3 = binary_string.substr(17, 3);
    std::string rs2 = binary_string.substr(7, 5);
    std::string imm_str = binary_string.substr(0, 1) + binary_string.substr(24, 1) + binary_string.substr(1, 6) + binary_string.substr(20, 4) + '0';

    int64_t offset = Conversions::bin_to_dec_any_bits(imm_str);

    auto s3 = [](int i) -> std::string
    {
        return std::bitset<3>(i).to_string();
    };
    bool decision;
    if (f3 == s3(0x0))
    {
        decision = (regs[rs1] == regs[rs2]);
    }
    else if (f3 == s3(0x1))
    {
        decision = (regs[rs1] != regs[rs2]);
    }
    else if (f3 == s3(0x4))
    {
        decision = (regs[rs1] < regs[rs2]);
    }
    else if (f3 == s3(0x5))
    {
        decision = (regs[rs1] > regs[rs2] or regs[rs1] == regs[rs2]);
    }
    else if (f3 == s3(0x6))
    {
        decision = (regs[rs1].unsgnd() < regs[rs2].unsgnd());
    }
    else if (f3 == s3(0x7))
    {
        decision = (regs[rs1].unsgnd() > regs[rs2].unsgnd() or regs[rs1].unsgnd() == regs[rs2].unsgnd());
    }

    if (decision)
        PC += static_cast<int>(offset);
    else
        PC += 4;

    regs["00000"] = 0;
}

/********************************************************************************************* */
void helpers::
    Encode_J(std::unordered_map<std::string, memory::Register> &regs, int &PC, std::string &binary_string, std::string &raw_string, std::deque<std::pair<std::string, int>> &call_stack, int line_no, std::pair<std::string, int> &current_stack)
{

    std::string rd = binary_string.substr(20, 5);
    std::string imm_str = binary_string.substr(0, 1) + binary_string.substr(12, 8) + binary_string.substr(11, 1) + binary_string.substr(1, 10) + '0';

    int64_t imm = Conversions::bin_to_dec_any_bits(imm_str);

    //  casting dosent matter cause becasue only 20 bits are used so remaining are just sign extension

    std::stringstream labelss(raw_string);
    std::string label;

    while (labelss >> label)
        ;

    // you extracted present label;

    call_stack.emplace_back(current_stack.first, line_no);

    regs[rd] = PC + 4;
    PC = PC + static_cast<int>(imm);

    current_stack.first = label;
    // line number would be that pc when show stack is called

    regs["00000"] = 0;
    return;
}