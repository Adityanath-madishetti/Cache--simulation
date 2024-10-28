#include <iostream>
#include <set>
#include <string>
#include <algorithm>
#include <vector>
#include "resources.hh"
#include <optional>

using namespace extras;

std::string extras::removespaces(std::string recieved_str)
{
    std::string::iterator new_end = std::remove_if(recieved_str.begin(), recieved_str.end(), [](const char &ch) -> bool
                                                   { return std::isspace(ch); });

    recieved_str.erase(new_end, recieved_str.end());
    return recieved_str;
}

float extras::similarity(std::string s1, std::string s2)
{
    s2=removespaces(s2);
 
    std::set<char> set1(s1.begin(), s1.end());
    std::set<char> set2(s2.begin(), s2.end());

    std::set<char> intersection;
    std::set_intersection(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(intersection, intersection.begin()));
    std::set<char> UNION;
    std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(UNION, UNION.begin()));

     if (UNION.empty())
    {
        return 0.0f; 
    }

    return (static_cast<float>(intersection.size()) / static_cast<float>(UNION.size()));
}

std::optional<std::string> extras::match(std::string cmd, std::vector<std::string> &default_cmds, float min_req)
{
    std::optional<std::string> match;
    float max_found = 0.0f;

    for (auto &present : default_cmds)
    {
        float value = similarity(cmd, present);
        if (value >= max_found)
        {
            
            max_found = value;
            match = present;
        }
    }

    if (max_found >= min_req)
    {
       
        return match;
    }
    else
    {
        return std::nullopt;
    }
}

void extras::decision(std::vector<std::string>&input_tokens,std::vector<std::string>&av_cmds)
{

    if(input_tokens.empty())return;

    std::string cmd ;
    for(auto&tokens :input_tokens)
        cmd+=tokens;
    
    // it will not have spaces

    std::optional<std::string>result=match(cmd,av_cmds,0.5);
    
    if(result.has_value())
    {
        std::cout<<color::red <<"did you mean : "<<color::reset<<result.value()<<std::endl;
    }
    else
    {
        std::cout<<"No such command found!!"<<std::endl;
    }

    
}
