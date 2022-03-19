#define CATCH_CONFIG_MAIN
#include "../include/bf.hpp"
#include "../third_party/catch.hpp"

using std::operator""s;

TEST_CASE("Insert and search strings", "[insert][search][string]") {
    auto bf = bf::BloomFilter{6, 1e-2};

    bf.insert(""s);
    bf.insert("hello"s);
    bf.insert("world"s);
    bf.insert("I"s);
    bf.insert("am"s);
    bf.insert("here"s);

    REQUIRE(bf.search(""s));
    REQUIRE(bf.search("hello"s));
    REQUIRE(bf.search("world"s));
    REQUIRE(bf.search("I"s));
    REQUIRE(bf.search("am"s));
    REQUIRE(bf.search("here"s));

    bf.clear();

    REQUIRE(!bf.search(""s));
    REQUIRE(!bf.search("hello"s));
    REQUIRE(!bf.search("world"s));
    REQUIRE(!bf.search("I"s));
    REQUIRE(!bf.search("am"s));
    REQUIRE(!bf.search("here"s));
}

TEST_CASE("Insert and search numbers", "[insert][search][number]") {
    auto bf = bf::BloomFilter{12, 1e-1};

    bf.insert(1.2);
    bf.insert(2.8);
    bf.insert(10.0);
    bf.insert(1);
    bf.insert(2);
    bf.insert(8);
    bf.insert(32);

    REQUIRE(bf.search(10.0));
    REQUIRE(bf.search(1.2));
    REQUIRE(bf.search(10.0));
    REQUIRE(bf.search(1));
    REQUIRE(bf.search(2));
    REQUIRE(bf.search(8));
    REQUIRE(bf.search(32));

    REQUIRE(!bf.search(10));
    REQUIRE(!bf.search(3.0));
    REQUIRE(!bf.search(3));
    REQUIRE(!bf.search(1.6));
    REQUIRE(!bf.search(4));
    REQUIRE(!bf.search(16));
    REQUIRE(!bf.search(64));
    REQUIRE(!bf.search(128));
    REQUIRE(!bf.search(256));
}

TEST_CASE("Insert and search strings (substrings and concatenations)", "[insert][search][string]") {
    auto bf = bf::BloomFilter{3, 1e-2};

    bf.insert("me"s);
    bf.insert("yo"s);
    bf.insert("you"s);

    REQUIRE(bf.search("me"s));
    REQUIRE(bf.search("yo"s));
    REQUIRE(bf.search("you"s));

    bf.clear();

    REQUIRE(!bf.search(""s));
    REQUIRE(!bf.search("m"s));
    REQUIRE(!bf.search("e"s));
    REQUIRE(!bf.search("o"s));
    REQUIRE(!bf.search("y"s));
    REQUIRE(!bf.search("u"s));

    REQUIRE(!bf.search("meyo"s));
    REQUIRE(!bf.search("yome"s));

    REQUIRE(!bf.search("meyou"s));
    REQUIRE(!bf.search("youme"s));
    REQUIRE(!bf.search("yoyou"s));
    REQUIRE(!bf.search("youyo"s));

    REQUIRE(!bf.search("meyoyou"s));
    REQUIRE(!bf.search("meyouyo"s));
    REQUIRE(!bf.search("yomeyou"s));
    REQUIRE(!bf.search("yoyoume"s));
    REQUIRE(!bf.search("youmeyo"s));
    REQUIRE(!bf.search("youyome"s));
}

TEST_CASE("Insert and search strings from a vector", "[insert][search][string]") {
    auto bf = bf::BloomFilter(5, 1e-3);
    auto words = std::vector<std::string>{"afopsiv"s, "coxpz"s, "pqeacxnvzm"s, "zm"s, "acxk"s};

    for (const auto &word : words) {
        bf.insert(word);
        REQUIRE(bf.search(word));
    }
}

TEST_CASE("Insert many and search many", "[insert_many][search_many][int]") {
    auto bf = bf::BloomFilter(12, 1e-4);
    auto nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    bf.insert_many(nums);
    for (const auto &num : nums) {
        REQUIRE(bf.search(num));
    }

    auto vals = bf.search_many(nums);
    for (const auto &val : vals) {
        REQUIRE(val);
    }
}
