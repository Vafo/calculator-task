#include "catch2/catch_all.hpp"

#include <string>

#include "shared_ptr.h"
#include "container.h"

namespace postfix::util {

TEST_CASE("shared_ptr: construct shared_ptr", "[shared_ptr][normal]") {
    shared_ptr<int> iptr(5);

    REQUIRE(*iptr == 5);

    *iptr = 123;
    REQUIRE(*iptr == 123);
    // Creates new shared_ptr (BAD PRACTICE, delegate conversion to shared_ptr to make_shared )
    iptr = 123;
    REQUIRE(*iptr == 123);
}

TEST_CASE("shared_ptr: copy ptr", "[shared_ptr][normal]") {
    // Conversion is not supported :-(
    shared_ptr<std::string> sptr = std::string("Hello World!");
    shared_ptr<std::string> sptr_copy = sptr;
    // They point to same object
    REQUIRE( sptr_copy == sptr );
    REQUIRE( *sptr_copy == *sptr );

    shared_ptr<std::string> sptr_other = std::string("Hello World!");

    REQUIRE( sptr_copy != sptr_other );
    REQUIRE( *sptr_copy == *sptr_other );
}

TEST_CASE("shared_ptr: vector of shared_ptr", "[shared_ptr][normal]") {
    const int iterations = 10;
    vector_t<shared_ptr<int>> ptrvec;
    for(int i = 0; i < iterations; ++i)
        // Conversion constructor is called
        ptrvec.push_back(i);

    for(int i = 0; i < iterations; ++i)
        REQUIRE(*ptrvec[i] == i);
}

struct counter_t {
    static int counter;
    counter_t() {
        ++counter;
    }

    counter_t(const counter_t &other) {
        ++counter;
    }

    ~counter_t() {
        --counter;
    }
};

int counter_t::counter = 0;

TEST_CASE("shared_ptr: count const and dest of class", "[shared_ptr][normal]") {
    const int iterations = 10;

    SECTION("construct individual ptrs") {
        // check if precondition is valid (independent from code under test)
        assert(counter_t::counter == 0);
        for(int i = 0; i < iterations; ++i)
            shared_ptr<counter_t> count_ptr = counter_t();
        REQUIRE(counter_t::counter == 0);
    }

    SECTION("constuct ptrs in vector") {
        assert(counter_t::counter == 0);
        {
            vector_t<shared_ptr<counter_t>> count_vec;
            for(int i = 0; i < iterations; ++i)
                count_vec.push_back(counter_t());
            REQUIRE(counter_t::counter == iterations);
        }
            REQUIRE(counter_t::counter == 0);
    }

}

} // namespace postfix::util