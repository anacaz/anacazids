/*
 * Integrated Diagnostics Subsystem
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
 * Pattern Generator
 *
 * This pattern generator is available to all tests to generate and step
 * through pattern signatures for injected test data.  The pattern generator
 * can also be used to create the expected data set for verification and
 * comparison purposes.
 *
 * rfa - 060905-06-12-090514-31-0604-10
 */
#include <common.h>
#include <test_api.h>

#define FORWARD	/**/

FORWARD void srand(unsigned int u, ...);
FORWARD unsigned int rand(void);
FORWARD pmod_t modgen(char *requested);

static unsigned char ham_c[] =
{
	0x00, 0xFF, 0xF0, 0xCC, 0xAA, 0x55
};

static unsigned short ham_s[] =
{
	0x0000, 0xFFFF, 0xFF00, 0xF0F0, 0xCCCC, 0xAAAA, 0x5555
};

static unsigned long ham_l[] =
{
	0x00000000, 0xFFFFFFFF, 0xFFFF0000, 0xFF00FF00,
	0xF0F0F0F0, 0xCCCCCCCC, 0xAAAAAAAA, 0x55555555
};

static unsigned long long ham_ll[] =
{
	0x0000000000000000LL, 0xFFFFFFFFFFFFFFFFLL,
	0xFFFFFFFF00000000LL, 0xFFFF0000FFFF0000LL,
	0xFF00FF00FF00FF00LL, 0xF0F0F0F0F0F0F0F0LL,
	0xCCCCCCCCCCCCCCCCLL, 0xAAAAAAAAAAAAAAAALL,
	0x5555555555555555LL
};

#ifdef COMMENT
static unsigned char generic_c[1];
static unsigned short generic_s[1];
static unsigned long generic_l[1];
static unsigned long long generic_ll[1];
#endif /* COMMENT */

pvalue_t spatgen(pvalue_t ivalue, pvalue_t seed, pmod_t modifier)
{
	CLR_PVF(ivalue, PVF_ALL);
	ivalue.f = 0;
	EQU_PV(ivalue, seed);
	PG_DATA_TYPE(ivalue) = PG_DATA_TYPE(seed);
	switch (modifier)
	{
	case ZERO: SET_PV(ivalue, 0); break;
	case ONES: SET_PV(ivalue, ~0); break;
	case RANDOM:
		srand(LONG(seed));
		SET_PV(ivalue, (unsigned long)rand());
		break;
	case ODD:
		break;
	case HAM:
		SET_PVF(ivalue, PVF_LIST);
		PG_LIST_INDEX(ivalue) = 0;
		switch (PG_DATA_TYPE(seed))
		{
		case sizeof(char):
			ivalue.list.p = (void *)ham_c;
			ivalue.list.z = sizeof(ham_c);
			break;
		case sizeof(short):
			ivalue.list.p = (void *)ham_s;
			ivalue.list.z = sizeof(ham_s) / PG_DATA_TYPE(seed);
			break;
		default:
		case sizeof(long):
			ivalue.list.p = (void *)ham_l;
			ivalue.list.z = sizeof(ham_l) / PG_DATA_TYPE(seed);
			break;
		case sizeof(long long):
			ivalue.list.p = (void *)ham_ll;
			ivalue.list.z = sizeof(ham_ll) / PG_DATA_TYPE(seed);
			break;
		}
		SET_PG_LIST(ivalue);
		break;
	case ASCEND:
	case DESCND:
	case REPEAT:
	case INVERT:
	case COMP2:
	case LSHIFT:
	case RSHIFT:
	case XOR:
	case OR:
	case AND:
	default:
		break;
	}
	/*
	 * Load the first value from the list.
	 */
	SET_PVF(ivalue, PVF_INIT);	/* Value has been initialized */
	return(ivalue);
}

