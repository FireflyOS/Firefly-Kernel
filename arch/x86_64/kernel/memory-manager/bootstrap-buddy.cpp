// #include <x86_64/memory-manager/bootstrap-buddy.hpp>

// namespace firefly::mm::buddy {
// //The address where the allocator resides, must be marked as used by the buddy on initialization
// static uint64_t buddy_allocator_address;

// //Resize the first entry in the memory map to store the buddy allocator itself
// void bootstrap_buddy(multiboot_uint64_t &mmap_base_entry, multiboot_uint64_t &length) {
// 	buddy_allocator_address = mmap_base_entry;
	
// 	//We'll just assume the non-existant buddy allocator is 4K in size
// 	//Replace 0x1000 with the size of the buddy allocator
// 	mmap_base_entry += 0x1000;
// 	length -= 0x1000;
// }
// }  // namespace firefly::mm::buddy
