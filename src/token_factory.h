#ifndef TOKEN_FACTORY_H
#define TOKEN_FACTORY_H

#include "new_token.h"

namespace postfix {

// Can build everyone, except numbers :-(
class token_factory {
public:

    token_factory(token_t& in_proto): 
        prototype(in_proto)
    {}

    token_t build() {
        return token_t(prototype);
    }

    std::string get_name() {
        return prototype.get_name();
    }

private:
    token_t prototype;
};

} // namespace postfix

#endif