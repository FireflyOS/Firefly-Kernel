#pragma once

namespace firefly::kernel::core::paging {
enum class AccessFlags : int {
    Readonly = 1,
    ReadWrite = 3,
    UserReadOnly = 5,
    UserReadWrite = 7
};

// TODO: Implement me!
enum class CacheMode : int {
    None,
    Uncachable,
    WriteCombine,
    WriteThrough,
    WriteBack
};
}  // namespace firefly::kernel::core::paging