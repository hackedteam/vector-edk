#include <Library/UefiBootServicesTableLib.h>

/** Sleep for the specified number of Microseconds.

    Implements the usleep(3) function.

    @param[in]    Microseconds    Number of microseconds to sleep.

    @retval   0   Always returns zero.
**/
int usleep(int microseconds)
{
	gBS->Stall(microseconds);

	return 0;
};
