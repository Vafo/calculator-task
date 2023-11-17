#ifndef TOKEN_FACTORY_H
#define TOKEN_FACTORY_H

#include "token.h"

namespace postfix {

// Can build everyone, except numbers :-( (as they require argument - number)
class token_factory {
public:

    token_factory(const token& in_proto): 
        prototype(in_proto)
    {}

    token build() {
        return token(prototype);
    }

    std::string get_name() {
        return prototype.get_name();
    }

private:
    token prototype;
};

} // namespace postfix

#endif