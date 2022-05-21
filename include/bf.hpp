/*
 * MIT License
 *
 * Copyright (c) 2022 David Oniani
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef BF_H
#define BF_H

#include <cmath>
#include <concepts>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <vector>

namespace bf {

/// @brief Declaration of the concept `Hashable`, which is satisfied by any type `T` such that for
/// values `a` of type `T`, the expression `std::hash<T>{}(a)` compiles and its result is
/// convertible to `std::size_t`. From https://en.cppreference.com/w/cpp/language/constraints.
template <typename T>
concept hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

template <typename T>
concept iterable = std::ranges::forward_range<T>;

template <typename T>
concept arithmetic = std::is_floating_point_v<T>;

using bitvec = std::vector<bool>;

/**
 * @brief A zero-dependency bloom filter implementation. The data structure provides efficient
 * data storage and lookup. It is important to note that due to the probabilistic nature of the
 * data structure, there is a chance for false positive results. In other words, after inserting
 * data into the bloom filter, a lookup can either tell that the data is present with some
 * probability of the false positive outcome or tell that the data is definitely not present in
 * the data structure.
 */
struct BloomFilter {
    // Number of bits in the bit vector.
    std::size_t bits;
    // Number of hash functions.
    std::uint64_t hash_fns;
    // Bit vector.
    bitvec bvec;

    /**
     * @brief Creates a new bloom filter with optimal parameters.
     * @param n An approximate number of elements to be inserted.
     * @param eps False positive probability.
     */
    explicit constexpr BloomFilter(const std::uint64_t elems, const arithmetic auto eps) {
        if (elems <= 0) {
            throw std::domain_error("Number of elements must be greater than zero.");
        }
        if (eps < 0 || eps > 1) {
            throw std::domain_error("False positive probability must be between zero and one.");
        }

        bits = -std::ceil(elems * std::log(eps) / std::pow(std::log(2), 2));
        hash_fns = std::ceil(static_cast<float>(bits) / static_cast<float>(elems) * std::log(2));
        bvec = std::vector(bits, false);
    }

    /**
     * @brief Inserts a new element into the bloom filter.
     * @param data Data to be inserted into a bloom filter.
     */
    template <hashable T>
    constexpr void insert(T data) noexcept {
        auto hash = std::hash<T>{};
        for (std::size_t idx = 0; idx < hash_fns; idx++) {
            bvec[(hash(data) + idx << 2) % bits] = true;
        }
    }

    /**
     * @brief Inserts elements from the provided iterable into the bloom filter.
     * @param it An iterable containing elements to be inserted into the bloom filter.
     */
    constexpr void insert_many(iterable auto it) noexcept {
        for (const auto& el : it) {
            insert(el);
        }
    }

    /**
     * @brief Checks if the provided input is likely to be in the bloom filter.
     * @param data Data to be searched in the bloom filter.
     * @return A boolean value specifying whether the input was present or not.
     */
    template <hashable T>
    [[nodiscard]] constexpr auto search(T data) const noexcept {
        auto hash = std::hash<T>{};
        for (std::size_t idx = 0; idx < hash_fns; idx++) {
            if (!bvec[(hash(data) + idx << 2) % bits]) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Checks if the provided input elements are likely to be in the bloom filter.
     * @param data Data to be searched in the bloom filter.
     * @return A boolean value specifying whether the input was present or not.
     */
    [[nodiscard]] auto search_many(iterable auto it) const noexcept {
        bitvec res{};
        for (const auto& el : it) {
            res.push_back(search(el));
        }
        return res;
    }

    /**
     * @brief Clears the bloom filter.
     */
    auto clear() noexcept {
        bvec = std::vector(bits, false);
    }
};

}  // namespace bf

#endif  // BF_H
