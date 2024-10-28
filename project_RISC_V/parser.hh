#ifndef PARSER_HH
#define PARSER_HH
#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "resources.hh"



class parser
{
private:

    
    std::string data_collection="";
    std::ifstream input;
    std::ofstream output;
    std::vector<std::pair<std::string,int>> lines;

    std::map<std::string, int> label_lines;

    std::vector<assembler::Type> format;

    std::vector<std::pair<std::string,int>> binary_lines;
    int original_line_counter;

    int line_counter;
    bool is_encoded;

public:

    parser(std::string in_file, std::string o_file)
    {

        input.open(in_file);
        output.open(o_file);
        line_counter = -1;
        original_line_counter=0;
        is_encoded = false;

    }

    void read();
    void encode();
    void print();
    std::string get_data_collection();
    std::vector<std::pair<std::string,int>> get_binary_lines();
    std::vector<std::pair<std::string,int>>get_raw_lines();
    std::map<std::string,int> get_label_map();
};
#endif