
extern "C" [[noreturn]] void kernel_main()
{
	asm("movl $0xdeadbeef, %eax"); //qemu console -> info registers (expect EAX=0xdeadbeef)
	while (1);
}