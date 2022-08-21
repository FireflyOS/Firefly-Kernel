#pragma once

static constexpr int kib_shift = 10;
static constexpr int mib_shift = 20;

#define BIT(o) (1 << o)
#define MiB(o) BIT(mib_shift) * o
#define GiB(o) MiB(1024L) * o