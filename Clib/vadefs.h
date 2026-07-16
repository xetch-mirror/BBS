#ifndef _INC_VADEFS
#define _INC_VADEFS

#include <stddef.h>

// va_list is just a pointer to the stack where the arguments are sitting
typedef char* va_list;

// Stack alignment helper. 
// On 32-bit x86, everything on the stack must be aligned to a 4-byte boundary.
#define _va_round(n) ((sizeof(n) + 3) & ~3)

// Get the address of the first optional argument (right after the last fixed one)
#define va_start(ap, last) (ap = (va_list)&(last) + _va_round(last))

// Grab the current argument on the stack, and bump the pointer to the next one
#define va_arg(ap, type) (*(type *)((ap += _va_round(type)) - _va_round(type)))

// Done reading. Null out the pointer so we don't accidentally leak anything.
#define va_end(ap) (ap = (va_list)0)

#endif
