#include <algorithm>
#include "resources.hh"
#include <cctype>
#include <fstream>
#include "./project_RISC_V/parser.hh"
#include <cctype>
#include <deque>
#include <map>
#include <optional>
#include <iomanip>
#include <fstream>
std::vector<std::string> input_collector()
{
    std::string input;
    std::getline(std::cin, input);

    if (input.size() == 0 || std::isspace(input[0]))
    {
        return input_collector();
    }
    std::stringstream ss(input);

    std::vector<std::string> input_tokens;

    std::string prev = "";

    while (ss >> input)
    {
        if (prev != "load")
            std::transform(input.begin(), input.end(), input.begin(), ::tolower);

        input_tokens.push_back(input);
        prev = input;
    }

    return input_tokens;
}

bool is_whitespace(char c)
{
    return std::isspace(static_cast<unsigned char>(c));
}

void trim(std::string &s)
{
    s.erase(s.find_last_not_of(" \n\r\t") + 1);
}

int main()
{

    try
    {

        /************************************************************************************* */
        std::vector<memory::byte> text_section_mem;
        text_section_mem.reserve(Conversions::hex_to_decimal_unsigned("0x10000")); // 0x0 to 0xffff

        std::vector<memory::byte> data_stack__mem;
        data_stack__mem.resize(Conversions::hex_to_decimal_unsigned("0x40001")); // 0x10000 , 0x50000

        std::unordered_map<std::string, memory::Register> regs; // registers 0-31
        int PC = 0x0;
        int e_pc = 0x0;
        std::vector<int> b_points;

        int data_ptr = -1;
        int stack_ptr = 0x40000 + 1; // 0x40000+1
        bool is_loaded = false;
        std::ifstream input_fileStream;

        std::vector<std::pair<std::string, int>> Binary_Lines; // final binary
        std::vector<std::pair<std::string, int>> raw_lines;    // lines from file
        std::map<std::string, int> __lables;

        std::deque<std::pair<std::string, int>> call_stack;
        std::pair<std::string, int> current_stack;

        /*********************** cache variables declaration***************** */

        bool cache_is_on = false;
        std::string reqs[5];
        cache::cache_table *CACHE = nullptr;
        std::vector<std::string> final_output_vector;
        std::string op_file_name;
        /******************************************************************* */

        std::vector<std::string> available_cmds = {"print text", "show-stack", "run", "step", "break", "del break", "load", "exit", "regs", "mem count", "clear", "cls"};
        /************************************************************************************************** */
        bool just_started;
        while (true)
        {

            std::vector<std::string> input_tokens = input_collector();

            /************************************************** */
            if ((input_tokens[0] == "cache_sim") && (input_tokens[1] == "dump"))
            {
                if (cache_is_on)
                {
                    std::fstream file_obj;
                    std::ofstream MyFile(input_tokens[2]);
                    // file_obj.open(input_tokens[2]);

                    // Set: 0x00, Tag: 0x100, Clean
                    // Set: 0x01, Tag: 0x123, Clean
                    // Set: 0x01, Tag: 0x736, Dirty
                    // Set:0x02, Tag: 0x145, Dirty
                    // Set:0x10, Tag: 0x321, Clean

                    int set = 0;

                    auto func = [](cache::cache_line line) -> std::string
                    {
                        if (line.is_clean())
                            return "Clean";
                        else
                            return "Dirty";
                    };

                    for (auto &ass : CACHE->table)
                    {
                        for (auto &line : ass.collection)
                        {
                            if (line.is_valid())
                                MyFile << "set: 0x" << std::hex << set << ", Tag: 0x" << line.get_tag() << ", " << func(line) << std::dec << std::endl;
                        }
                        set++;
                    }
                }
            }
            

            else if (input_tokens[0] == "cache_sim" && input_tokens[1] == "enable")
            {
                cache_is_on = true;

                std::ifstream config_file;

                config_file.open(input_tokens[2]);

                std::string temps;

                int i = 0;
                while (std::getline(config_file, temps))
                {
                    if (i >= 5)
                    {
                        throw std::runtime_error("config_file problem");
                    }
                    trim(temps);
                    reqs[i] = temps;
                    i++;
                }
                if(cache_is_on)
                    delete CACHE;
                CACHE = new cache::cache_table(std::stoi(reqs[0]), std::stoi(reqs[1]), std::stoi(reqs[2]), reqs[3], reqs[4]);
            }

            else if (input_tokens[0] == "cache_sim" && input_tokens[1] == "disable")
            {

                if (cache_is_on)
                {

                    delete CACHE;
                }
                cache_is_on = false;
            }

            else if (input_tokens[0] == "cache_sim" && input_tokens[1] == "status")
            {
                if (cache_is_on)
                {
                    helpers::print_cache_status(CACHE, reqs);
                }
                else
                {
                    std::cout << "cache simulation is on!!" << std::endl;
                }
            }
            else if (input_tokens[0] == "cache_sim" && input_tokens[1] == "invalidate")
            {
                if (cache_is_on)
                {
                    //  !!!!!!!!  its not enough ( in wb policy first handel dirty bits and then write back them)
                    int cache_set_number = 0;
                    for (auto &c : CACHE->table)
                    {
                        for (auto &line : c.collection)
                        {
                            if (line.is_valid() && (!(line.is_clean())))
                            {
                                uint32_t earlier_tag = line.tag_container;
                                int earlier_mem_address = ((earlier_tag << CACHE->NO_OF_INDEX_BITS_) + cache_set_number) << CACHE->NO_OF_OFFSET_BITS_; // !!think
                                for (int k = 0; k < std::pow(2, CACHE->NO_OF_OFFSET_BITS_); k++)
                                {
                                    data_stack__mem[earlier_mem_address + k - 0x10000] = CACHE->table[cache_set_number].collection[0].cache_data[k]; // 0th index is being replaced
                                }
                                line.valid = cache::validity::no;
                                line.change = cache::status::clean;
                            }
                        }
                        cache_set_number++;
                    }
                }
            }
            else if (input_tokens[0] == "cache_sim" && input_tokens[1] == "stats")
            {
                if (cache_is_on)
                {
                    helpers::cache_stats(CACHE);
                }
                else
                {
                    std::cout << "cache-simulation is disabled " << std::endl;
                }
            }

            /***************************************************************** */

            else if (input_tokens[0] == "exit")
            {
                std::cout << "Exited the simulator" << std::endl;
                break;
            }

            else if (input_tokens[0] == "load" && input_tokens.size() == 2)
            {
                is_loaded = true;

                if (cache_is_on)
                {
                    CACHE->invalidate(); // just cleans every thing by marking all valids as not valid
                }

                if (!final_output_vector.empty())
                {
                    final_output_vector.clear();
                }

                if (input_fileStream.is_open())
                    input_fileStream.close(); // to close the previous file

                auto remove_extension = [](const std::string &filename) -> std::string
                {
                    size_t pos = filename.find_last_of('.');

                    if (pos == std::string::npos || pos == 0)
                        return filename;

                    return filename.substr(0, pos);
                };

                std::string filename = input_tokens[1];
                op_file_name = remove_extension(input_tokens[1]) + ".output";

                std::ofstream temp_file;// for cache
                if (cache_is_on)
                {
                    temp_file.open(op_file_name);
                    if (!temp_file.is_open())
                    {
                        temp_file.open(input_tokens[1]);
                        temp_file.close();
                    }
                    else
                    {
                        temp_file.close();
                    }
                }

                parser P_object(filename, "output.hex");

                P_object.read();

                P_object.encode();

                std::string data_section = P_object.get_data_collection();

                P_object.print(); // redundant , actually for testing purpose
                e_pc = 0x0;
                just_started = true;
                helpers::fill_default(text_section_mem, data_stack__mem, data_ptr, stack_ptr, regs, PC, b_points, call_stack);

                current_stack.first = "main";
                current_stack.second = 0x0;

                // now take from output ,and store them in some vector
                Binary_Lines = P_object.get_binary_lines();
                raw_lines = P_object.get_raw_lines();
                __lables = P_object.get_label_map();

                helpers::fill_text(text_section_mem, Binary_Lines); // fills all the binary bienary code into text section
                helpers::init_data_section(data_stack__mem, data_section, data_ptr);
            }

            // duplicates as of know
            /***********************   regs ******************************* */
            else if (input_tokens[0] == "regs" && (input_tokens.size() == 1 || input_tokens.size() == 2))
            {
                if (!is_loaded)
                {
                    std::cout << "file not yet loaded" << std::endl;
                    continue;
                }

                if (input_tokens.size() == 1)
                    helpers::print_regs(regs);
                else
                {
                    if (input_tokens[1].front() == '-')
                    {
                        input_tokens[1].erase(0, 1);

                        bool correct_format = true;
                        for (auto &character : input_tokens[1])
                        {
                            if (!std::isdigit(character))
                            {
                                correct_format = false;
                                std::cout << "incorrect flag for regs" << std::endl;
                                break;
                            }
                        }

                        if (correct_format)
                        {
                            std::cout << "register X" << std::stoi(input_tokens[1]) << ": ";
                            std::cout << regs[std::bitset<5>(std::stoull(input_tokens[1])).to_string()] << std::endl;
                        }
                    }
                    else
                    {
                        std::cout << "No such command " << std::endl;
                    }
                }

                std::cout << std::endl;
            }

            /***********************   print text ******************************* */
            else if (input_tokens[0] == "print" && input_tokens[1] == "text")
            {
                if (!is_loaded)
                {
                    std::cout << "file not yet loaded" << std::endl;
                    continue;
                }

                helpers::dispay_section(text_section_mem);
            }

            /***********************   run ******************************* */
            else if (input_tokens[0] == "run" && input_tokens.size() == 1)
            {
                just_started = false;

                if (!is_loaded)
                {
                    std::cout << "file not yet loaded" << std::endl;
                    continue;
                }

                if (helpers::reached_end(PC, Binary_Lines.size()))
                {
                    std::cout << "Nothing to run " << std::endl;
                    continue;
                }

                helpers::identify(regs, data_stack__mem, data_ptr, stack_ptr, PC, Binary_Lines, raw_lines, b_points, false, call_stack, __lables, e_pc, current_stack, cache_is_on, CACHE, final_output_vector, op_file_name);
            }

            /***********************   break  ******************************* */

            else if (input_tokens[0] == "break" && input_tokens.size() == 2)
            {

                if (!is_loaded)
                {
                    std::cout << "file not yet loaded" << std::endl;
                    continue;
                }

                auto useful_function = [&raw_lines, &input_tokens]() -> int
                {
                    int b_point = std::stoi(input_tokens[1]);

                    for (int i = 0; i < raw_lines.size(); i++)
                    {
                        if (raw_lines[i].second >= b_point)
                        {
                            return raw_lines[i].second;
                        }
                    }
                    return -1;
                };

                int eval = useful_function();
                if (eval == -1)
                {
                    std::cerr << "no line present at " << input_tokens[1] << std::endl;
                    continue;
                }
                else
                {
                    b_points.push_back(eval);
                    std::cout << "break point set at line " << eval << std::endl;
                }
            }

            /***********************   del break ******************************* */

            else if (input_tokens[0] == "del" && input_tokens[1] == "break" && input_tokens.size() == 3)
            {

                if (!is_loaded)
                    std::cout << "file not yet loaded" << std::endl;
                else
                    helpers::delete_break_point(b_points, std::stoi(input_tokens[2]));
                continue;
            }

            /***********************   step ******************************* */

            else if (input_tokens[0] == "step" && input_tokens.size() == 1)
            {
                just_started = false;
                // to know main:0 in satck

                if (!is_loaded)
                    std::cout << "file not yet loaded" << std::endl;

                else if (helpers::reached_end(PC, Binary_Lines.size()))
                    std::cout << "Nothing to step " << std::endl;

                else
                    helpers::identify(regs, data_stack__mem, data_ptr, stack_ptr, PC, Binary_Lines, raw_lines, b_points, true, call_stack, __lables, e_pc, current_stack, cache_is_on, CACHE, final_output_vector, op_file_name);
            }

            /***********************   mem ******************************* */

            else if (input_tokens[0] == "mem" && input_tokens.size() == 3)
            {
                if (!is_loaded)
                {
                    std::cout << "file not yet loaded" << std::endl;
                    continue;
                }

                helpers::request_data(data_stack__mem, input_tokens[1], std::stoi(input_tokens[2]));
            }

            /***********************  clear ******************************* */

            else if (input_tokens[0] == "cls" || input_tokens[0] == "clear")
            {
                std::cout << "\033[2J\033[1;1H"; // makes platform independent
            }

            else if (input_tokens[0] == "show-stack")
            {

                if (PC / 4 < Binary_Lines.size())
                    std::cout << "Call Stack:" << std::endl;

                for (auto &it : call_stack)
                {

                    std::cout << it.first << ":" << it.second << std::endl;
                }

                if (PC == 0)
                {
                }

                if (just_started)
                    std::cout << "main:0" << std::endl;
                else if (PC / 4 < Binary_Lines.size())
                    std::cout << current_stack.first << ":" << Binary_Lines[e_pc / 4].second << std::endl;
                else
                    std::cout << "Empty Call Stack: Execution complete" << std::endl;
            }
            else if (input_tokens[0] == "pc")
            {
                std::cout << helpers::pc_value(PC) << std::endl;
                std::cout << "PC/4  : " << PC / 4 << " " << " total lines : " << Binary_Lines.size() << std::endl;
            }

            else
            {
                extras::decision(input_tokens, available_cmds);
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << e.what();
    }
}