#pragma once
#include <vapaee/base/base.hpp>
#include <vapaee/pay/tables.hpp>
#include <vapaee/pay/modules/hub.hpp>


namespace vapaee {
    namespace pay {
        namespace billing {

            inline name get_self() {
                return vapaee::pay::contract;
            }

            bool get_invoice (
                bool create,
                invoices_table& inv,
                const name& ram_payer,
                const char * error_code
            ) {
                // vapaee::pay::billing::get_invoice
                invoices invoice_tb(get_self(), inv.fixed.symbol.code().raw());
                auto ptr = invoice_tb.find(inv.id.value);

                if (ptr == invoice_tb.end() && !create)  {
                    if (error_code) {
                        check(false, 
                            create_error_string1(
                                string(string(error_code) + ": the entry for the invoice was not found. (invoice): ").c_str(),
                                inv.id.to_string()
                            ).c_str()
                        );
                    } else {
                        return false;
                    }
                }

                if (ptr == invoice_tb.end() && create) {
                    invoice_tb.emplace(ram_payer, [&](auto &a){
                        a.id      = inv.id;
                        a.fixed   = inv.fixed;
                        a.percent = inv.percent;
                        a.payhub  = inv.payhub;
                    });

                    return true;
                };

                if (ptr != invoice_tb.end() && create) {
                    invoice_tb.modify(ptr, ram_payer, [&](auto &a){
                        a.fixed   = inv.fixed;
                        a.percent = inv.percent;
                        a.payhub  = inv.payhub;
                    });

                    return true;
                };
                
                inv.id           = ptr->id;
                inv.fixed        = ptr->fixed;
                inv.percent      = ptr->percent;
                inv.payhub       = ptr->payhub;
                return true;
            }

            // creates of mofiifies an invoice template for a token
            void action_billing(
                const name& admin,
                const name& invoice_name,
                const symbol_code& token,
                const asset& fixed,
                const double percent,
                const string& target
            ) {
                PRINT("vapaee::pay::billing::action_newinvoice()\n");
                PRINT(" admin: ", admin.to_string(), "\n");
                PRINT(" invoice_name: ", invoice_name.to_string(), "\n");
                PRINT(" token: ", token.to_string(), "\n");
                PRINT(" fixed: ", fixed.to_string(), "\n");
                PRINT(" percent: ", std::to_string(percent), "\n");
                PRINT(" target: ", target.c_str(), "\n");

                require_auth(admin);
                name ram_payer = admin;

                check(token == fixed.symbol.code(), "ERR-ABL-01: the token must be the same as the fixed asset");

                payhub_target pay_target;
                vapaee::pay::hub::parse_payhub_target(target, pay_target);
                PRINT(" pay_target: ", pay_target.to_string(), "\n");
                check(pay_target.payhub != MAX_VALUE,
                    create_error_string1(
                        "ERR-ABL-02: the target must be a valid payhub",
                        target
                    ).c_str()
                );

                // check it is not registered yet
                invoices_table inv;
                inv.id      = invoice_name;
                inv.fixed   = fixed;
                inv.percent = percent;
                inv.payhub  = pay_target.payhub;

                check(invoice_name == name("main"), "ERR-ABL-03: the invoice name must be 'main' (for now)");
                
                vapaee::pay::billing::get_invoice(true, inv, ram_payer, "ERR-ABL-03");
            }

