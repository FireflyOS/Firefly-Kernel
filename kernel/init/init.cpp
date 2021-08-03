#include <init/init.hpp>
#include <init/mb2_proto.hpp>

namespace firefly::kernel
{
    void kernel_init(uint64_t mb2_struct_address)
	{
		mb2proto::init(mb2_struct_address);
        // firefly::kernel::mb2proto::init(mb2_struct_address);
    }
}  // namespace firefly::kernel