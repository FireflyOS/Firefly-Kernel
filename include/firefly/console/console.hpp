#pragma once

#include "firefly/stivale2.hpp"

namespace firefly::kernel::console
{
	void init(stivale2_struct_tag_framebuffer *fb);
	void write(const char *str);
} // namespace firefly::kernel::device::console
