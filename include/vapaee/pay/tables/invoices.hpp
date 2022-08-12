// -- invoice --
// All tokens may have any number of invoice templates. Each time a payment is made, the invoice template is used to create an invoice registration.

// scope: token
// row: represents an invice template for this token

TABLE invoices_table {
    name id;             // name of the invoice template. name("main") by default for each token
    asset fixed;         // fixed amount to be charged as fees
    double percent;      // percentage of the payment to be charged as fees
    uint64_t payhub;     // destination payhub node for the fees
    uint64_t primary_key() const { return id.value; }

    std::string to_string() const {
        return fixed.symbol.code().to_string() + "(" + id.to_string() + ")";
    } 
};

typedef eosio::multi_index<"invoices"_n, invoices_table> invoices;

