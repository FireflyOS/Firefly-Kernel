#pragma once

namespace firefly::std {
    template <typename T>
    struct remove_reference { using type = T; };
    template <typename T>
    struct remove_reference<T&> { using T = type; };
    template <typename T>
    struct remove_reference<T&&> { using T = type; };

    template <typename T>
    using remove_reference_t = remove_reference<T>::type;
}  // namespace firefly::std