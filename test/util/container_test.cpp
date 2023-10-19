#include <catch2/catch_all.hpp>
#include "container.h"

namespace postfix::util {

TEST_CASE("vector_t: push_back", "[vector_t][normal]") {
    vector_t<int> vec;
    int true_range[10];
    int true_range_small[5];
    for(int i = 0; i < 10; ++i) {
        true_range[i] = i;
        REQUIRE_NOTHROW(vec.push_back(i));
    }
        
    REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(true_range));

    SECTION("pop elements and check") {
        for(int i = 0; i < 5; ++i) {
            true_range_small[i] = i;
            REQUIRE_NOTHROW(vec.pop_back());
        }
        
        REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(true_range_small));
        REQUIRE(vec.size() == 5);
        REQUIRE_FALSE(vec.empty());
    }

}

TEST_CASE("vector_t: constructor", "[vector_t][normal]") {
    vector_t<int> vec;
    for(int i = 0; i < 10; ++i)
        vec.push_back(i);

    SECTION("copy constructor") {
        vector_t<int> vec2(vec);

        REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(vec2));
    }

    SECTION("assignment operator") {
        vector_t<int> vec2;
        vec2 = vec;

        REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(vec2));
    }
    
    SECTION("assignment operator 2") {
        vector_t<int> vec2 = {1, 2, 3, 4};
        vec2 = vec;

        REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(vec2));
    }

}

struct example_t {
public:
    static int counter;
    static int default_const;
    static int copy_const;
    static int assignment_op;
    static int destruct;


    example_t(): counter_id(counter) {
        counter++;
        default_const++;
    }

    example_t(const example_t& other): counter_id(other.counter_id) {
        counter++;
        copy_const++;
    }

    example_t&
    operator=(const example_t& other) {
        counter_id = other.counter_id;
        assignment_op++;
        return *this;
    }

    ~example_t() {
        destruct++;
    }

    bool
    operator==(const example_t& other) {
        return counter_id == other.counter_id;
    }

private:
    int counter_id;

};

int example_t::counter = 0;
int example_t::default_const = 0;
int example_t::copy_const = 0;
int example_t::assignment_op = 0;
int example_t::destruct = 0;

TEST_CASE("vector_t: constructor calls", "[vector_t][normal]") {
    vector_t<example_t> vec(5);
    REQUIRE(example_t::default_const == 5);

    vector_t<example_t> vec2(vec);
    REQUIRE(example_t::copy_const == 5);

    {
        vector_t<example_t> vec3;
        // vec3 does not create any object
        REQUIRE(example_t::default_const + example_t::copy_const == 10);

        vec3 = vec;
        REQUIRE(example_t::copy_const == 10);
    }
    REQUIRE(example_t::destruct == 5);
    vec = vec2;
    // They have enough space, no space for object is allocated
    REQUIRE(example_t::default_const + example_t::copy_const == 15);
    // No object is deleted as well
    REQUIRE(example_t::destruct == 5);
    // They were assigned
    REQUIRE(example_t::assignment_op == 5);
}

} // namespace postfix::util