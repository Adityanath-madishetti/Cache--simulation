#include "resources.hh"
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "parser.hh"

/*

.data and .text

nothing

.text


*/

void parser::read()
{
    
    std::string line;
    while (std::getline(input, line))
    {
            original_line_counter++;
        
            line = helper::removeLeadingSpaces(line);
            line = helper::removeTrailingSpaces(line);
            
        


            std::stringstream data_checker(line);
            std::string first_word_for_line1;
            data_checker>>first_word_for_line1;

        if (first_word_for_line1== ".data")
        {
            std::string collector = "";
            
                first_word_for_line1.clear();

            // data_collection = "";

            while(data_checker>>first_word_for_line1)
                data_collection+=" "+first_word_for_line1;

            while (true)
            {
                
                original_line_counter++;
                if(!std::getline(input, collector))
                    return;
                
                if (collector.find(".text") != std::string::npos)
                    break;

                data_collection = data_collection + " " + collector;
            }

            
            collector=helper::removeLeadingSpaces(collector);
            collector=helper::removeTrailingSpaces(collector);

            if (collector == ".text")
            {
                continue;
            }
        }

       else if (line == ".text")
            continue;
        if (line.size() == 0 || (line.size()==1 && std::isspace(line[0]) ))
            continue;
        if (line[0] == ';') // comments are ignored
            continue;
        else
        {
            line_counter++;

            std::stringstream words(line);
            std::string first_word;

            words >> first_word;
            if (first_word.back() == ':') // label collections
            {
                // you can strip of labels and just note ist number
                line.erase(line.begin(), line.begin() + first_word.size());
                line = helper::removeLeadingSpaces(line); // this makes empty line to

                first_word.pop_back();
                if (line.size() == 0 || std::isspace(line[0])) // label in seperate line
                {

                    line_counter--;
                    label_lines[first_word] = line_counter + 1;
                    continue;
                }
                else
                {
                    label_lines[first_word] = line_counter;
                } // capture label line
            }

            lines.push_back({line,original_line_counter});

            std::stringstream format_checker(line);
            format_checker >> first_word; // first word now will be a fromat

            if (Typemap.find(first_word) == Typemap.end())
            {
                throw std::runtime_error("line no : " + std::to_string(line_counter + 1) + first_word + " : no such opcode found ");
                exit(1);
            }

            format.push_back(Typemap[first_word].type);
        }
    }
    input.close();
}

void parser::encode()
{
    for (int line_number = 0; line_number <= line_counter; line_number++)
    {

        std::vector<std::string> tokens;
        std::stringstream ss(lines[line_number].first);
        std::string word;

        int word_count = 0;
        while (ss >> word)
        {

            
            std::stringstream error;
            error << "line number :" << line_number + 1 << "\n"
                  << lines[line_number].first << "\n"
                  << "expected an identifier ,";
          
            tokens.push_back(word);
        }
        // check syntax in their respective functions

        switch (format[line_number])
        {
        case assembler::R_TYPE:
            binary_lines.push_back({assembler::Rtype(tokens, line_number, lines[line_number].first),lines[line_number].second});
            break;
        case assembler::I_TYPE:
            binary_lines.push_back({assembler::Itype(tokens, line_number, lines[line_number].first),lines[line_number].second});
            break;
        case assembler::S_TYPE:
            binary_lines.push_back({assembler::stype(tokens, line_number, lines[line_number].first),lines[line_number].second});
            break;
        case assembler::J_TYPE:
            binary_lines.push_back({assembler::Jtype(tokens, line_number, lines[line_number].first, label_lines),lines[line_number].second});
            break;
        case assembler::B_TYPE:
            binary_lines.push_back({assembler::Btype(tokens, line_number, lines[line_number].first, label_lines),lines[line_number].second});
            break;
        case assembler::U_TYPE:

            binary_lines.push_back({assembler::Utype(tokens, line_number, lines[line_number].first),lines[line_number].second});
            break;
        }
    }

    is_encoded = true;
}

void parser::print()
{
    if (!is_encoded)
        throw std::runtime_error("file is not encoded");
    else
    {
        for (auto &line : binary_lines)
        {
            output << helper::bin_to_hex(line.first) << std::endl;
        }
    }

    output.close();
}

std::string
parser::
    get_data_collection()
{
    return this->data_collection;
}

std::vector<std::pair<std::string,int>> 
parser::
get_binary_lines()
{
    return this->binary_lines;
}

std::vector<std::pair<std::string,int>> 
parser::get_raw_lines()
{
    return this->lines;
}

std::map<std::string,int> 
parser::get_label_map()
{
    return this->label_lines;
}
