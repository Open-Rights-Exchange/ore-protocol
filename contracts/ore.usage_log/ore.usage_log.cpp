#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <chrono>

using namespace eosio;
using namespace std;

class usage_log : public eosio::contract
{
  public:
    usage_log(account_name self) : eosio::contract(self) {}

    //@abi action
    void createlog(uint64_t instrument_id, string right_name, string token_hash, uint64_t timestamp)
    {

        logs_table _logs(_self, _self);

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
    // @abi action
    void deletelog(uint64_t instrument_id, string token_hash)
    {
        logs_table _logs(_self, _self);

        auto itr = _logs.find(hashStr(token_hash));

        eosio_assert(itr != _logs.end(), "No log exist for the given pair or right and instrument");

        print("token hash", itr->token_hash);

        _logs.erase(itr);
    }

    //@abi action
    void updatecount(uint64_t instrument_id, string right_name, asset cpu)
    {
        uint64_t right_hash = hashStr(right_name);

        //Test the scoping

        counts_table _counts(_self, instrument_id);

        auto itr = _counts.find(right_hash);

        if (itr == _counts.end())
        {
            _counts.emplace(_self, [&](auto &a) {
                a.right_hash = right_hash;
                a.right_name = right_name;
                a.last_usage_time = now();
                a.total_count = 1;
                a.total_cpu = cpu;
            });
        }
        else
        {
            _counts.modify(itr, _self, [&](auto &a) {
                a.last_usage_time = now();
                a.total_count += 1;
                a.total_cpu += cpu;
            });
        }
    }

  private:
    //@abi table logs i64
    struct log
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
    //@abi table counts i64
    struct callcount
    {
        uint64_t right_hash;
        string right_name;
        time last_usage_time;
        uint64_t total_count;
        asset total_cpu;
        auto primary_key() const { return right_hash; }

        EOSLIB_SERIALIZE(callcount, (right_hash)(right_name)(last_usage_time)(total_count)(total_cpu))
    };

    typedef eosio::multi_index<N(counts), callcount> counts_table;
    typedef eosio::multi_index<N(logs), log> logs_table;

    uint64_t hashStr(const string &strkey)
    {
        return hash<string>{}(strkey);
    }
};
EOSIO_ABI(usage_log, (createlog)(deletelog)(updatecount))
