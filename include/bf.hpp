/// A zero-dependency bloom filter. The current implementation can load over 400000 words in under
/// 0.15 seconds. Searches for words are instantaneous. That being said, due to the fundamental
/// property of bloom filters, there is a chance for false positive results.

#ifndef BF_H
#define BF_H

#include <cmath>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace bf {

/// Declaration of the concept `Hashable`, which is satisfied by any type `T` such that for values
/// `a` of type `T`, the expression `std::hash<T>{}(a)` compiles and its result is convertible to
/// `std::size_t`.
///
/// Taken from https://en.cppreference.com/w/cpp/language/constraints.
template <typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept Iterable = std::ranges::forward_range<T>;

using BitVec = std::vector<bool>;

struct BloomFilter {
    /// Number of bits in the bit vector.
    std::size_t bits{};
    /// Number of hash functions.
    std::uint64_t hash_fns{};
    /// Bit vector.
    BitVec bitvec{};

    /// Creates a new bloom filter with optimal parameters.
    ///
    /// @param n An approximate number of elements to be inserted.
    /// @param eps False positive probability.
    template <typename T, typename = std::enable_if_t<std::is_floating_point_v<T>, T>>
    explicit constexpr BloomFilter(const std::uint64_t elems, const T eps) {
        if (elems <= 0) {
            throw std::domain_error("Number of elements must be greater than zero.");
        }
        if (eps < 0 || eps > 1) {
            throw std::domain_error("False positive probability must be between zero and one.");
        }

        bits = -std::ceil(elems * std::log(eps) / std::pow(std::log(2), 2));
        hash_fns = std::ceil(bits / static_cast<T>(elems) * std::log(2));
        bitvec = std::vector(bits, false);
    }

    /// Inserts a new element into the bloom filter.
    ///
    /// @param data Data to be inserted into a bloom filter.
    template <Hashable T>
    constexpr void insert(T data) noexcept {
        auto hash = std::hash<T>{};
        for (std::size_t idx = 0; idx < hash_fns; idx++) {
            bitvec[(hash(data) + idx << 2) % bits] = true;
        }
    }

    /// Inserts elements from the provided iterable into the bloom filter.
    ///
    /// @param it An iterable containing elements to be inserted into the bloom filter.
    template <Iterable T>
    constexpr void insert_many(T it) noexcept {
        for (const auto& el : it) {
            insert(el);
        }
    }

    /// Checks if the provided input is likely to be in the bloom filter.
    ///
    /// @param data Data to be searched in the bloom filter.
    /// @return A boolean value specifying whether the input was present or not.
    template <Hashable T>
    [[nodiscard]] constexpr auto search(T data) const noexcept {
        auto hash = std::hash<T>{};
        for (std::size_t idx = 0; idx < hash_fns; idx++) {
            if (!bitvec[(hash(data) + idx << 2) % bits]) {
                return false;
            }
        }
        return true;
    }

    /// Checks if the provided input elements are likely to be in the bloom filter.
    ///
    /// @param data Data to be searched in the bloom filter.
    /// @return A boolean value specifying whether the input was present or not.
    template <Iterable T>
    [[nodiscard]] auto search_many(T it) const noexcept {
        BitVec res{};
        for (const auto& el : it) {
            res.push_back(search(el));
        }
        return res;
    }

    /// Clears the bloom filter.
    auto clear() noexcept { bitvec = std::vector(bits, false); }
};

}  // namespace bf

#endif  // BF_H
