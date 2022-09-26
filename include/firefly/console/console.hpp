#pragma once

namespace firefly::kernel::console
{
	void init();
	[[gnu::no_sanitize_address]] void write(const char *str);
} // namespace firefly::kernel::device::console
