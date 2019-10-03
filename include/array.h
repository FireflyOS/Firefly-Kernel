#pragma once
#include <algorithm.h>
#include <cstdlib/cstdint.h>
#include <initializer_list.h>

namespace firefly::std {
    template <typename T, size_t N>
    class array {
        T data[N];

    public:
        using iterator = T*;
        using const_iterator = const T*;

        array() = default;
        array(::std::initializer_list<T> const& arr) {
            firefly::std::copy(
                arr.begin(), arr.end(), data);
        }

        array& operator=(array const&) = default;
        array& operator=(array&&) = default;

        array(array const& arr) noexcept {
            firefly::std::copy(
                arr.begin(), arr.end(), this->begin());
        }

        array(array&& arr) noexcept {
            firefly::std::copy(
                arr.begin(), arr.end(), this->begin());
        }

        [[nodiscard]] iterator begin() {
            return data;
        }

        [[nodiscard]] iterator end() {
            return data + N;
        }

        [[nodiscard]] const_iterator begin() const {
            return data;
        }

        [[nodiscard]] const_iterator end() const {
            return data + N;
        }

        [[nodiscard]] T& operator[](size_t idx) noexcept {
            return data[idx];
        }

        [[nodiscard]] T const& operator[](size_t idx) const noexcept {
            return data[idx];
        }

        [[nodiscard]] constexpr size_t max_size() {
            return N;
        }

        [[nodiscard]] constexpr size_t size() {
            return N;
        }
    };

}  // namespace firefly::std