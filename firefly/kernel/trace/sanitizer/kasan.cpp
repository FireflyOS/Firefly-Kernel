// #include "firefly/logger.hpp"
// #include "firefly/memory-manager/virtual/virtual.hpp"
// void kasan_init() {
// 	firefly::kernel::mm::kernelPageSpace::accessor().mapRange(0xffffe00000000000, 1024*1024*1024, firefly::kernel::mm::AccessFlags::ReadWrite);
// }

// extern "C" void __asan_load1_noabort(void* p) {
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load2_noabort(void* p) {
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load4_noabort(void* p) {
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load8_noabort(void* p) {
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load16_noabort(void* p) {
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store1_noabort(void* p) {
// 	// firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store2_noabort(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }

// extern "C" void __asan_store4_noabort(void* p) {
// 	// firefly::kernel::info_logger << __func__ << '\n';
// 	firefly::kernel::info_logger << firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store8_noabort(void* p) {
// 	// firefly::kernel::info_logger << __func__ << '\n';
//     firefly::kernel::info_logger << firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store16_noabort(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_loadN_noabort(void* p, void* size) {
// 	// firefly::kernel::info_logger << __func__ << '\n';
// 	firefly::kernel::info_logger << firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_storeN_noabort(void* p, void* size) {
// 	firefly::kernel::info_logger << firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// 	// firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }


// extern "C" void __asan_load1(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load2(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load4(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load8(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_load16(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store1(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store2(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store4(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store8(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_store16(void* p) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_loadN(void* p, void* size) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }
// extern "C" void __asan_storeN(void* p, void* size) {
// 	firefly::kernel::info_logger << __func__ << '\n';
//     // firefly::kernel::info_logger << //firefly::kernel::info_logger.format("%s: %x\n", __func__, (unsigned long)p);
// }

// extern "C" void __asan_handle_no_return() {
//     firefly::kernel::info_logger << "kasan: __asan_handle_no_return()\n";
// }

// extern "C" void __asan_before_dynamic_init(const char* module_name) {
//     firefly::kernel::info_logger << "kasan: __asan_before_dynamic_init(" << module_name << ")\n";
// }

// extern "C" void __asan_after_dynamic_init() {
// 	firefly::kernel::info_logger << "kasan: __asan_after_dynamic_init()\n";
// }