#pragma once

namespace firefly::kernel
{
    class UserSpace
    {
        public:
            UserSpace();
            void jump(void (*fn)());
    };
} // namespace firefly::kernel