pvalue_t patgen(pvalue_t curval, pvalue_t mask, pmod_t modifier)
{
	unsigned char cval;
	unsigned short sval;
	unsigned long lval;

	pvalue_t newval;

	/*
	 * NOTE!!!  Caller sync issue!!!
	 */
	PG_DATA_TYPE(mask) = PG_DATA_TYPE(curval);

	PG_DATA_TYPE(newval) = PG_DATA_TYPE(curval);	/* Set size */
	EQU_PV(newval, curval);				/* newval = curval */
	switch (modifier)
	{
	/*
	 * Fixed data sets.
	 */
	case ZERO:
		if (TST_PVF(curval, PVF_END))
			return(curval);
		SET_PVF(curval, PVF_END);
		SET_PV(newval, 0);
		break;
	case ONES:
		if (TST_PVF(curval, PVF_END))
			return(curval);
		SET_PVF(curval, PVF_END);
		SET_PV(newval, ~0);
		break;
	/*
	 * Operators, arithmetic and logical.
	 */
	case ASCEND: ADD_PV(newval, 1); break;
	case DESCND: SUB_PV(newval, 1); break;
	case REPEAT:
		break;
	case RANDOM:
		SET_PV(newval, (unsigned long)rand());
		break;
	case INVERT:
		GET_PV(curval, val);
		cval = ~cval;
		sval = ~sval;
		lval = ~lval;
		PUT_PV(newval, val);
		// PVALUE(newval) = ~PVALUE(curval);
		break;
	case COMP2:
		GET_PV(curval, val);
		cval = ~cval;
		sval = ~sval;
		lval = ~lval;
		PUT_PV(newval, val);
		ADD_PV(newval, 1);
		// PVALUE(newval) = ~PVALUE(curval) + 1;
		break;
	case LSHIFT:
		GET_PV(curval, val);
		cval = cval << 1;
		sval = sval << 1;
		lval = lval << 1;
		PUT_PV(newval, val);
		// PVALUE(newval) = PVALUE(curval) << 1;
		break;
	case RSHIFT:
		GET_PV(curval, val);
		cval = cval >> 1;
		sval = sval >> 1;
		lval = lval >> 1;
		PUT_PV(newval, val);
		// PVALUE(newval) = PVALUE(curval) >> 1;
		break;
	case XOR:
		GET_PV(curval, val);
		cval = cval ^ CHAR(mask);
		sval = sval ^ SHORT(mask);
		lval = lval ^ LONG(mask);
		PUT_PV(newval, val);
		// PVALUE(newval) = PVALUE(curval) ^ PVALUE(mask);
		break;
	case OR:
		GET_PV(curval, val);
		cval = cval | CHAR(mask);
		sval = sval | SHORT(mask);
		lval = lval | LONG(mask);
		PUT_PV(newval, val);
		// PVALUE(newval) = PVALUE(curval) | PVALUE(mask);
		break;
	case AND:
		GET_PV(curval, val);
		cval = cval & CHAR(mask);
		sval = sval & SHORT(mask);
		lval = lval & LONG(mask);
		PUT_PV(newval, val);
		// PVALUE(newval) = PVALUE(curval) & PVALUE(mask);
		break;
	/*
	 * List based selection.
	 */
	case HAM:
		if (TST_PVF(curval, PVF_END))
			return(curval);
		if (++PG_LIST_INDEX(newval) >= PG_LIST_LEN(newval))
		{
			/*
			 * The flags are not copied on EQU_PV
			 * therefore the flags must be set on
			 * the returned instance.
			 *
			 * This is intentional!  Do not alter!
			 */
			SET_PVF(curval, PVF_END);
			break;
		}
		switch (PG_DATA_TYPE(newval))
		{
		case sizeof(char):
			SET_PV(newval, PG_LIST_NEXT(newval,char));
			break;
		case sizeof(short):
			SET_PV(newval, PG_LIST_NEXT(newval,short));
			break;
		default:
		case sizeof(long):
			SET_PV(newval, PG_LIST_NEXT(newval,long));
			break;
		case sizeof(long long):
			SET_PV(newval, PG_LIST_NEXT(newval,long long));
			break;
		}
		break;
	default:
		break;
	}
	EQU_PV(curval, newval);				/* curval = newval */
	return(curval);
}

/*
 * Convert a menu parameter or argument variable from the caller to
 * the correct patgen modifier.
 */
pmod_t modgen(char *requested)
{
	if (!strcmp("zero", requested))
		return(ZERO);
	if (!strcmp("ones", requested))
		return(ONES);
	if (!strcmp("ascend", requested))
		return(ASCEND);
	if (!strcmp("descend", requested))
		return(DESCND);
	if (!strcmp("repeat", requested))
		return(REPEAT);
	if (!strcmp("random", requested))
		return(RANDOM);
	if (!strcmp("invert", requested))
		return(INVERT);
	if (!strcmp("comp2", requested))
		return(COMP2);
	if (!strcmp("xor", requested))
		return(XOR);
	if (!strcmp("or", requested))
		return(OR);
	if (!strcmp("and", requested))
		return(AND);
	if (!strcmp("lshift", requested))
		return(LSHIFT);
	if (!strcmp("rshift", requested))
		return(RSHIFT);
	if (!strcmp("ham", requested))
		return(HAM);
	if (!strcmp("odd", requested))
		return(ODD);
	return(DEFAULT);
}

