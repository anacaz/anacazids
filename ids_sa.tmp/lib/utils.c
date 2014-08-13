#ifdef LINUX
#include <stdio.h>
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
#include <malloc.h>
#else /* LINUX */
#include <common.h>
#include <asm/processor.h>
//#include <memio.h>
#include <linux/ctype.h>
#endif /* LINUX */

static __inline__ unsigned long get_msr(void)
{
	unsigned long msr;

#ifdef COMMENT
	asm volatile("mfmsr %0" : "=r" (msr) :);
#endif /* COMMENT */
	return msr;
}

static __inline__ void set_msr(unsigned long msr)
{
#ifdef COMMENT
	asm volatile("mtmsr %0" : : "r" (msr));
#endif /* COMMENT */
}

static __inline__ unsigned long get_dec(void)
{
	unsigned long val;

#ifdef COMMENT
	asm volatile("mfdec %0" : "=r" (val) :);
#endif /* COMMENT */
	return val;
}

static __inline__ void set_dec(unsigned long val)
{
#ifdef COMMENT
	asm volatile("mtdec %0" : : "r" (val));
#endif /* COMMENT */
}

void enable_interrupts(void)
{
    set_msr (get_msr() | MSR_EE);
}

/* returns flag if MSR_EE was set before */
int disable_interrupts(void)
{
    ulong msr;

    msr = get_msr();
    set_msr (msr & ~MSR_EE);
    return ((msr & MSR_EE) != 0);
}

#ifdef COMMENT
u8 in8(u32 port)
{
    return in_byte(port);
}

void out8(u32 port, u8 val)
{
    out_byte(port, val);
}

unsigned long in32(u32 port)
{
    return in_long(port);
}
#endif /* COMMENT */

unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
	unsigned long result = 0,value;

	if (*cp == '0') {
		cp++;
		if ((*cp == 'x') && isxdigit(cp[1])) {
			base = 16;
			cp++;
		}
		if (!base) {
			base = 8;
		}
	}
	if (!base) {
		base = 10;
	}
	while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
	    ? toupper(*cp) : *cp)-'A'+10) < base) {
		result = result*base + value;
		cp++;
	}
	if (endp)
		*endp = (char *)cp;
	return result;
}

long simple_strtol(const char *cp,char **endp,unsigned int base)
{
	if(*cp=='-')
		return -simple_strtoul(cp+1,endp,base);
	return simple_strtoul(cp,endp,base);
}

#ifdef COMMENT
static inline void
soft_restart(unsigned long addr)
{
	/* SRR0 has system reset vector, SRR1 has default MSR value */
	/* rfi restores MSR from SRR1 and sets the PC to the SRR0 value */

	__asm__ __volatile__ ("mtspr    26, %0"         :: "r" (addr));
	__asm__ __volatile__ ("li       4, (1 << 6)"    ::: "r4");
	__asm__ __volatile__ ("mtspr    27, 4");
	__asm__ __volatile__ ("rfi");

	while(1);       /* not reached */
}

void
do_reset (void)
{
	ulong addr;
	/* flush and disable I/D cache */
	__asm__ __volatile__ ("mfspr    3, 1008"        ::: "r3");
	__asm__ __volatile__ ("ori      5, 5, 0xcc00"   ::: "r5");
	__asm__ __volatile__ ("ori      4, 3, 0xc00"    ::: "r4");
	__asm__ __volatile__ ("andc     5, 3, 5"        ::: "r5");
	__asm__ __volatile__ ("sync");
	__asm__ __volatile__ ("mtspr    1008, 4");
	__asm__ __volatile__ ("isync");
	__asm__ __volatile__ ("sync");
	__asm__ __volatile__ ("mtspr    1008, 5");
	__asm__ __volatile__ ("isync");
	__asm__ __volatile__ ("sync");

#ifdef CFG_RESET_ADDRESS
	addr = CFG_RESET_ADDRESS;
#else
	/*
	 * note: when CFG_MONITOR_BASE points to a RAM address,
	 * CFG_MONITOR_BASE - sizeof (ulong) is usually a valid
	 * address. Better pick an address known to be invalid on your
	 * system and assign it to CFG_RESET_ADDRESS.
	 */
	addr = CFG_MONITOR_BASE - sizeof (ulong);
#endif
	soft_restart(addr);
	while(1);       /* not reached */
}
#endif /* COMMENT */

unsigned decrementer_count;	     /* count value for 1e6/HZ microseconds */

volatile ulong timestamp = 0;

/*
 * timer_interrupt - gets called when the decrementer overflows,
 * with interrupts disabled.
 * Trivial implementation - no need to be really accurate.
 */
void timer_interrupt(struct pt_regs *regs)
{
	set_dec(decrementer_count);
	timestamp++;
}

void reset_timer(void)
{
	timestamp = 0;
}

ulong get_timer(ulong base)
{
	return (timestamp - base);
}

void set_timer(ulong t)
{
	timestamp = t;
}

