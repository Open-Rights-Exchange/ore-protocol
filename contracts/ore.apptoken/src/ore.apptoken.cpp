#include "../include/ore.apptoken.hpp"

using namespace eosio;

ACTION apptoken::create( const name&   issuer,
                    const asset&  maximum_supply )
{
    require_auth( name("system.ore"));

    auto sym = maximum_supply.symbol;
    check( sym.is_valid(), "invalid symbol name [apptoken.create]" );
    check( maximum_supply.is_valid(), "invalid supply [apptoken.create]");
    check( maximum_supply.amount > 0, "max-supply must be positive [apptoken.create]");

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing == statstable.end(), "token with symbol already exists [apptoken.create]" );

    statstable.emplace( get_self(), [&]( auto& s ) {
       s.supply.symbol = maximum_supply.symbol;
       s.max_supply    = maximum_supply;
       s.issuer        = issuer;
    });
}

ACTION apptoken::issue( const name& to, const asset& quantity, const string& memo )
{
    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name [apptoken.issue]" );
    check( memo.size() <= 256, "memo has more than 256 bytes [apptoken.issue]" );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing != statstable.end(), "token with symbol does not exist, create token before issue [apptoken.issue]" );
    const auto& st = *existing;
    check( to == st.issuer, "tokens can only be issued to issuer account [apptoken.issue]" );

    require_auth( st.issuer );
    check( quantity.is_valid(), "invalid quantity [apptoken.issue]" );
    check( quantity.amount > 0, "must issue positive quantity [apptoken.issue]" );

    check( quantity.symbol == st.supply.symbol, "symbol precision mismatch [apptoken.issue]" );
    check( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply [apptoken.issue]");

    statstable.modify( st, same_payer, [&]( auto& s ) {
       s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );
}

ACTION apptoken::retire( const asset& quantity, const string& memo )
{
    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name [apptoken.retire]" );
    check( memo.size() <= 256, "memo has more than 256 bytes [apptoken.retire]" );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing != statstable.end(), "token with symbol does not exist [apptoken.retire]" );
    const auto& st = *existing;

    require_auth( st.issuer );
    check( quantity.is_valid(), "invalid quantity [apptoken.retire]" );
    check( quantity.amount > 0, "must retire positive quantity [apptoken.retire]" );

    check( quantity.symbol == st.supply.symbol, "symbol precision mismatch [apptoken.retire]" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
       s.supply -= quantity;
    });

    sub_balance( st.issuer, quantity );
}

ACTION apptoken::transfer( const name&    from,
                      const name&    to,
                      const asset&   quantity,
                      const string&  memo )
{
    check( from != to, "cannot transfer to self [apptoken.transfer]" );
    require_auth( from );
    check( is_account( to ), "to account does not exist [apptoken.transfer]");
    auto sym = quantity.symbol.code();
    stats statstable( get_self(), sym.raw() );
    const auto& st = statstable.get( sym.raw() );

    require_recipient( from );
    require_recipient( to );

    check( quantity.is_valid(), "invalid quantity [apptoken.transfer]" );
    check( quantity.amount > 0, "must transfer positive quantity [apptoken.transfer]" );
    check( quantity.symbol == st.supply.symbol, "symbol precision mismatch [apptoken.transfer]" );
    check( memo.size() <= 256, "memo has more than 256 bytes [apptoken.transfer]" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

void apptoken::sub_balance( const name& owner, const asset& value ) {
   accounts from_acnts( get_self(), owner.value );

   const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found [apptoken.sub_balance]" );
   check( from.balance.amount >= value.amount, "overdrawn balance [apptoken.sub_balance]" );

   from_acnts.modify( from, owner, [&]( auto& a ) {
         a.balance -= value;
      });
}

void apptoken::add_balance( const name& owner, const asset& value, const name& ram_payer )
{
   accounts to_acnts( get_self(), owner.value );
   auto to = to_acnts.find( value.symbol.code().raw() );
   if( to == to_acnts.end() ) {
      to_acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = value;
      });
   } else {
      to_acnts.modify( to, same_payer, [&]( auto& a ) {
        a.balance += value;
      });
   }
}

ACTION apptoken::open( const name& owner, const symbol& symbol, const name& ram_payer )
{
   require_auth( ram_payer );

   check( is_account( owner ), "owner account does not exist [apptoken.open]" );

   auto sym_code_raw = symbol.code().raw();
   stats statstable( get_self(), sym_code_raw );
   const auto& st = statstable.get( sym_code_raw, "symbol does not exist [apptoken.open]" );
   check( st.supply.symbol == symbol, "symbol precision mismatch [apptoken.open]" );

   accounts acnts( get_self(), owner.value );
   auto it = acnts.find( sym_code_raw );
   if( it == acnts.end() ) {
      acnts.emplace( ram_payer, [&]( auto& a ){
        a.balance = asset{0, symbol};
      });
   }
}

ACTION apptoken::close( const name& owner, const symbol& symbol )
{
   require_auth( owner );
   accounts acnts( get_self(), owner.value );
   auto it = acnts.find( symbol.code().raw() );
   check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect. [apptoken.close]" );
   check( it->balance.amount == 0, "Cannot close because the balance is not zero. [apptoken.close]" );
   acnts.erase( it );
}


EOSIO_DISPATCH(apptoken, (create)(issue)(transfer)(open)(close)(retire))