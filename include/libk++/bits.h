#pragma once

static constexpr auto kib_shift = 10;
static constexpr auto mib_shift = 20;

#define BIT(o) (1 << o)

#define MiB(o) BIT(mib_shift) * o