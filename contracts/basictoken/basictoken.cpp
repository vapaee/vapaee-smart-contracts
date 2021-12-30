#include "basictoken.hpp"




void basictoken::create( const name&   issuer,
                const asset&  maximum_supply )
{
    require_auth( get_self() );

    auto sym = maximum_supply.symbol;
    check( sym.is_valid(), "invalid symbol name (basictoken)" );
    check( maximum_supply.is_valid(), "invalid supply (basictoken)");
    check( maximum_supply.amount > 0, "max-supply must be positive (basictoken)");

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing == statstable.end(), (string("token with symbol already exists (basictoken): ") + sym.code().to_string()).c_str() );

    statstable.emplace( get_self(), [&]( auto& s ) {
        s.supply.symbol = maximum_supply.symbol;
        s.max_supply    = maximum_supply;
        s.issuer        = issuer;
    });
}


void basictoken::issue( const name& to, const asset& quantity, const string& memo )
{
    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name (basictoken)" );
    check( memo.size() <= 256, "memo has more than 256 bytes (basictoken)" );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing != statstable.end(), "token with symbol does not exist, create token before issue (basictoken)" );
    const auto& st = *existing;
    check( to == st.issuer, "tokens can only be issued to issuer account (basictoken)" );

    require_auth( st.issuer );
    check( quantity.is_valid(), "invalid quantity (basictoken)" );
    check( quantity.amount > 0, "must issue positive quantity (basictoken)" );

    check( quantity.symbol == st.supply.symbol, "symbol precision mismatch (basictoken)" );
    check( quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply (basictoken)");

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply += quantity;
    });

    add_balance( st.issuer, quantity, st.issuer );
}

void basictoken::retire( const asset& quantity, const string& memo )
{
    auto sym = quantity.symbol;
    check( sym.is_valid(), "invalid symbol name (basictoken)" );
    check( memo.size() <= 256, "memo has more than 256 bytes (basictoken)" );

    stats statstable( get_self(), sym.code().raw() );
    auto existing = statstable.find( sym.code().raw() );
    check( existing != statstable.end(), "token with symbol does not exist (basictoken)" );
    const auto& st = *existing;

    require_auth( st.issuer );
    check( quantity.is_valid(), "invalid quantity (basictoken)" );
    check( quantity.amount > 0, "must retire positive quantity (basictoken)" );

    check( quantity.symbol == st.supply.symbol, "symbol precision mismatch (basictoken)" );

    statstable.modify( st, same_payer, [&]( auto& s ) {
        s.supply -= quantity;
    });

    sub_balance( st.issuer, quantity );
}

void basictoken::transfer( const name&    from,
                      const name&    to,
                      const asset&   quantity,
                      const string&  memo )
{

    PRINT("--> ACTION(",get_self().to_string(),"::transfer) <-- from:", from.to_string(), " to:", to.to_string(), " quantity:", quantity.to_string(), " memo:", memo,
        quantity.amount <= 0 ? " ERROR: quantity.amount <= 0" : ""
    );
    check( from != to, "cannot transfer to self (basictoken)" );
    require_auth( from );
    check( is_account( to ), "to account does not exist (basictoken)");
    auto sym = quantity.symbol.code();
    stats statstable( get_self(), sym.raw() );
    const auto& st = statstable.get( sym.raw() );

    require_recipient( from );
    require_recipient( to );

    check( quantity.is_valid(), "invalid quantity (basictoken)" );
    // check( quantity.amount > 0, 
    //     vapaee::dex::error::create_error_asset1(
    //         "must transfer positive quantity (basictoken)",
    //         quantity
    //     )        
    // );

    check( quantity.symbol == st.supply.symbol, "symbol precision mismatch (basictoken)" );
    check( memo.size() <= 256, "memo has more than 256 bytes (basictoken)" );

    auto payer = has_auth( to ) ? to : from;

    sub_balance( from, quantity );
    add_balance( to, quantity, payer );
}

void basictoken::sub_balance( const name& owner, const asset& value ) {
    accounts from_acnts( get_self(), owner.value );

    const auto& from = from_acnts.get( value.symbol.code().raw(), "no balance object found (basictoken)" );
    check( from.balance.amount >= value.amount, "overdrawn balance (basictoken)" );

    from_acnts.modify( from, owner, [&]( auto& a ) {
        a.balance -= value;
    });
}

void basictoken::add_balance( const name& owner, const asset& value, const name& ram_payer )
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

void basictoken::open( const name& owner, const symbol& symbol, const name& ram_payer )
{
    require_auth( ram_payer );
 
    check( is_account( owner ), "owner account does not exist (basictoken)" );
 
    auto sym_code_raw = symbol.code().raw();
    stats statstable( get_self(), sym_code_raw );
    const auto& st = statstable.get( sym_code_raw, "symbol does not exist (basictoken)" );
    check( st.supply.symbol == symbol, "symbol precision mismatch (basictoken)" );
 
    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( sym_code_raw );
    if( it == acnts.end() ) {
        acnts.emplace( ram_payer, [&]( auto& a ){
            a.balance = asset{0, symbol};
        });
    }
}

void basictoken::close( const name& owner, const symbol& symbol )
{
    require_auth( owner );
    accounts acnts( get_self(), owner.value );
    auto it = acnts.find( symbol.code().raw() );
    check( it != acnts.end(), "Balance row already deleted or never existed. Action won't have any effect. (basictoken)" );
    check( it->balance.amount == 0, "Cannot close because the balance is not zero. (basictoken)" );
    acnts.erase( it );
}