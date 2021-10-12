/* api.h
 *   by SergeyMC9730
 *
 * Created:
 *   9/26/2021, 7:45:07 PM
 * Last edited:
 *   9/26/2021, 8:05:01 PM
 * Auto updated?
 *   Yes
 *
 * Description:
 *   This is Firefly API for external applications. 
 *   This API contains function pointers.
**/

#pragma once

/**
 * printf function
 * 
 * @param string (const char *)
 * @return int (int)
 **/
extern int *fapi_printf;

/**
 * firefly::kernel::io::legacy::writeTextSerial function
 * 
 * @param string (const char *)
 * @return nothing (void)
 **/
extern void *fapi_writeTextSerial;

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
