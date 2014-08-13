#include <ppc_asm.tmpl>
#include <ppc_defs.h>
#include <config.h>
#include <watchdog.h>

/*
 * unsigned long long get_ticks(void);
 *
 * read timebase as "long long"
 */
	.globl	get_ticks
get_ticks:
1:	mftbu	r3
	mftb	r4
	mftbu	r5
	cmp	0,r3,r5
	bne	1b
	blr

/*
 * Delay for a number of ticks
 */
	.globl	wait_ticks
wait_ticks:
	mflr	r8		/* save link register */
	mr	r7, r3		/* save tick count */
	bl	get_ticks	/* Get start time */

	/* Calculate end time */
	addc    r7, r4, r7	/* Compute end time lower */
	addze	r6, r3		/*     and end time upper */

 	/* WATCHDOG_RESET		.* Trigger watchdog, if needed */
1:	bl	get_ticks	/* Get current time */
	subfc	r4, r4, r7	/* Subtract current time from end time */
	subfe.	r3, r3, r6
	bge	1b		/* Loop until time expired */

	mtlr	r8		/* restore link register */
	blr
