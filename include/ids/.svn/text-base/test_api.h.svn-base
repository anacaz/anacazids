#ifndef _test_api_h
#define _test_api_h
/*
 * Integrated Diagnostics Subsystem
 *
 * Test API header file to be included by individual tests.
 *
 * rfa - 060718-0804-08-11-0906-090515-29-0610
 */

/*
 * General data and pointer reference types
 */
typedef union
{
	unsigned char		*c;
	unsigned short		*s;
	unsigned long		*l;
	unsigned long long	*ll;
} ref_types_u;

typedef union
{
	unsigned char		c;
	unsigned short		s;
	unsigned long		l;
	unsigned long long	ll;
} data_types_u;

/*
 * Pattern Generator Definitions
 *
 * lib_ids/patgen.c
 *
 * PVALUE
 *
 * Pattern Value:  The pattern value is used to control patter data
 *                 and pattern lists.
 */
typedef struct pvalue
{
#define PVF_INIT	0x0001		/* Initialized */
#define PVF_LIST	0x0010		/* List data */
#define PVF_END		0x8000		/* Last in squence, no more */
#define PVF_ALL		0xFFFF
#define TST_PVF(X,F)	(((pvalue_t)X).f & (F))
#define SET_PVF(X,F)	(((pvalue_t)X).f |= (F))
#define CLR_PVF(X,F)	(((pvalue_t)X).f &= ~(F))
	unsigned short	f;			/* Flags */
#define PG_DATA_TYPE(X)	(((pvalue_t)X).z)
	int		z;			/* Data type size */
	/*
	 * List control, including hamming
	 * patterns.
	 */
	struct
	{
#define PG_LIST_INDEX(X)(((pvalue_t)X).list.n)
		int 	n;			/* List index */
#define PG_LIST_LEN(X)	(((pvalue_t)X).list.z)
		int	z;			/* List size */
#define PG_LIST(X,type)	((unsigned type *)((pvalue_t)X).list.p)
		void	*p;			/* List */
	} list;
#define PG_LIST_NEXT(X,type)	PG_LIST(X,type)[PG_LIST_INDEX(X)]
	/*
	 * The actual data value resides here.
	 */
	union
	{
#define CHAR(X)		(((pvalue_t)X).v.c)
#define SHORT(X)	(((pvalue_t)X).v.s)
#define LONG(X)		(((pvalue_t)X).v.l)
#define LONGLONG(X)	(((pvalue_t)X).v.ll)
		unsigned char		c;	/*  8-bit */
		unsigned short		s;	/* 16-bit */
		unsigned long		l;	/* 32-bit */
		unsigned long long	ll;	/* 64-bit */
	} v;
} pvalue_t;

/*
 * PVALUE manipulation macros.
 */
#define PG_PROC_LIST(Y) \
do \
{ \
	next = PG_LIST_INDEX(dataset); \
	while (next < PG_LIST_LEN(dataset)) \
	{ \
		next = PG_LIST_INDEX(dataset); \
	} \
} while (0)

#define SET_PG_LIST(X) \
do \
{ \
	if (X.z == sizeof(char)) \
		CHAR(X) = PG_LIST(X,char)[PG_LIST_INDEX(X)]; \
	else if (X.z == sizeof(short)) \
		SHORT(X) = PG_LIST(X,short)[PG_LIST_INDEX(X)]; \
	else \
		LONG(X) = PG_LIST(X,long)[PG_LIST_INDEX(X)]; \
} while (0)

#define SET_PV(X,N) \
do \
{ \
	if (X.z == sizeof(char)) \
		CHAR(X) = (unsigned char)(N); \
	else if (X.z == sizeof(short)) \
		SHORT(X) = (unsigned short)(N); \
	else \
		LONG(X) = (unsigned long)(N); \
} while (0)

#define GET_PV(X,V) \
do \
{ \
	if (X.z == sizeof(char)) \
		c##V = CHAR(X); \
	else if (X.z == sizeof(short)) \
		s##V = SHORT(X); \
	else \
		l##V = LONG(X); \
} while (0)

#define PUT_PV(X,V) \
do \
{ \
	if (X.z == sizeof(char)) \
		CHAR(X) = c##V; \
	else if (X.z == sizeof(short)) \
		SHORT(X) = s##V; \
	else \
		LONG(X) = l##V; \
} while (0)