            // just register invoice in history. just print values.
            void action_invoice(
                asset quantity, asset fee, string memo
            ) {
                PRINT("vapaee::pay::billing::action_invoice()\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" fee: ", fee.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                require_auth(get_self());
            }


            // send copies or the invoice to payer, seller and fee_recipient
            void send_invoice(
                const name& payer,
                const name& seller,
                const name& fee_recipient,
                const asset& quantity,
                const asset& fee,
                const string& memo
            ) {
                PRINT("vapaee::pay::billing::send_invoice()\n");
                PRINT(" payer: ", payer.to_string(), "\n");
                PRINT(" seller: ", seller.to_string(), "\n");
                PRINT(" fee_recipient: ", fee_recipient.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" fee: ", fee.to_string(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                
                // send invoice transfer to billing payer
                action(
                    permission_level{get_self(), "active"_n},
                    payer,
                    "invoice"_n,
                    std::make_tuple(quantity, fee, memo)
                ).send();

                // send invoice transfer to billing seller
                action(
                    permission_level{get_self(), "active"_n},
                    seller,
                    "invoice"_n,
                    std::make_tuple(quantity, fee, memo)
                ).send();
                
                // send invoice transfer to fee recipient
                action(
                    permission_level{get_self(), "active"_n},
                    fee_recipient,
                    "invoice"_n,
                    std::make_tuple(quantity, fee, memo)
                ).send();
            }

            // apply invoice fee to a given quantity for a given payhub_target with a gievn memo
            // get the token invoice template identified by name("main") for the given token (quantity.symbol.code())
            // apply the invoice fee to the quantity and send it to the invoice fee receiver
            // send an invoice transfer to both, the billing payer and the fee recipient
            // returns the final quantity to be sent to the target
            asset apply_invoice_fee(
                const name& payer,
                const asset& quantity,
                const payhubs_table& seller_payhub,
                const string& memo
            ) {
                PRINT("vapaee::pay::billing::apply_invoice_fee()\n");
                PRINT(" payer: ", payer.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" seller_payhub.id: ", std::to_string((long)seller_payhub.id), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");
                
                invoices_table inv;
                inv.id      = name("main");  // this is fixed for now until we decide to allow multiple invoices
                inv.fixed   = asset(0, quantity.symbol);
                
                // get invoice template entry
                vapaee::pay::billing::get_invoice(false, inv, get_self(), "ERR-AIF-01");

                payhubs_table fee_payhub;
                vapaee::pay::hub::get_payhub_for_id(false, inv.payhub, fee_payhub, get_self(), "ERR-AIF-02");

                check(quantity.symbol == inv.fixed.symbol, 
                    create_error_asset2(
                        "ERR-AIF-03: the quantity must be of the same token as the invoice fixed amount",
                        quantity,
                        inv.fixed
                    ).c_str()
                );
            
                asset total = quantity;
                if (inv.fixed.amount > 0) {
                    total = total - inv.fixed;
                }
                if (inv.percent > 0) {
                    total = total - asset(uint64_t((double)total.amount * inv.percent), total.symbol);
                }
                check(total.amount > 0, "ERR-AIF-04");
                    
                // amount of fee charged to the seller    
                asset total_fee = quantity - total;

                // here we are going to charge the fee_recipient for the service of the invoice
                asset system_fee = asset(total_fee.amount * INVOICE_SYSTEM_FEE_PERCENT, total_fee.symbol);

                // here we substract the system fee from the total fee
                asset fee_recipient_fee = total_fee - system_fee;

                name seller = seller_payhub.billing_to;
                name fee_recipient = fee_payhub.billing_to;

                send_invoice(payer, seller, fee_recipient, quantity, total_fee, memo);

                // send fee to fee_recipient
                string fee_memo = total_fee.to_string() + " (you chaged your client) - " + system_fee.to_string() + " (we charge you as gains fees) = " + fee_recipient_fee.to_string() + " (your net profit)";
                vapaee::pay::hub::handle_payhub_payment(fee_recipient_fee, std::to_string((unsigned long long)fee_payhub.id), fee_memo);
                
                // send fee to vapaee system
                string vapaee_memo = string("fees for invoice system");
                vapaee::pay::hub::handle_payhub_payment(system_fee, INVOICE_SYSTEM_FEE_PAYHUB_ALIAS, vapaee_memo);
                
                // TODO: acá falta pagarle al payhub del fee recipient
                return total;
            }

            // handler ------
            void handle_invoice(const name& from, const asset& quantity, string& target, const string& memo) {
                PRINT("vapaee::pay::billing::handle_invoice()\n");
                PRINT(" from: ", from.to_string(), "\n");
                PRINT(" quantity: ", quantity.to_string(), "\n");
                PRINT(" target: ", target.c_str(), "\n");
                PRINT(" memo: ", memo.c_str(), "\n");

                payhub_target pay_target;
                payhubs_table payhub;
                string generic_error = create_error_string1(
                    "ERROR on vapaeepayhub payment handler. You sent a transfer with memo 'invoice <PayHub-id> <optional-description>' but that PayHub was not found. Got: ",
                    target
                );
                bool found;
                int type = vapaee::pay::hub::parse_payhub_target(target, pay_target);
                switch(type) {
                    case TARGET_PAYHUB: {}
                    case TARGET_NAME: {
                        found = vapaee::pay::hub::get_payhub_for_id(false, pay_target.payhub, payhub, get_self(), NULL);
                        check(found, string(string("ERR-HINV-01: ") + generic_error).c_str());
                        break;
                    }
                    case TARGET_ACCOUNT: {
                        payhub.id = MAX_VALUE;
                        payhub.billing_to = pay_target.account;
                        break;
                    }

                    // The seller MUST have a payhub registered in order to receive payments with invoice
                    default: {
                        PRINT(" > pay_target: ", pay_target.to_string(), "\n");
                        PRINT(" > type: ", std::to_string((long)type), "\n");

                        check(false, string(string("ERR-HINV-02: ") + generic_error).c_str());
                    }
                };


                asset final = apply_invoice_fee(from, quantity, payhub, memo);


                PRINT(" -- Ending billing -- \n");
                // delegate payment to hub module
                vapaee::pay::hub::handle_payhub_payment(final, target, memo);

            }



            
            


        };     
    };
};
