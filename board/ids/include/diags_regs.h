/*
 * Integrated Diagnostics Subsystem
 */
#ifndef _ids_regs_h
#define _ids_regs_h

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
