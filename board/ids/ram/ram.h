#ifndef _ram_h
#define _ram_h

#define RAM_LOAD(X) \
do \
{ \
	int _index; \
\
	ram.X = memory; \
	_index = 0; \
	switch (overrun) \
	while (_index < count) \
	{ \
	case 0: ram.X[_index++] = pattern.X; \
	case 7: ram.X[_index++] = pattern.X; \
	case 6: ram.X[_index++] = pattern.X; \
	case 5: ram.X[_index++] = pattern.X; \
	case 4: ram.X[_index++] = pattern.X; \
	case 3: ram.X[_index++] = pattern.X; \
	case 2: ram.X[_index++] = pattern.X; \
	case 1: ram.X[_index++] = pattern.X; \
	} \
} while (0)

#define RAM_FILL(X) \
do \
{ \
	int _index; \
\
	ram.X = memory; \
	_index = 0; \
	switch (overrun) \
	while (_index < count) \
	{ \
	case 0: ram.X[_index++] = pattern.X; \
	case 7: ram.X[_index++] = pattern.X; \
	case 6: ram.X[_index++] = pattern.X; \
	case 5: ram.X[_index++] = pattern.X; \
	case 4: ram.X[_index++] = pattern.X; \
	case 3: ram.X[_index++] = pattern.X; \
	case 2: ram.X[_index++] = pattern.X; \
	case 1: ram.X[_index++] = pattern.X; \
	} \
} while (0)

#define RAM_EXCLUDE(X) \
do \
{ \
	int _index; \
\
	ram.X = memory; \
	_index = 0; \
	switch (overrun) \
	while (_index < count) \
	{ \
	case 0: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 7: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 6: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 5: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 4: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 3: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 2: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	case 1: if (ram.X[_index] == pattern.X) { return(-1); } ++_index; \
	} \
} while (0)

#define RAM_ADDR(X) \
do \
{ \
	unsigned long _mask; \
\
	for (_mask = 0x00000002L; _mask <= stop; _mask <<= 0x00000001L) \
	{ \
		*(unsigned long *)(start + _mask) = start | _mask; \
		printf("\t[ exclude:%08X ]\n", start | _mask); \
		if (start == _mask) \
			start &= ~_mask; \
		RAM_EXCLUDE(l); \
	} \
	for (_mask = 0x00000002L; (stop + _mask) <= (unsigned long)(memory + size); _mask <<= 0x00000001L) \
	{ \
		*(unsigned long *)(stop + _mask) = stop | _mask; \
		printf("\t[ exclude:%08X ]\n", stop | _mask); \
		RAM_EXCLUDE(l); \
	} \
} while (0)

#define RAM_FIND(X) \
do \
{ \
	int _index; \
\
	ram.X = memory; \
	_index = 0; \
	switch (overrun) \
	while (_index < count) \
	{ \
	case 0: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 7: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 6: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 5: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 4: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 3: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 2: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	case 1: if (ram.X[_index] != pattern.X) { return(-1); } ++_index; \
	} \
} while (0)

#define CACHE_FILL(X) \
do \
{ \
	int _index; \
\
	ram.X = memory; \
	_index = 0; \
	switch (overrun) \
	while (_index < count) \
	{ \
	case 0: data.X = ram.X[_index++]; \
	case 7: data.X = ram.X[_index++]; \
	case 6: data.X = ram.X[_index++]; \
	case 5: data.X = ram.X[_index++]; \
	case 4: data.X = ram.X[_index++]; \
	case 3: data.X = ram.X[_index++]; \
	case 2: data.X = ram.X[_index++]; \
	case 1: data.X = ram.X[_index++]; \
	} \
} while (0)

#define RAM_READBACK(X) \
do \
{ \
	int _index; \
\
	ram.X = memory; \
	_index = 0; \
	switch (overrun) \
	while (_index < count) \
	{ \
	case 0: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 7: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 6: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 5: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 4: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 3: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 2: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	case 1: ram.X[_index] = pattern.X; data.X = ram.X[_index]; \
		if (data.X != pattern.X) { return(-1); } ++_index; \
	} \
} while (0)

#define RAM_TITLE(title) \
do \
{  \
	printf("[ %s %s: %08X:%08X (%d) %d-BIT ACCESS ]\n", \
		#title, program, memory, memory + size, size, access * 8); \
} while (0)

ref_types_u ram;
data_types_u data;
data_types_u pattern;

#endif /* !_ram_h */
