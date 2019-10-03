#pragma once
#include <cstdlib/cstdint.h>

namespace std {
    template <typename T>
    class initializer_list {
    public:
        using iterator = const T*;
        using const_iterator = const T*;

    private:
        iterator _array;
        size_t _sz;
        constexpr initializer_list(const_iterator ar, size_t _sz)
            : _array{ ar }, _sz{ _sz } {
        }

    public:
        constexpr initializer_list()
            : _array{ 0 }, _sz{ 0 } {
        }

        constexpr size_t size() const noexcept {
            return _sz;
        }

        constexpr const_iterator begin() const noexcept {
            return _array;
        }

        constexpr const_iterator end() const noexcept {
            return _array + _sz;
        }
    };

}  // namespace firefly::std