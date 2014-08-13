#ifdef LINUX
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <malloc.h>
#else /* LINUX */
#include <common.h>
#include <watchdog.h>
#include <command.h>
#include <asm/processor.h>
#include <ppc_asm.tmpl>
#endif /* LINUX */
#include <ids_jmpvec.h>

unsigned decrementer_count;		/* count value for 1e6/HZ microseconds */
static __inline__ unsigned long get_msr(void)
{
	unsigned long msr;

	asm volatile("mfmsr %0" : "=r" (msr) :);
	return msr;
}

static __inline__ void set_msr(unsigned long msr)
{
	asm volatile("mtmsr %0" : : "r" (msr));
	asm volatile("isync");
}

static __inline__ unsigned long get_dec (void)
{
	unsigned long val;

	asm volatile ("mfdec %0":"=r" (val):);

	return val;
}


static __inline__ void set_dec (unsigned long val)
{
	if (val)
		asm volatile ("mtdec %0"::"r" (val));
}

#ifdef COMMENT
void enable_interrupts (void)
{
	set_msr (get_msr() | MSR_EE);
}
#endif /* COMMENT */

/* returns flag if MSR_EE was set before */
#ifdef COMMENT
int disable_interrupts (void)
{
	ulong msr = get_msr();
	set_msr (msr & ~MSR_EE);
	return ((msr & MSR_EE) != 0);
}
#endif /* COMMENT */

int interrupt_init (void)
{
	volatile immap_t *immr = (immap_t *)CFG_IMMR;

	immr->im_pic.gcr = MPC85xx_PICGCR_RST;
	while (immr->im_pic.gcr & MPC85xx_PICGCR_RST)
		;
	immr->im_pic.gcr = MPC85xx_PICGCR_M;
	decrementer_count =
		GET_TBCLK() /
		CFG_HZ;
#ifdef COMMENT
	mtspr(SPRN_TCR, TCR_PIE);
	set_dec (decrementer_count);
	set_msr (get_msr () | MSR_EE);
#endif
	return (0);
}

/*
 * Install and free a interrupt handler. Not implemented yet.
 */
void
irq_install_handler(int vec, interrupt_handler_t *handler, void *arg)
{
	return;
}

void
irq_free_handler(int vec)
{
	return;
}

/****************************************************************************/


extern volatile ulong timestamp;

/*
 * timer_interrupt - gets called when the decrementer overflows,
 * with interrupts disabled.
 * Trivial implementation - no need to be really accurate.
 */
#ifdef COMMENT
void timer_interrupt(struct pt_regs *regs)
{
	timestamp++;
	set_dec (decrementer_count);
	mtspr(SPRN_TSR, TSR_PIS);
#if defined(CONFIG_WATCHDOG)
	if ((timestamp % 1000) == 0)
		reset_85xx_watchdog();
#endif /* CONFIG_WATCHDOG */
}
#endif /* COMMENT */

#ifdef COMMENT
void reset_timer (void)
{
	timestamp = 0;
}

ulong get_timer (ulong base)
{
	return (timestamp - base);
}

void set_timer (ulong t)
{
	timestamp = t;
}
#endif /* COMMENT */

#if (CONFIG_COMMANDS & CFG_CMD_IRQ)

/*******************************************************************************
 *
 * irqinfo - print information about PCI devices,not implemented.
 *
 */
int
do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	printf ("\nInterrupt-unsupported:\n");

	return 0;
}

#endif  /* CONFIG_COMMANDS & CFG_CMD_IRQ */
