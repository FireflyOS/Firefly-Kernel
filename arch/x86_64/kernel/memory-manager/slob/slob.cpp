#include "x86_64/memory-manager/slob/slob.hpp"

#include "x86_64/memory-manager/buddy/buddy.hpp"

namespace firefly::kernel::mm::slob {
using namespace buddy;

void Slob::initialize() {

}

void Slob::allocate([[maybe_unused]]size_t size) {
}
}  // namespace firefly::kernel::mm::slob
