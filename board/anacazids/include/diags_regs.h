#ifndef _ids_regs_h
#define _ids_regs_h
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
 */

#ifdef COMMENT
#define	RW	0	/* Read/Write register */
#define	RO	1	/* Read only register */
#define	WO	2	/* Write only register */
#define	RC	3	/* Read/Write 1 to clear */
#define	RS	4	/* Read/Write 1 to set */
#endif /* COMMENT */

typedef enum
{
	RW,		/* Read and write register */
	RO,		/* Read only register */
	WO,		/* Write only register */
	RC,		/* Read/Write 1 to clear */
	RS		/* Read/Write 1 to set */
} reg_enum_t;

typedef struct ids_regs
{
	char		*name;
	reg_enum_t	type;
	unsigned long	offset;
	unsigned long	init_val;
	unsigned long	rw_mask;
} ids_regs_t;

typedef struct _addr_regs
{
	unsigned long *reg;
	unsigned long rw_mask;
	char *name;
} ADDR_REGS;

/*
 * reg test operations
 */
typedef enum
{
	REG_READ,	/* 0 */
	RESET_TEST,	/* 1 */
	WRITE_TEST,	/* 2 */
	ADDR_TEST,	/* 3 */
	ISR_TEST	/* 4 */
} pops_t;

typedef int ptests_t;

#endif /* !_ids_regs_h */
