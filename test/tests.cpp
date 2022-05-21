#include <catch2/catch_all.hpp>

#include "../include/bf.hpp"

TEST_CASE("Insert and search strings", "[insert][search][string]") {
    auto bf = bf::BloomFilter{6, 1e-2};

    bf.insert("");
    bf.insert("hello");
    bf.insert("world");
    bf.insert("I");
    bf.insert("am");
    bf.insert("here");

    REQUIRE(bf.search(""));
    REQUIRE(bf.search("hello"));
    REQUIRE(bf.search("world"));
    REQUIRE(bf.search("I"));
    REQUIRE(bf.search("am"));
    REQUIRE(bf.search("here"));

    bf.clear();

    REQUIRE(!bf.search(""));
    REQUIRE(!bf.search("hello"));
    REQUIRE(!bf.search("world"));
    REQUIRE(!bf.search("I"));
    REQUIRE(!bf.search("am"));
    REQUIRE(!bf.search("here"));
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

    bf.insert("me");
    bf.insert("yo");
    bf.insert("you");

    REQUIRE(bf.search("me"));
    REQUIRE(bf.search("yo"));
    REQUIRE(bf.search("you"));

    bf.clear();

    REQUIRE(!bf.search(""));
    REQUIRE(!bf.search("m"));
    REQUIRE(!bf.search("e"));
    REQUIRE(!bf.search("o"));
    REQUIRE(!bf.search("y"));
    REQUIRE(!bf.search("u"));

    REQUIRE(!bf.search("meyo"));
    REQUIRE(!bf.search("yome"));

    REQUIRE(!bf.search("meyou"));
    REQUIRE(!bf.search("youme"));
    REQUIRE(!bf.search("yoyou"));
    REQUIRE(!bf.search("youyo"));

    REQUIRE(!bf.search("meyoyou"));
    REQUIRE(!bf.search("meyouyo"));
    REQUIRE(!bf.search("yomeyou"));
    REQUIRE(!bf.search("yoyoume"));
    REQUIRE(!bf.search("youmeyo"));
    REQUIRE(!bf.search("youyome"));
}

TEST_CASE("Insert and search strings from a vector", "[insert][search][string]") {
    auto bf = bf::BloomFilter(5, 1e-3);
    auto words = std::vector<std::string>{"afopsiv", "coxpz", "pqeacxnvzm", "zm", "acxk"};

    for (const auto& word : words) {
        bf.insert(word);
        REQUIRE(bf.search(word));
    }
}

TEST_CASE("Insert many and search many", "[insert_many][search_many][int]") {
    auto bf = bf::BloomFilter(12, 1e-4);
    auto nums = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

    bf.insert_many(nums);
    for (const auto& num : nums) {
        REQUIRE(bf.search(num));
    }

    auto vals = bf.search_many(nums);
    for (const auto& val : vals) {
        REQUIRE(val);
    }
}
