#pragma once

namespace firefly::std {

    template <typename InputIt, typename OutputIt>
    OutputIt copy(InputIt begin, const InputIt end, OutputIt write) {
        for (; begin < end; *(write++) = *(begin++))
            ;
        return write;
    }

    template <typename InputIt, typename OutputIt, typename UnaryPredicate>
    OutputIt copy_if(InputIt begin, const InputIt end, OutputIt write, UnaryPredicate func) {
        while (begin < end) {
            if (!func(*begin)) {
                continue;
            }
            *(write++) = *(begin++);
        }
        return write;
    }
}  // namespace firefly::std
