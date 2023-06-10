#include <eosio/eosio.hpp>

using namespace eosio;

class [[eosio::contract]] pruebavapaee : public contract {
public:
  using contract::contract;

  [[eosio::action]]
  void init() {
    print("init()");
  }

  [[eosio::action]]
  void test() {
    print("test()");
  }
};
