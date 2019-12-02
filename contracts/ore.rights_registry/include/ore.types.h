#pragma once

#include "eosiolib/transaction.hpp"
#include <string>

using namespace std;
using namespace eosio;

namespace oretypes {
    struct args
    {
        string name;
        string value;
    };

    // this struct is used to specify parameter_rules in an instrument
    // for type such as required and locked, there is no value. Hence pass in an empty string as value.
    struct param_type
    {
        string type;
        vector<args> values;
    };

    struct endpoint_url
    {
        string base_right;
        string url;
        string method;
        vector<args> matches_params;
        uint64_t token_life_span;
        bool is_default;
    };

    struct params
    {
        vector<args> params;
    };

    struct url_params
    {
        vector<params> url_params;
    };

    //right in instrument
    struct right
    {
        string right_name;
        string description;
        string price_in_cpu;
        vector<params> additional_url_params;
    };

    //right in right registry
    struct right_param
    {
        string right_name;
        vector<endpoint_url> urls;
        vector<name> whitelist;
    };

    struct api_voucher_params
    {
        string right_name;
        params additional_url_params;
    };

    struct offer_params
    {
        string right_name;
        string right_description;
        string right_price_in_cpu;
        string api_name;
        string api_description;
        string api_price_in_cpu;
        string api_payment_model;
        string api_additional_url_params;
    };
}