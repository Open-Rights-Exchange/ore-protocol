#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/time.hpp>
#include <chrono>

using namespace eosio;
using namespace std;

class [[eosio::contract("ore.usage_log")]] usage_log : public eosio::contract
{
  public:
    using contract::contract;
    usage_log(name receiver, name code, datastream<const char*> ds): contract(receiver, code, ds), _logs(receiver, receiver.value){}

    [[eosio::action]]
    void createlog(uint64_t instrument_id, string right_name, string token_hash, uint64_t timestamp)
    {
        uint64_t right_hash = hashStr(right_name);

        _logs.emplace(_self, [&](auto &a) {
            a.instrument_id = instrument_id;
            a.right_hash = right_hash;
            a.right_name = right_name;
            a.token_hash = hashStr(token_hash);
            a.timestamp = timestamp;
        });
    }

    // TODO: require authority of the reconciler
    // delets the entry from the log table with matching token_hash
    [[eosio::action]]
    void deletelog(uint64_t instrument_id, string token_hash)
    {
        auto itr = _logs.find(hashStr(token_hash));

        eosio_assert(itr != _logs.end(), "No log exist for the given pair or right and instrument");

        print("token hash", itr->token_hash);

        _logs.erase(itr);
    }

    [[eosio::action]]
    void updatecount(uint64_t instrument_id, string right_name, asset cpu)
    {
        uint64_t right_hash = hashStr(right_name);

        counts_table _counts(_self, instrument_id);

        auto itr = _counts.find(right_hash);

        if (itr == _counts.end())
        {
            _counts.emplace(_self, [&](auto &a) {
                a.right_hash = right_hash;
                a.right_name = right_name;
                a.last_usage_time = time_point_sec(now());
                a.total_count = 1;
                a.total_cpu = cpu;
            });
        }
        else
        {
            _counts.modify(itr, _self, [&](auto &a) {
                a.last_usage_time = time_point_sec(now());
                a.total_count += 1;
                a.total_cpu += cpu;
            });
        }
    }

  private:
    struct [[eosio::table]] log
    {
        uint64_t instrument_id;
        uint64_t right_hash;
        string right_name;
        bool action_type;
        uint64_t token_hash;
        uint64_t timestamp;
        auto primary_key() const { return token_hash; }
        EOSLIB_SERIALIZE(log, (instrument_id)(right_hash)(right_name)(action_type)(token_hash)(timestamp))
    };

    //following structure enables fast query of api call count for the verifier
    struct [[eosio::table]] callcount
    {
        uint64_t right_hash;
        string right_name;
        time_point_sec last_usage_time;
        uint64_t total_count;
        asset total_cpu;
        auto primary_key() const { return right_hash; }

        EOSLIB_SERIALIZE(callcount, (right_hash)(right_name)(last_usage_time)(total_count)(total_cpu))
    };
    
    typedef eosio::multi_index<"counts"_n, callcount> counts_table;
    typedef eosio::multi_index<"logs"_n, log> logs_table;

    logs_table _logs;

    uint64_t hashStr(const string &strkey)
    {
        return hash<string>{}(strkey);
    }
};
EOSIO_DISPATCH(usage_log, (createlog)(deletelog)(updatecount))
