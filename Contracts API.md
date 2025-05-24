## acorntwitter

**Acciones**

* `init()`
  Inicializa variables globales y prepara el estado del módulo atwitter.

**Handlers**

* `handle_transfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Atiende notificaciones de transferencias para procesar eventos de Twitter (módulo atwitter).

---

## basictoken

**Acciones**

* `init()`
  Inicializa el contrato y registra la tabla de estadísticas.
* `create(const name& issuer, const asset& maximum_supply)`
  Crea un nuevo token con un suministro máximo definido.
* `issue(const name& to, const asset& quantity, const string& memo)`
  Emite tokens a una cuenta.
* `retire(const asset& quantity, const string& memo)`
  Retira tokens del suministro circulante.
* `transfer(const name& from, const name& to, const asset& quantity, const string& memo)`
  Transfiere tokens entre dos cuentas.
* `open(const name& owner, const symbol& symbol, const name& ram_payer)`
  Crea la cuenta de balance para un propietario y símbolo.
* `close(const name& owner, const symbol& symbol)`
  Cierra la cuenta de token si su balance es cero.

**Funciones estáticas de consulta**

* `get_supply(const name& token_contract_account, const symbol_code& sym_code)`
  Devuelve el suministro total actual del token.
* `get_balance(const name& token_contract_account, const name& owner, const symbol_code& sym_code)`
  Devuelve el balance de un propietario para un token dado.

---

## koinonospool

**Acciones**

* `init()`
  Inicializa variables globales del contrato.
* `cancelfund(name funder, uint64_t marketid)`
  Cancela la participación de un fondo en un mercado.
* `takepart(name funder, uint64_t marketid, asset score)`
  Retira la participación de un fondo del pool.
* `selftransf(name from, name to, asset quantity, string memo)`
  Realiza una transferencia interna (swap) sin emitir evento adicional.

**Handlers**

* `htransfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Gestiona eventos de transferencia entrantes para ejecutar swaps y quemar comisiones.

---

## pruebavapaee

**Acciones**

* `init()`
  Inicializa el contrato usando el módulo global.
* `test(name a)`
  Acción de prueba que simplemente imprime un log.

---

## telosbookdex

**Acciones**

* `init()`
  Inicializa variables globales y tablas del libro de órdenes.
* `cancel(name owner, name type, const symbol_code& commodity, const symbol_code& currency, const vector<uint64_t>& orders)`
  Cancela órdenes específicas en el order-book.
* `order(name owner, name type, const asset& total, const asset& price, uint64_t client)`
  Crea una orden de compra o venta.
* `withdraw(name owner, const asset& quantity, uint64_t client)`
  Retira fondos depositados en el DEX.

**Handlers**

