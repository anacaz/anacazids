#include <ids_jmpvec.h>

/*
 * This function is intended for SHORT delays only.
 * It will overflow at around 10 seconds @ 400MHz,
 * or 20 seconds @ 200MHz.
 */
unsigned long usec2ticks(unsigned long usec)
{
	unsigned long ticks;

	if (usec < 1000)
		ticks = ((usec * ((GET_TBCLK())/1000)) + 500) / 1000;
	else
		ticks = ((usec / 10) * (GET_TBCLK() / 100000));
	return (ticks);
}

/*
 * We implement the delay by converting the delay (the number of
 * microseconds to wait) into a number of time base ticks; then we
 * watch the time base until it has incremented by that amount.
 */
void udelay(unsigned long usec)
{
	unsigned long ticks = usec2ticks(usec);

	WAIT_TICKS(ticks);
}

unsigned long ticks2usec(unsigned long ticks)
{
	unsigned long tbclk = GET_TBCLK();

	/* usec = ticks * 1000000 / tbclk
	 * Multiplication would overflow at ~4.2e3 ticks,
	 * so we break it up into
	 * usec = ( ( ticks * 1000) / tbclk ) * 1000;
	 */
	ticks *= 1000L;
	ticks /= tbclk;
	ticks *= 1000L;

	return((unsigned long)ticks);
}
