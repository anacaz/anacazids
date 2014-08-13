/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 090723
 *
 * This file contains the IDS descriptions for the RGB TCM HW.
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#include <idshw_rgb.h>

#define FORWARD	/* */

void hub_aput(unsigned long base, unsigned long addr, unsigned long value)
{
	*((volatile unsigned short *)(base + 0x14)) =
		(unsigned short)(value & 0xFFFF);
	*((volatile unsigned short *)(base + 0x16)) = 
		(unsigned short)(value >> 16);
}

void hub_put(unsigned long base, unsigned long addr, unsigned long value)
{
	*((volatile unsigned short *)(base + 0x10)) =
		(unsigned short)(addr & 0xFFFF);
	*((volatile unsigned short *)(base + 0x12)) = 
		(unsigned short)(addr >> 16) & 0x03FF;
	*((volatile unsigned short *)(base + 0x14)) =
		(unsigned short)(value & 0xFFFF);
	*((volatile unsigned short *)(base + 0x16)) = 
		(unsigned short)(value >> 16);
}

unsigned long hub_aget(unsigned long base, unsigned long addr)
{
	unsigned long value;

	value = *((volatile unsigned short *)(base + 0x10));
	value |= *((volatile unsigned short *)(base + 0x12)) << 16;
	return(value);
}

unsigned long hub_get(unsigned long base, unsigned long addr)
{
	unsigned long value;
	unsigned short v1, v2;

	*((volatile unsigned short *)(base + 0x10)) =
		(unsigned short)(addr & 0xFFFF);
	*((volatile unsigned short *)(base + 0x12)) = 
		(unsigned short)(addr >> 16) & 0x03FF;
	v1 = *((volatile unsigned short *)(base + 0x14));
	v2 = *((volatile unsigned short *)(base + 0x16));
	v1 = *((volatile unsigned short *)(base + 0x14));
	v2 = *((volatile unsigned short *)(base + 0x16));
	value = (unsigned long)v2;
	value <<= 16;
	value |= (unsigned long)(v1 & 0xFFFF);
	return(value);
}

void a6_aput(unsigned long base, unsigned long addr, unsigned long value)
{
	a6_wrreg(base + 8, value);
}

void a6_put(unsigned long base, unsigned long addr, unsigned long value)
{
	a6_wrreg(base + 4, addr);
	a6_wrreg(base + 8, value);
}

unsigned long a6_aget(unsigned long base, unsigned long addr)
{
	return(a6_rdreg(base + 8));
}

unsigned long a6_get(unsigned long base, unsigned long addr)
{
	a6_wrreg(base + 4, addr);
	return(a6_rdreg(base + 8));
}

unsigned short mux_get(unsigned long addr)
{
	*((volatile unsigned short *)(0x80000000 + 0x8)) = 
		(unsigned short)(addr >> 16);
	*((volatile unsigned short *)(0x80000000 + 0xA)) =
		(unsigned short)(addr & 0xFFFF);
	return(*((volatile unsigned short *)(0x80000000 + 0xC)));
}

unsigned short mux_aget(unsigned long addr)
{
	return(*((volatile unsigned short *)(0x80000000 + 0xC)));
}

void mux_aput(unsigned long addr, unsigned short value)
{
	*((volatile unsigned short *)(0x80000000 + 0xC)) = value;
}

void mux_put(unsigned long addr, unsigned short value)
{
	*((volatile unsigned short *)(0x80000000 + 0x8)) =
		(unsigned short)(addr >> 16);
	*((volatile unsigned short *)(0x80000000 + 0xA)) =
		(unsigned short)(addr & 0xFFFF);
	*((volatile unsigned short *)(0x80000000 + 0xC)) = value;
}
