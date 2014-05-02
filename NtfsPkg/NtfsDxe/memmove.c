/***
*memmove.c - contains memmove routine
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       memmove() copies a source memory buffer to a destination buffer.
*       Overlapping buffers are treated specially, to avoid propogation.
*
*******************************************************************************/
#include <sys/types.h>

/***
*memmove - Copy source buffer to destination buffer
*
*Purpose:
*       memmove() copies a source memory buffer to a destination memory buffer.
*       This routine recognize overlapping buffers to avoid propogation.
*       For cases where propogation is not a problem, memcpy() can be used.
*
*Entry:
*       void *dst = pointer to destination buffer
*       const void *src = pointer to source buffer
*       size_t count = number of bytes to copy
*
*Exit:
*       Returns a pointer to the destination buffer
*
*Exceptions:
*******************************************************************************/

void * memmove (void * dst, const void * src, size_t count)
{
        void * ret = dst;

        if (dst <= src || (char *)dst >= ((char *)src + count)) {
                /*
                 * Non-Overlapping Buffers
                 * copy from lower addresses to higher addresses
                 */
                while (count--) {
                        *(char *)dst = *(char *)src;
                        dst = (char *)dst + 1;
                        src = (char *)src + 1;
                }
        }
        else {
                /*
                 * Overlapping Buffers
                 * copy from higher addresses to lower addresses
                 */
                dst = (char *)dst + count - 1;
                src = (char *)src + count - 1;

                while (count--) {
                        *(char *)dst = *(char *)src;
                        dst = (char *)dst - 1;
                        src = (char *)src - 1;
                }
        }

        return(ret);
}
