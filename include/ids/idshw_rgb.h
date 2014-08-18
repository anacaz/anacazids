#ifndef _idshw_rgb_h
#define _idshw_rgb_h
/*
 * Integrated Diagnostics Subsystem
 *
 * Copyright (c) 2006-2014 "Anacaz Networks, Inc."
 * Diagnostics subsystem for U-Boot
 * 
 * This file is part of anacazids.
 * 
 * anacazids is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * rfa - 090708
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

/*
 * A6 access
 *
 */
#define LBUS_A6_BASE		0x80030000

#define LBUS_A6_0		(0x00000000 << 1)
#define LBUS_A6_1		(0x00000010 << 1)
#define LBUS_A6_2		(0x00000020 << 1)
#define LBUS_A6_3		(0x00000030 << 1)
#define LBUS_A6_4		(0x00000040 << 1)
#define LBUS_A6_5		(0x00000050 << 1)
#define LBUS_A6_6		(0x00000060 << 1)
#define LBUS_A6_7		(0x00000070 << 1)
#define LBUS_A6_8		(0x00000080 << 1)
#define LBUS_A6_9		(0x00000090 << 1)
#define LBUS_A6_10		(0x000000A0 << 1)
#define LBUS_A6_11		(0x000000B0 << 1)
#define LBUS_A6_ALL		(0x000000F0 << 1)

#define A6_BASE(X)		(LBUS_A6_BASE | (((X) & 0xF) << 5))

#define A6_FLASH_DMA		0x60000000
#define A6_FLASH_CPU		0x60001000
#define A6_SD_CTL		0x60002000
#define A6_VIC			0x60003000
#define A6_DDR_CTL		0x60004000
#define A6_DMA_CTL		0x60005000
#define A6_USB			0x60006000

#define A6_DRAM_CTL		(A6_DDR_CTL | 0x00)	/* REGISTER  */
#define DRAM_ENABLE		0x00000001		/* BIT FIELD */
#define DRAM_AUTO_REF_EN	0x00000002		/* BIT FIELD */
#define DRAM_CKE		0x00000004		/* BIT FIELD */
#define DRAM_RESET		0x00000008		/* BIT FIELD */

#define A6_DRAM_CFG		(A6_DDR_CTL | 0x04)	/* REGISTER */
#define DRAM_TYPE		0x00000001		/* BIT FIELD */
#define DDR_2			0			/* VALUE */
#define DDR_3			1			/* VALUE */
#define DRAM_SIZE		0x0000000E		/* BIT FIELD */
#define DDR_256			0			/* VALUE */
#define DDR_512			1			/* VALUE */
#define DDR_1024		2			/* VALUE */
#define DDR_2048		3			/* VALUE */
#define DDR_4096		4			/* VALUE */
#define DDR_8192		5			/* VALUE */
#define DQ_BUS_WIDTH		0x00000010		/* BIT FIELD */
#define BUS_32			0			/* VALUE */
#define BUS_16			1			/* VALUE */
#define CTLR_ODT_CFG		0x000000E0		/* BIT FIELD */
#define IDDQ_TST_EN		0x00000100		/* BIT FIELD */
#define DQS_IEN_EN		0x00000200		/* BIT FIELD */
#define PWR_DOWN_MODE_EN	0x00000400		/* BIT FIELD */
#define SERIAL_MODE		0x00000800		/* BIT FIELD */
#define POSTED_REF_CREDIT	0x0000F000		/* BIT FIELD */
#define DQS_SYNC_CFG		0x00070000		/* BIT FIELD */
#define ZQCS_CLBR_CFG		0x00380000		/* BIT FIELD */
#define ZQCS_SEL		0x00400000		/* BIT FIELD */

#define A6_DRAM_TIMING1		(A6_DDR_CTL | 0x08)	/* REGISTER */
#define A6_DRAM_TIMING2		(A6_DDR_CTL | 0x0C)	/* REGISTER */
#define A6_DRAM_INIT_CTL	(A6_DDR_CTL | 0x10)	/* REGISTER */
#define A6_DRAM_MODE_REG0	(A6_DDR_CTL | 0x14)	/* REGISTER */
#define A6_DRAM_MODE_REG1	(A6_DDR_CTL | 0x18)	/* REGISTER */
#define A6_DRAM_MODE_REG2	(A6_DDR_CTL | 0x1C)	/* REGISTER */
#define A6_DRAM_MODE_REG3	(A6_DDR_CTL | 0x20)	/* REGISTER */
#define A6_DRAM_DQS_SYNC	(A6_DDR_CTL | 0x24)	/* REGISTER */

/*
 * HARDWARE
 *
 * Write an A6 register.  Break the 32 bit value up into to 16 bit
 * values and write the LSW first and then the MSW second.  Returns
 * nothing.
 */
static inline void a6_wrreg(unsigned long r, unsigned long v)
{
	*((volatile unsigned short *)r) = (unsigned short)v;
	*((volatile unsigned short *)(r + 2)) = (unsigned short)(v >> 16);
}

/*
 * HARDWARE
 *
 * Read an A6 register.  The read comes in two parts (with a third as a
 * temporary, but harmless, workaround).  First read the LSW and then the
 * MSW return the two 16 bit values as a single 32 bit value.
 */
static inline unsigned long a6_rdreg(unsigned long r)
{
	unsigned long v;

	v = (unsigned long)*(volatile unsigned short *)r;
	v |= (unsigned long)*(volatile unsigned short *)(r + 2) << 16;
	return(v);
}

unsigned long a6_get(unsigned long, unsigned long);
unsigned long a6_aget(unsigned long, unsigned long);
void a6_put(unsigned long, unsigned long, unsigned long);
void a6_aput(unsigned long, unsigned long, unsigned long);
unsigned short mux_get(unsigned long addr);
unsigned short mux_aget(unsigned long addr);
void mux_aput(unsigned long addr, unsigned short value);
void mux_put(unsigned long addr, unsigned short value);
unsigned long hub_get(unsigned long base, unsigned long addr);
void hub_put(unsigned long base, unsigned long addr, unsigned long value);
unsigned long hub_aget(unsigned long base, unsigned long addr);
void hub_aput(unsigned long base, unsigned long addr, unsigned long value);

/*
 * Write the A6 HIF control register with the caller supplied
 * value and then reset to zero.
 */
static inline void a6_hif(unsigned long base, unsigned short value)
{
	*(volatile unsigned short *)base = value;
	udelay(200);
	*(volatile unsigned short *)base = 0x0000;
	udelay(200);
}

static inline void a6_provision(unsigned long base)
{
	unsigned long mask = 0x00003000;

	a6_hif(base, 0x4000);			/* HIF Reset */
	a6_put(base, 0x60010024, mask);		/* Interrupt Sense */
	a6_put(base, 0x6001002C, mask);		/* Interrupt Event */
}

static inline void a6_wakeup(unsigned long base)
{
	unsigned long mask = 0x00003000;

	a6_put(base, 0x60010010, mask);		/* Interrupt Enable */
	a6_hif(base, 0x3000);			/* HIF Wakeup */
	a6_put(base, 0x60010038, 0x00003000);	/* Edge Trig Ev Clr */
	a6_put(base, 0x60010014, mask);		/* Interrupt Reset */
}

#endif /* !_idshw_rgb_h */
