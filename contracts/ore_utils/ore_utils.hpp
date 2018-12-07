#pragma once

#include "eosiolib/transaction.hpp"
#include <string>

using namespace std;
using namespace eosio;

class ore_utils
{
  public:
    char* string_to_char(string str);
};

char* ore_utils::string_to_char(string str)
{
    char cstr[str.size() + 1];
    return copy(str.begin(), str.end(), cstr);
}