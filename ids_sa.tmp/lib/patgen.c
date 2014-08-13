/*
 * Integrated Diagnostics Subsystem
 *
 * rfa - 060905-06-12
 */
#include <test_api.h>

extern long random(void);
extern void srandom(unsigned seed);

static pvalue_t ivalue;

void spatgen(pvalue_t seed, pmod_t modifier)
{
	switch (modifier)
	{
	case ZERO:	PG_REFLONG(ivalue) = 0;		       break;
	case ONES:	PG_REFLONG(ivalue) = ~0;	       break;
	case ASCEND:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case DESCND:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case REPEAT:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case RANDOM:
		PG_REFLONG(ivalue) = PG_REFLONG(seed);
		srandom(PG_REFLONG(ivalue));
		break;
	case INVERT:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case COMP2:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case LSHIFT:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case RSHIFT:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case XOR:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case OR:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	case AND:	PG_REFLONG(ivalue) = PG_REFLONG(seed); break;
	}
}

pvalue_t patgen(pvalue_t seed, pvalue_t mask, pmod_t modifier)
{
	pvalue_t value;

	switch (modifier)
	{
	case ZERO:	PG_REFLONG(value) = 0;			   break;
	case ONES:	PG_REFLONG(value) = ~0;			   break;
	case ASCEND:	PG_REFLONG(value) = PG_REFLONG(seed) + 1;  break;
	case DESCND:	PG_REFLONG(value) = PG_REFLONG(seed) - 1;  break;
	case REPEAT:	PG_REFLONG(value) = PG_REFLONG(seed);	   break;
	case RANDOM:	PG_REFLONG(value) = random();		   break;
	case INVERT:	PG_REFLONG(value) = ~PG_REFLONG(seed);	   break;
	case COMP2:	PG_REFLONG(value) = ~PG_REFLONG(seed) + 1; break;
	case LSHIFT:	PG_REFLONG(value) = PG_REFLONG(seed) << 1; break;
	case RSHIFT:	PG_REFLONG(value) = PG_REFLONG(seed) >> 1; break;
	case XOR:
		PG_REFLONG(value) = PG_REFLONG(seed) ^ PG_REFLONG(mask);
		break;
	case OR:
		PG_REFLONG(value) = PG_REFLONG(seed) | PG_REFLONG(mask);
		break;
	case AND:
		PG_REFLONG(value) = PG_REFLONG(seed) & PG_REFLONG(mask);
		break;
	}
	return(value);
}

#ifdef COMMENT
void test(void)
{
	memory_test(0xB1000, 0x8000, lfsr(memory_catch));
}

int lfsr(int (*operator)(pvalue_t x), condition)
{
	int result;
	pvalue_t x;

	for (x = patgen(x, ZERO); condition; x = patgen(x, ASCEND))
	{
		x = patgen(patgen(patgen(x, XOR), COMP2), LSHIFT);
		if ((result = (*operator)(x)) == -1)
			return(result);
	}
	return(result);
}

int memory_catch(pvalue_t x)
{
	return(0);
}
#endif /* COMMENT */
