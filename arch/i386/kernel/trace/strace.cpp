#include <i386/libk++/iostream.h>

//TODO: Implement stack trace, that'll be for another time however
void panic(const char *msg)
{
	klog(msg);
	while (1)
		;
}