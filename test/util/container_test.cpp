#include <catch2/catch_all.hpp>
#include "vector.h"
#include "stack.h"

namespace postfix::util {

TEST_CASE("vector: push_back", "[vector][normal]") {
    vector<int> vec;
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

TEST_CASE("vector: constructor", "[vector][normal]") {
    vector<int> vec;
    for(int i = 0; i < 10; ++i)
        vec.push_back(i);

    SECTION("copy constructor") {
        vector<int> vec2(vec);

        REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(vec2));
    }

    SECTION("assignment operator") {
        vector<int> vec2;
        vec2 = vec;

        REQUIRE_THAT(vec, Catch::Matchers::RangeEquals(vec2));
    }
    
    SECTION("assignment operator 2") {
        vector<int> vec2 = {1, 2, 3, 4};
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

TEST_CASE("vector: constructor calls", "[vector][normal]") {
    vector<example_t> vec(5);
    REQUIRE(example_t::default_const == 5);

    vector<example_t> vec2(vec);
    REQUIRE(example_t::copy_const == 5);

    {
        vector<example_t> vec3;
        // vec3 does not create any object
        REQUIRE(example_t::default_const + example_t::copy_const == 10);

        vec3 = vec;
        REQUIRE(example_t::copy_const == 10);
    }
    REQUIRE(example_t::destruct == 5);
    vec = vec2;
/*  
    No more valid tests, as copying of vector, does not consider reusing previous space
    vector assignment uses copy-and-swap idiom

    // They have enough space, no space for object is allocated
    REQUIRE(example_t::default_const + example_t::copy_const == 15);
    // No object is deleted as well
    REQUIRE(example_t::destruct == 5);
    // They were assigned
    REQUIRE(example_t::assignment_op == 5);
*/
}

TEST_CASE("vector: erase", "[vector][normal]") {
    vector<int> ivec(10);
    std::iota(ivec.begin(), ivec.end(), 0);

    SECTION("erase begin()") {
        for(int i = 0; i < 10; ++i) {
            vector<int> test_vec(10 - i);
            std::iota(test_vec.begin(), test_vec.end(), i);
            REQUIRE_THAT(ivec, Catch::Matchers::RangeEquals(test_vec));

            ivec.erase(ivec.begin());
        }
    }

    for(int i = 0; i < 10; ++i) {
        SECTION("erase at ith pos") {
            ivec.erase(ivec.begin() + i);
            vector<int> test_vec(10 - 1);
            std::iota(test_vec.begin(), test_vec.begin() + i, 0);
            std::iota(test_vec.begin() + i, test_vec.end(), i + 1);

            REQUIRE_THAT(ivec, Catch::Matchers::RangeEquals(test_vec));
        }
    }

}

TEST_CASE("stack: check LIFO", "[stack][normal]") {
    char text[] = "abcdefg";
    int text_len = sizeof(text)/sizeof(text[0]);
    stack<char> st;

    for(int i = 0; i < text_len; ++i)
        st.push(text[i]);

    for(int i = text_len - 1; i >= 0; --i) {
        REQUIRE(text[i] == st.peek());
        st.pop();
    }

}

TEST_CASE("stack: constructor & assignment", "[stack][normal]") {
    stack<int> st = {0, 1, 2, 3};
    
    SECTION("copy constructor") {
        stack<int> st2(st);
        REQUIRE(st == st2);
    }

    SECTION("assignment operator") {
        stack<int> st2;
        REQUIRE_FALSE(st == st2);
        st2 = st;
        REQUIRE(st == st2);
    }

    SECTION("assignment operator 2") {
        stack<int> st2 = {1, 2, 3};
        REQUIRE_FALSE(st == st2);
        st2 = st;
        REQUIRE(st == st2);
    }
}

TEST_CASE("stack: assignment operator", "[stack][normal][assignment]") {
    const int iterations = 10;
    stack<int> st = {1, 2, 3, 4};

    stack<int> st_copy;
    for(int i = 0; i < iterations; ++i) {
        st.push(i);
        st_copy = st;
        REQUIRE(st == st_copy);
    }
}

} // namespace postfix::util