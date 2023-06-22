struct recipient_info {
    asset part;
    string target;    // It can refer to one of these options:
                      // -1 - not valid
                      //  0 - no specified yet
                      //  1 - it is a name and exists the account -> Telos account
                      //  2 - it is a number -> PayHub(id)
                      //  2 - it is a "<symbol_code>-<name>" -> REX pool (token & poollabel)
                      //  4 - it is a simple string -> must exist a (vip) name -> PayHub 
    std::string to_string() const {
        return part.to_string() + ": " + target;
    }
};
