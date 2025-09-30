# vapaee-smart-contracts
This is the set of smart contracts for Vapaée projects

# Vapaée Old Tokens
[https://github.com/vapaee/vapaee-smart-contracts/tree/ddeda1ec10b29b9481d5ead644be3f839d426e2c/contracts](https://github.com/vapaee/vapaee-smart-contracts/tree/ddeda1ec10b29b9481d5ead644be3f839d426e2c/contracts)

# Vapaée Contracts

En este documento se enumeran los contratos inteligentes implementados con Antelope para la blockchain TELOS nativa, con su rol, tareas y responsabilidades, así como sus acciones y handlers.

## 1. basictoken

**Rol y responsabilidades:** Contrato estándar de token que implementa las funcionalidades básicas de creación, emisión, retiro y transferencia de tokens. Sirve para contar con tokens standard con los cuales debemos interactuar pero estando en ambientes de testeo.

### Acciones

* `create(issuer, maximum_supply)`: Crea un nuevo token con suministro máximo especificado.  
* `issue(to, quantity, memo)`: Emite tokens a una cuenta.  
* `retire(quantity, memo)`: Retira tokens del suministro circulante.  
* `transfer(from, to, quantity, memo)`: Transfiere tokens entre cuentas.  
* `open(owner, symbol, ram_payer)`: Abre una cuenta para un símbolo token específico.  
* `close(owner, symbol)`: Cierra la cuenta de token si su balance es cero.  

## 2. koinonospool

**Rol y responsabilidades:** Implementación específica de un `converter` que maneja pools para la pareja KOINE/EUROT, quemando las comisiones al vender KOINE y redireccionando las comisiones como ganancias del sistema mediante el contrato `vapaeepayhub`.

### Acciones

* `init()`: Inicializa variables globales del contrato.  
* `cancelfund(funder, marketid)`: Cancela la participación de un fondo en un mercado.  
* `takepart(funder, marketid, score)`: Retira participación de un pool.  
* `selftransf(from, to, quantity, memo)`: Transfiere tokens internamente evitando un mensaje adicional para operaciones de swap.  

### Handlers

* `htransfer(from, to, quantity, memo) [[eosio::on_notify("*::transfer")]]`: Maneja eventos de transferencia para ejecutar swaps y quemar fees.  

## 3. telosbookdex

**Rol y responsabilidades:** Exchange de tipo order-book, maneja órdenes de compra/venta y depósitos.

### Acciones

* `init()`: Inicializa variables globales.  
* `cancel(owner, type, commodity, currency, orders)`: Cancela órdenes en el libro.  
* `order(owner, type, total, price, client)`: Crea una orden de compra o venta.  
* `withdraw(owner, quantity, client)`: Retira fondos depositados.  

### Handlers

* `htransfer(from, to, quantity, memo) [[eosio::on_notify("*::transfer")]]`: Gestiona depósitos automáticos cuando `memo == "deposit"`.  

## 4. telosmaindex

**Rol y responsabilidades:** DEX multi-módulo con gestión de clientes, tokens, mercados, swaps, registro de eventos y DAO.

### Acciones

* `init()`: Inicializa módulos globales de base y DEX.

#### Registering Actors
* `addclient(admin, receiver, params, title, website, brief, banner, thumbnail)`: Añade un nuevo cliente al DEX.  
* `updateclient(client, admin, receiver, params, title, website, brief, banner, thumbnail)`: Actualiza datos de un cliente.  
* `addtoken(contract, symbol, precision, admin, title, website, brief, banner, icon, iconlg, pcontact, gcontact, tradeable, stable)`: Registra un token en el DEX.  
* `updatetoken(sym_code, title, website, brief, banner, icon, iconlg, pcontact, gcontact, tradeable, stable)`: Actualiza info de un token.  
* `tokenadmin(sym_code, admin)`: Asigna administrador de token.  
* `settokendata(sym_code, id, action, text, link, shownas)`: Añade datos adicionales al token.  
* `addcurrency(sym_code, website, brief)`: Registra una moneda en el DEX.  
* `udpcurrency(currency_id, website, brief)`: Actualiza datos de una moneda.  
* `newmarket(commodity, currency, converter)`: Crea un nuevo mercado.

#### Exchange
* `regpoolswap(recipient, converter, rate, sent, result, fee)`: Registra intercambio en pool dex.  
* `updpoolswap(market, converter)`: Actualiza estado de intercambio en pool.  
* `regbookdeal(type, buyer, seller, price, inverse, payment, amount, buyfee, sellfee)`: Registra trato en book dex.  
* `history(buyer, seller, isbuy, price, inverse, amount, payment, buyfee, sellfee, date)`: Registra un historial de operaciones.

#### Maintenance
* `maintenance(credits_to)`: Ejecuta tareas de mantenimiento.  
* `reward(user, points, exp)`: Otorga experiencia y puntos.

#### DAO
* `balloton(operation, params, arguments, feepayer)`: Inicia una votación DAO.

#### Debug
* `updatenow()`: Acción de depuración para actualizar tiempo.  
* `testdao(ballotname, finalresults, totalvoters)`: Simula resultado DAO.  
* `timeoffset(offset)`: Ajusta offset de tiempo para pruebas.  
* `pause(value)`: Pausa o reanuda el contrato.  
* `hotfix()`: Acción vacía para aplicar fix.

### Handlers

* `hbroadcast(ballot_name, endresults, total_voters) [[eosio::on_notify("telos.decide::broadcast")]]`: Procesa resultados de DAO.  
* `htransfer(from, to, quantity, memo) [[eosio::on_notify("*::transfer")]]`: Maneja pagos para fees o swaps según `memo`.  

## 5. telospooldex

**Rol y responsabilidades:** Implementa un convertidor genérico con varios pools de diferentes locktime y APYs independientes. Sirve como convertidor genérico para la mayoría de markets TOKENA/TOKENB.

### Acciones

* `init()`: Inicializa variables globales del pool.  
* `cancelfund(funder, marketid)`: Cancela fondos.  
* `takepart(funder, marketid, score)`: Retira participación.  
* `selftransf(from, to, quantity, memo)`: Transferencia interna para swaps.

### Handlers

* `htransfer(from, to, quantity, memo) [[eosio::on_notify("*::transfer")]]`: Gestiona eventos de transferencia para ejecutar swaps.

## 6. vapaeepayhub

**Rol y responsabilidades:**  
Hub de pagos y servicios financieros descentralizados en la blockchain TELOS: gestiona staking (REX), pools de liquidez, pagos programados, facturación (invoices) y distribución de fondos entre PayHubs y sus receptores.

### Tablas

- **names**  
  Guarda los alias VIP asociados a cuentas Telos, usados como identificadores alternativos de PayHubs.
- **payhubs**  
  Define cada PayHub con su `id`, `alias`, administrador, destinatario de facturas (`billing_to`) y lista de receptores (cada uno con su parte porcentual).
- **paypockets**  
  Registra los saldos por token de cada PayHub, con marcas temporales de última entrada y salida para permitir auditoría y distribución.
- **payments**  
  Cola de pagos programados; cada entrada contiene `quantity`, un destino (`payhub_target`), un `memo` y timestamp de creación.
- **invoices**  
  Plantillas de facturación por token: para cada `id` (p.ej. `"main"`) fija un importe fijo, un porcentaje y el PayHub receptor de las comisiones.
- **leakpools**  
  Pools de liquidez con distribución gradual (“leak”) de un activo a lo largo de un periodo, ligados a un PayHub receptor.
- **stakeconfigs**  
  Configuración global de staking por token: categorías disponibles, tiempos de bloqueo, y estadísticas agregadas de cuánto se está apostando.
- **stakepools**  
  Parámetros y estado de cada pool de staking específico: locktime, título, totales apostados (`pool_stake`), fondos reales (`pool_funds`) y REX emitidos.
- **mystake**  
  Estado global de staking de un usuario por token: total apostado, total maduro disponible y asignación de créditos.
- **mypoolstake**  
  Estado de la participación de un usuario en un pool concreto: cuántos tokens tiene apostados, cuántos han madurado, cuántos REX posee y sus “slots” de maduración.
- **stakers**  
  Lista de cuentas que participan en staking de cada token, utilizada para indexar rápidamente a los apostadores.

### Acciones

#### Global
- `init()`  
  Inicializa variables globales del contrato (no modifica tablas de pago).

#### Pool Creation
- `stakeconfig(action, admin, token, contract, title, categories, dayslock)`  
  Crea o actualiza la configuración de staking de un token.  
  **Tablas afectadas:** `stakeconfigs` (inserta o modifica la entrada para ese `token`).
- `stakepool(action, token, poollabel, title, locktime)`  
  Crea o actualiza un pool de staking para un token.  
  **Tablas afectadas:** `stakepools` (inserta o modifica la entrada `token`+`poollabel`).

#### Pool Funding
- `droponpool(quantity, poollabel)`  
  Agrega liquidez externa a un pool existente. 
  **Tablas afectadas:**  
  - `stakeconfigs.total_funds` (incrementa)  
  - `stakepools.pool_funds` (incrementa)  
  - `stakepools.history` (registra snapshot de `pool_funds`)

#### Stake, Credits and Profits
- `stake(owner, quantity, poollabel)`  
  El usuario apuesta tokens en un pool: debita activos, emite REX y bloquea según locktime.  
  **Tablas afectadas:**  
  - `stakeconfigs.total_stake`, `stakeconfigs.total_funds` (incrementan)  
  - `stakepools.pool_stake`, `pool_funds`, `pool_rex` (incrementan)  
  - `mystake` (inserta/modifica `total_stake` y `total_mature`)  
  - `mypoolstake` (inserta/modifica `stake`, `rex`, `maturing`)  
  - `stakers` (registra al `owner` si es nuevo apostador)
- `unstake(owner, quantity, poollabel, credits)`  
  Retira tokens apostados y REX correspondientes; devuelve los fondos al usuario.  
  **Tablas afectadas:**  
  - `stakeconfigs.total_stake`, `total_funds` (decrementan)  
  - `stakepools.pool_stake`, `pool_funds`, `pool_rex` (decrementan)  
  - `mystake.total_stake`, `total_mature` (decrementan)  
  - `mypoolstake.stake`, `mature`, `rex` (decrementan)  
  - `mystake.credits`, `credits_update` (si se pasan `credits`)
- `takeprofits(owner, token, poollabel)`  
  Permite al usuario retirar solo las ganancias generadas (diferencia de REX) sin tocar el stake base.  
  **Tablas afectadas:**  
  - `stakeconfigs.total_funds` (decrementa)  
  - `stakepools.pool_funds`, `pool_rex` (decrementan)  
  - `mypoolstake.rex` (decrementa)
- `updtstake(owner, token, poollabel)`  
  Libera los montos apostados que han alcanzado su fecha de maduración.  
  **Tablas afectadas:**  
  - `mystake.total_mature` (incrementa)  
  - `mypoolstake.mature` (incrementa) y `mypoolstake.maturing` (filtra slots)
- `resetmslot(owner, token, poollabel, slotindex)`  
  Fusiona o elimina un slot de maduración específico en la participación del usuario.  
  **Tablas afectadas:** `mypoolstake` (ajusta vector `maturing`)
- `buymslot(owner, token, poollabel, slots)`  
  Incrementa el número de slots de maduración disponibles para el usuario, cobrando un coste.  
  **Tablas afectadas:** `mypoolstake.maturing_slots`
- `mycredits(owner, credits)`  
  Asigna la distribución de créditos de experiencia entre categorías configuradas.  
  **Tablas afectadas:** `mystake.credits` y `mystake.credits_update`

#### PayHub Node
- `newpayhub(admin, vipname, recipients, pockets, billingto)`  
  Registra un nuevo PayHub con alias, receptores y billeteras. Abre pockets iniciales.  
  **Tablas afectadas:**  
  - `names` (lectura/creación de VIP alias)  
  - `payhubs` (inserta nueva entrada)  
  - `paypockets` (crea filas con `balance=0` para cada token en `pockets`)
- `updatehub(admin, payhub_id, recipients, pockets, billingto)`  
  Modifica la configuración de un PayHub existente y abre nuevos pockets si es necesario.  
  **Tablas afectadas:**  
  - `payhubs` (modifica `recipients` y `billing_to`)  
  - `paypockets` (abre nuevos pockets)
- `newname(owner, vname)`  
  Registra un alias VIP para una cuenta.  
  **Tablas afectadas:** `names` (inserta o actualiza asociación `alias`→`owner`)

- `update(helper)`  
  Acción de mantenimiento que puede ser ejecutada por cualquiera (helper). Procesa un pago pendiente en la cola de `payments`, moviéndolo a la distribución real.  
  **Tablas afectadas:**  
  - `payments` (elimina la primera entrada)  
  - `paypockets` (ajusta saldos y dispara transferencias)
- `movepocket(target, signer, memo)`  
  Ejecuta inmediatamente la distribución de un pocket: mueve fondos a los receptores según sus partes.
  La función `update` la ejecutará eventualmente cuando encuentre un pocket sin procesar
  **Tablas afectadas:** `paypockets` (decrementa saldos)
- `movepayment(target, token, signer)`  
  Ejecuta un pago programado específico: consume la entrada y distribuye los fondos.
  La función `update` la ejecutará eventualmente cuando encuentre un payment sin procesar
  **Tablas afectadas:**  
  - `payments` (elimina la entrada)  
  - `paypockets` o `stakepools` (según destino)

- `pay(quantity, target, memo, move)`  
  Programa un pago futuro añadiéndolo a la cola de `payments`.  
  **Tablas afectadas:** `payments` (inserta nueva entrada)

- `newleakpool(admin, payhub_id, token, title, liquidity, issueallaw, easing, epochstart, epochend)`  
  Crea un leakpool para distribuir gradualmente activos a un PayHub.  
  **Tablas afectadas:** `leakpools` (inserta nueva entrada)
- `udpleakpool(admin, leakpoolid, title, issuemore)`  
  (No testeada) Actualiza parámetros y cronograma de un leakpool existente.  
  **Tablas afectadas:** `leakpools` (modifica `title`, `allowed`, `total`, `end`)
- `leakpool(leakpoolid, random)`
  Realiza un ciclo de “leak”: calcula delta, actualiza estado y envía activos al PayHub receptor. Esta acción puede ser ejecutada por cualquiera en cualquier momento.
  **Tablas afectadas:**  
  - `leakpools` (`leaked`, `left`, `delta`, `last_leak`)  
  - `paypockets` (incrementa saldos del receptor)

- `billing(admin, invname, token, fixed, percent, payhub)`  
  Crea o actualiza la plantilla de factura de un token.  
  **Tablas afectadas:** `invoices` (inserta o modifica la entrada `invname`)
- `invoice(payer, seller, collector, quantity, fee, fiat, memo)`  
  Emite notificaciones de factura a los participantes; no modifica tablas.
  **Tablas afectadas:** ninguna








### Handlers

* `htransfer(from, to, quantity, memo) [[eosio::on_notify("*::transfer")]]`: Maneja transferencias para procesar pagos.

## 7. vapaeetokens

**Rol y responsabilidades:** Contrato de tokens avanzado con módulos de emisión, envoltura, débito y manejadores de transferencia.

### Acciones

* `init()`  
* `create(issuer, maximum_supply)`  
* `issue(to, quantity, memo)`  
* `retire(quantity, memo)`  
* `transfer(from, to, quantity, memo)`  
* `open(owner, symbol, ram_payer)`  
* `close(owner, symbol)`  
* `chissuer(new_issuer, token)`  
* `issuer(action, theissuer, max)`  
* `deposit(owner, quantity, tokcontract, memo)`  
* `withdraw(owner, quantity, notes)`  
* `debit(from, collector, quantity, memo)`  
* `allowance(owner, collector, quantity)`

### Handlers

* `handle_transfer(from, to, quantity, memo) [[eosio::on_notify("*::transfer")]]`: Gestiona depósitos y desvinculaciones automáticas.
