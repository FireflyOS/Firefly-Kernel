/* api.c
 *   by SergeyMC9730
 *
 * Created:
 *   9/26/2021, 7:45:07 PM
 * Last edited:
 *   9/27/2021, 5:54:53 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This is Firefly API for external applications. 
 *   This API contains function pointers.
**/

#include "api.h"

/**
 * printf function. No description
 * 
 * @param string (const char *)
 * @return int (int)
 **/
int *fapi_printf = (int *)0xFFFFFFFF802053D0;

/**
 * firefly::kernel::io::legacy::writeTextSerial function. No desctipnm.
 * 
 * @param string (const char *)
 * @return nothing (void)
 **/
void *fapi_writeTextSerial = (void *)0xFFFFFFFF80201000;

/**
 * Running functions
 * 
 * int function_result = ((int (*)(const char *))fapi_printf)("Hello World!\n");
 * ^                       ^       ^             ^            ^
 * |                       |       |             |            |
 * You can make variable   Function|             Function     Start a
 * if function returns     type    Function      pointer      function
 * something                       arguments
 **/