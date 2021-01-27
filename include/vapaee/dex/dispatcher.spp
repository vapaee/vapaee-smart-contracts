// helpper macros -----------------
#define HANDLER void 

#define EOSIO_DISPATCH_DEX( TYPE, MEMBERS, HANDLERS ) \
extern "C" { \
   [[eosio::wasm_entry]] \
   void apply( uint64_t receiver, uint64_t code, uint64_t action ) { \
      if( code == receiver ) { \
         switch( action ) { \
            EOSIO_DISPATCH_HELPER( TYPE, MEMBERS ) \
         } \
      } \
      name handler = eosio::name(string("h") + eosio::name(action).to_string()); \
      switch( handler.value ) { \
         EOSIO_DISPATCH_HELPER( TYPE, HANDLERS ) \
      } \
   } \
} \