* `htransfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Detecta transfers con memo `"deposit"` y registra depósitos automáticamente.

---

## telosmaindex

**Acciones**

* `init()`
  Inicializa módulos globales y del DEX.
* **Módulo de clientes**

  * `addclient(name admin, name receiver, string params, string title, string website, string brief, string banner, string thumbnail)`
  * `updateclient(uint64_t client, name admin, name receiver, string params, string title, string website, string brief, string banner, string thumbnail)`
* **Módulo de tokens/mercados**

  * `addtoken(name contract, const symbol_code& symbol, uint8_t precision, name admin, string title, string website, string brief, string banner, string icon, string iconlg, string pcontact, string gcontact, bool tradeable, bool stable`
  * `updatetoken(const symbol_code & sym_code, string title, string website, string brief, string banner, string icon, string iconlg, string pcontact, string gcontact, bool tradeable, bool stable)`
  * `tokenadmin(const symbol_code& sym_code, name admin)`
  * `settokendata(const symbol_code& sym_code, uint64_t id, name action, string text, string link, name shownas)`
  * `addcurrency(const symbol_code& sym_code, string website, string brief)`
  * `udpcurrency(uint64_t currency_id, string website, string brief)`
  * `newmarket(const symbol_code& commodity, const symbol_code& currency, name converter)`
* **Registro de historial**

  * `history(name buyer, name seller, bool isbuy, asset price, asset inverse, asset amount, asset payment, asset buyfee, asset sellfee, time_point_sec date)`
* **Módulo de experiencia**

  * `reward(name user, const asset& points, const asset& exp)`
* **Registro de swaps/deals**

  * `regpoolswap(name recipient, name converter, asset rate, asset sent, asset result, asset fee)`
  * `updpoolswap(uint64_t market, name converter)`
  * `regbookdeal(name type, name buyer, name seller, asset price, asset inverse, asset payment, asset amount, asset buyfee, asset sellfee)`
* **Mantenimiento y DAO**

  * `maintenance(name credits_to)`
  * `balloton(name operation, vector<string> params, string arguments, name feepayer)`
* **Acciones de depuración**

  * `updatenow()`
  * `testdao(name ballotname, map<name, asset> finalresults, uint32_t totalvoters)`
  * `timeoffset(uint32_t offset)`
  * `pause(bool value)`
  * `hotfix()`

**Handlers**

* `hbroadcast(name ballot_name, map<name, asset> endresults, uint32_t total_voters)`
  \[\[`eosio::on_notify("telos.decide::broadcast")`]]
  Procesa resultados de votaciones DAO.
* `htransfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Diferencia entre pago de fees y swaps para encaminar la lógica correspondiente.

---

## telospooldex

**Acciones**

* `init()`
  Inicializa el contrato y tablas de pools.
* `cancelfund(name funder, uint64_t marketid)`
  Cancela la aportación de un fondo.
* `takepart(name funder, uint64_t marketid, asset score)`
  Retira participación de un pool.
* `selftransf(name from, name to, asset quantity, string memo)`
  Ejecuta swaps internos autorizados por el propio contrato.

**Handlers**

* `htransfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Intercepta transfers externos y ejecuta la lógica de swap.

---

## trngconsumer

**Acciones**

* `init()`
  Inicializa tablas de solicitudes de RNG.
* `requestrand(uint64_t seed, const name& caller, uint64_t min, uint64_t max)`
  Envía una solicitud de valor aleatorio al oráculo.
* `recordrand(uint64_t request_id, name caller, uint64_t number)`
  Guarda el número aleatorio recibido.
* `delrequest(uint64_t request_id)`
  Elimina una solicitud pendiente.

**Handler**

* `receiverand(uint64_t caller_id, checksum256 random)`
  \[\[`eosio::on_notify("rng.oracle::submitrand")`]]
  Recibe la notificación del oráculo con el valor aleatorio generado.

---

## vapaeepayhub

**Acciones**

* `init()`
  Inicializa módulos globales y tablas de pago.
* **Configuración de staking**

  * `stakeconfig(name action, name admin, symbol_code token, name contract, string title, vector<name> categories, string dayslock)`
  * `stakepool(name action, symbol_code token, name poollabel, string title, string locktime)`
* **Operaciones de pool**

  * `droponpool(asset quantity, name poollabel)`
  * `stake(name owner, asset quantity, name poollabel)`
  * `unstake(name owner, asset quantity, name poollabel, vector<tuple<name, asset>> credits)`
  * `takeprofits(name owner, symbol_code token, name poollabel)`
  * `updtstake(name owner, symbol_code token, name poollabel)`
  * `mycredits(name owner, vector<tuple<name, asset>> credits)`
* **Gestión de PayHubs**

  * `newpayhub(name admin, string vipname, vector<tuple<asset,string>> recipients, vector<symbol_code> pockets, name billingto)`
  * `update(name helper)`
  * `updatehub(name admin, uint64_t payhub_id, vector<tuple<asset,string>> recipients, vector<symbol_code> pockets, name billingto)`
  * `newname(name owner, string vname)`
* **Pagos y pockets**

  * `movepocket(string target, name signer, string memo)`
  * `pay(asset quantity, string target, string memo, bool move)`
  * `movepayment(string target, symbol_code token, name signer)`
* **Leak pools**

  * `newleakpool(name admin, uint64_t target, symbol_code token, string title, asset liquidity, asset issueallaw, name easing, uint32_t epochstart, uint32_t epochend)`
  * `udpleakpool(name admin, uint64_t leakpoolid, string title, asset issuemore)`
  * `leakpool(uint64_t leakpoolid, uint64_t random)`
* **Facturación**

  * `billing(name admin, name invname, symbol_code token, asset fixed, double percent, string payhub)`
  * `invoice(const name& payer, const name& seller, const name& collector, const asset& quantity, const asset& fee, const asset& fiat, const string& memo)`
* **Slots de maduración**

  * `resetmslot(name owner, symbol_code token, name poollabel, int slotindex)`
  * `buymslot(name owner, symbol_code token, name poollabel, int slots)`

**Handlers**

* `htransfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Encaminamiento genérico de transfers hacia la lógica de pagos y distribuciones.

---

## vapaeetokens

**Acciones**

* `init()`
  Inicializa tablas y módulo global.
* **Módulo estándar de token**

  * `create(const name& issuer, const asset& maximum_supply)`
  * `issue(const name& to, const asset& quantity, const string& memo)`
  * `retire(const asset& quantity, const string& memo)`
  * `transfer(const name& from, const name& to, const asset& quantity, const string& memo)`
  * `open(const name& owner, const symbol& symbol, const name& ram_payer)`
  * `close(const name& owner, const symbol& symbol)`
* **Módulo de emisión**

  * `chissuer(const name& new_issuer, const symbol_code& token)`
  * `issuer(const name& action, const name& theissuer, const asset& max)`
* **Módulo de envoltura (wrapper)**

  * `deposit(const name& owner, const asset& quantity, const name& tokcontract, const string memo)`
  * `withdraw(const name& owner, const asset& quantity, const string& notes)`
* **Módulo de débito**

  * `debit(name from, name collector, asset quantity, string memo)`
  * `allowance(name owner, name collector, asset quantity)`

**Handler**

* `handle_transfer(name from, name to, asset quantity, string memo)`
  \[\[`eosio::on_notify("*::transfer")`]]
  Gestiona depósitos, devoluciones o débito automático según el memo.

