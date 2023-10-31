#include "catch2/catch_all.hpp"

#include <string>

#include "unique_ptr.h"

namespace postfix::util {

TEST_CASE("unique_ptr: create obj", "[unique_ptr][normal]") {
    unique_ptr<std::string> uni_ptr = make_unique<std::string>("Aboba");
    REQUIRE(*uni_ptr == "Aboba");

    unique_ptr<std::string> other_ptr( make_unique<std::string>(*uni_ptr) );
    
    REQUIRE(*other_ptr == *uni_ptr);
    (*other_ptr)[0] = 'K';

    REQUIRE(*other_ptr != *uni_ptr);
    REQUIRE(*other_ptr == "Kboba");
}

struct Base {
    Base() {}
    virtual ~Base() {}

    virtual int get_val() { return 1; }
};

struct Derived: public Base {
    Derived() {}
    virtual ~Derived() override {}

    virtual int get_val() { return 2; }
};

TEST_CASE("unique_ptr: create derived", "[unique_ptr]") {
    unique_ptr<Derived> d_ptr(new Derived());
    unique_ptr<Base> b_ptr = std::move(d_ptr);

    // b_ptr = make_unique<Derived>();

    REQUIRE(b_ptr->get_val() == 2);
}

TEST_CASE("unique_ptr: swap ptrs", "[unique_ptr]") {
    unique_ptr<std::string> a = make_unique<std::string>("Helo");
    unique_ptr<std::string> b = make_unique<std::string>("Oleh");

    a.swap(b);

    REQUIRE(*a == "Oleh");
    REQUIRE(*b == "Helo");
}

} // namespace postfix::util