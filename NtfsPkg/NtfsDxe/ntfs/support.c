#include "support.h"

const BOOL test_and_set_bit(int bit, int var)
{
	const BOOL old_state = test_bit(bit, var);
	set_bit(bit, var);
	return old_state;
}

const BOOL test_and_clear_bit(int bit, int var)
{
 const BOOL old_state = test_bit(bit, var);
 clear_bit(bit, var);
 return old_state;
}