static unsigned int m_w, m_z;

void srand(unsigned int u, ...)
{
	int v = 0;

	va_list args;

	va_start(args, u);

	v = va_arg(args, unsigned int);

	/*
	 * These values are not magical, just the default values
	 * Marsaglia used.  Any pair of unsigned integers should
	 * be fine.
	 */
	if (v != 0)
	{
		m_z = v;
		m_w = u; 		/* u can = 0 since v is != 0 */
	}
	else if (u != 0)
	{
		m_w = u;		/* v == 0 so don't set u = 0 */
		m_z = 362436069;	/* Default */
	}
	else
	{
		m_w = 521288629;	/* Default */
		m_z = 362436069;	/* Default */
	}
	va_end (args);
}

unsigned int rand()
{
	m_z = 36969 * (m_z & 65535) + (m_z >> 16);
	m_w = 18000 * (m_w & 65535) + (m_w >> 16);
	return((m_z << 16) + m_w);
}

#ifdef COMMENT
using System;

namespace TestSimpleRNG
{
	/// <summary>
	/// SimpleRNG is a simple random number generator based on 
	/// George Marsaglia's MWC (multiply with carry) generator.
	/// Although it is very simple, it passes Marsaglia's DIEHARD
	/// series of random number generator tests.
	/// 
	/// Written by John D. Cook 
	/// http://www.johndcook.com
	/// </summary>
	public class SimpleRNG
	{
		private static uint m_w;
		private static uint m_z;

		static SimpleRNG()
		{
			// These values are not magical,
			// just the default values Marsaglia used.
			// Any pair of unsigned integers should be fine.
			m_w = 521288629;
			m_z = 362436069;
		}

		// The random generator seed can be set three ways:
		// 1) specifying two non-zero unsigned integers
		// 2) specifying one non-zero unsigned integer
		//    and taking a default value for the second
		// 3) setting the seed from the system time

		public static void SetSeed(uint u, uint v)
		{
			if (u != 0) m_w = u; 
			if (v != 0) m_z = v;
		}

		public static void SetSeed(uint u)
		{
			m_w = u;
		}

		public static void SetSeedFromSystemTime()
		{
			System.DateTime dt = System.DateTime.Now;
			long x = dt.ToFileTime();
			SetSeed((uint)(x >> 16), (uint)(x % 4294967296));
		}

		// Produce a uniform random sample from the open interval (0, 1).
		// The method will not return either end point.
		public static double GetUniform()
		{
			// 0 <= u <= 2^32
			uint u = GetUint();
			// The magic number below is 1/(2^32 + 2).
			// The result is strictly between 0 and 1.
			return (u + 1) * 2.328306435454494e-10;
		}

		// Get normal (Gaussian) random sample with
		// mean 0 and standard deviation 1
		public static double GetNormal()
		{
			// Use Box-Muller algorithm
			double u1 = GetUniform();
			double u2 = GetUniform();
			double r = Math.Sqrt( -2.0*Math.Log(u1) );
			double theta = 2.0*Math.PI*u2;
			return r*Math.Sin(theta);
		}

		// Get normal (Gaussian) random sample with
		// specified mean and standard deviation
		public static double GetNormal(double mean, double standardDeviation)
		{
			return mean + standardDeviation*GetNormal();
		}

		// Get exponential random sample with mean 1
		public static double GetExponential()
		{
			return -Math.Log( GetUniform() );
		}

		// Get exponential random sample with specified mean
		public static double GetExponential(double mean)
		{
			return mean*GetExponential();
		}

		// This is the heart of the generator.
		// It uses George Marsaglia's MWC algorithm
		// to produce an unsigned integer.
		// See http://www.bobwheeler.com/statistics/Password/MarsagliaPost.txt
		private static uint GetUint()
		{
			m_z = 36969 * (m_z & 65535) + (m_z >> 16);
			m_w = 18000 * (m_w & 65535) + (m_w >> 16);
			return (m_z << 16) + m_w;
		}
	}
}
#endif /* COMMENT */