#define EQU_PV(X,V) \
do \
{ \
	X.z = V.z; \
	X.list.n = V.list.n; \
	X.list.z = V.list.z; \
	X.list.p = V.list.p; \
	LONG(X) = LONG(V); \
} while (0)

#define ADD_PV(X,N) \
do \
{ \
	if (X.z == sizeof(char)) \
		X.v.c += (unsigned char)(N); \
	else if (X.z == sizeof(short)) \
		X.v.s += (unsigned short)(N); \
	else \
		X.v.l += (unsigned long)(N); \
} while (0)

#define SUB_PV(X,N) \
do \
{ \
	if (X.z == sizeof(char)) \
		X.v.c -= (unsigned char)(N); \
	else if (X.z == sizeof(short)) \
		X.v.s -= (unsigned short)(N); \
	else \
		X.v.l -= (unsigned long)(N); \
} while (0)

/*
 * Pattern types
 */
typedef enum
{
	DEFAULT,
	ZERO,		/* Fixed pattern zeros */
	ONES,		/* Fixed pattern ones */
	ASCEND,		/* Incrementing pattern */
	DESCND,		/* Decrementing pattern */
	REPEAT,		/* ??? */
	RANDOM,		/* Randomizing pattern */
	INVERT,		/* Modifier for fixed or dynamic patterns */
	COMP2,		/* Modifier for fixed or dynamic patterns */
	XOR,		/* Modifier for fixed or dynamic patterns */
	OR,		/* Modifier for fixed or dynamic patterns */
	AND,		/* Modifier for fixed or dynamic patterns */
	LSHIFT,		/* Modifier for fixed or dynamic patterns */
	RSHIFT,		/* Modifier for fixed or dynamic patterns */
	HAM,		/* Fixed list of patterns */
	ODD		/* Odd number of bits used for parity testing */
} pmod_t;

pvalue_t NOMASK;

pvalue_t spatgen(pvalue_t ivalue, pvalue_t seed, pmod_t modifier);
pvalue_t patgen(pvalue_t curval, pvalue_t mask, pmod_t modifier);
pmod_t modgen(char *requested);

/*
 * The following is the mailbox definitions for implementing and calling
 * mailbox services.
 *
 * lib_ids/mailbox.c
 *
 * rfa - 090508-0610
 *
 * NOP   -- not operation.
 * SUSP  -- drop into IDS from running test only to eventually return.
 * RELOC -- intended to allow dynamic relocation of u_boot_vectors.
 * SVC   -- denotes services available to running test.
 */
typedef enum mb_op
{
	MB_NOP,			/* No operation, return the ack count */
	MB_ACK,			/* Ack received, inc ack count */
	MB_PING,		/* Round turn no operation */
	MB_SUSP,		/* Suspend running test */
	MB_RELOC,		/* u_boot_vector table relocation */
	MB_GETFILE,		/* Down load file from tftp server */
	MB_PGMFILE,		/* Program device FPGA, CPLD, etc. */
	MB_PUTFILE,		/* Up load file to tftp server */
	MB_EXEC_CMD,
	MB_END			/* End mailbox session */
} mb_op_enum_t;

/*
 * From mailbox.c
 */
int mb_suspend(int id);
int (*mb_start(int (*callback)(mb_op_enum_t, ...)))(mb_op_enum_t, ...);
int mb_dispatch(mb_op_enum_t op, ...);

/*
 * SERVICES: From exec.c
 */
time_t gettime(void);
void settime(time_t time);
void exec_wait(int time);
int exec_pause(void);
int exec_ask(char *question);
int is_control_c(void);
void sleep(unsigned long);

/*
 * SERVICES: From fileio.c
 */
void lprintf(const char *, ...);

/*
 * COMMANDS: From cmd.c
 */
typedef struct cmd
{
	int	type;		/* 0 - Fast Display; 1 - Hold Display */
	char	*name;
	int	(*command)(int argc, char **argv);
	char	*usage;
} cmd_t;

int cli(int c);
cmd_t *findcmd(char *name);

#endif /* !_test_api_h */
