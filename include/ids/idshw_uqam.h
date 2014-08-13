#ifndef _idshw_uqam_h
#define _idshw_uqam_h
/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 091201
 */

#define XAM(X)		(X)
#define CPUMAP(A)	(A)

#define CPU_SDRAM_LO	CPUMAP(0x00000000)		/* CPU SDRAM */
#define CPU_SDRAM_HI	CPUMAP(0x0FFFFFFF)
#define U1_LO		CPUMAP(0x10000000)
#define U1_HI		CPUMAP(0x7FFFFFFF)
#define PCI_BUS_LO	CPUMAP(0x80000000)
#define PCI_BUS_HI	CPUMAP(0x9FFFFFFF)
#define U2_LO		CPUMAP(0xA0000000)
#define U2_HI		CPUMAP(0xDFFFFFFF)
#define CCSRBAR_LO	CPUMAP(0xE0000000)		/* CCSRBAR config reg */
#define CCSRBAR_HI	CPUMAP(0xE1FFFFFF)
#define PCI_IO_LO	CPUMAP(0xE2000000)		/* PCI I/O */
#define PCI_IO_HI	CPUMAP(0xEFFFFFFF)
#define LBUS_SDRAM_LO	CPUMAP(0xF0000000)		/* Local bus SDRAM */
#define LBUS_SDRAM_HI	CPUMAP(0xFDFFFFFF)
#define FLASH_LO	CPUMAP(0xFE000000)		/* Flash */
#define FLASH_HI	CPUMAP(0xFFFFFFFF)
						/* Local Bus Intf. Module  */
#define FPGA_LIM_LO	(XAM(0x01) | CPUMAP(0x00000000))
#define FPGA_LIM_HI	(XAM(0x01) | CPUMAP(0xFFFFFFFF))
#define U3_LO		(XAM(0x02) | CPUMAP(0x00000000))
#define U3_HI		(XAM(0x02) | CPUMAP(0xFFFFFFFF))
#define FPGA_DNLD_LO	(XAM(0x03) | CPUMAP(0x00000000))
#define FPGA_DNLD_HI	(XAM(0x03) | CPUMAP(0xFFFFFFFF))

static inline unsigned long duc_get(unsigned long base, unsigned long addr)
{
	return(0);
}

static inline void duc_put(unsigned long base, unsigned long addr, unsigned long value)
{
}

static inline unsigned long duc_aget(unsigned long base, unsigned long addr)
{
	return(0);
}

static inline void duc_aput(unsigned long base, unsigned long addr, unsigned long value)
{
}

#endif /* !_idshw_uqam_h */
