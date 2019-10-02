#pragma once
#include <type_traits.h>

namespace firefly::std {
    template <typename T>
    constexpr remove_reference_t<T>&& move(T&& value) noexcept {
        return static_cast<
            remove_reference_t<T>&&>(value);
    };
}  // namespace firefly::std