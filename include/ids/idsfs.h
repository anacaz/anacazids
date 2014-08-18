#ifndef _idsfs_h
#define _idsfs_h
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
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 060719-0829-090612-17
 */

/*
 * One directory entry at the start of the files ram carve area.
 *
 * For LOG FILES the linkp pointer references the log_menu.
 */
typedef struct ids_dir
{
	void		*linkp;		/* Linked reference pointer */
	unsigned long	base_addr;	/* From the GET_RAM() macro */
	unsigned long	load_addr;	/* Current available location */
	unsigned int	entries;	/* Number of files (itemcount) */
	unsigned long	limit_addr;	/* Last address + 1 */
} ids_dir_t;

extern ids_dir_t dir_tbl[];

/*
 * Defined in fileio.c:
 *
 * Referenced in menu.c:
 */
extern ids_dir_t tls_dir;		/* TLS file directory locator */
extern ids_dir_t log_dir;		/* LOG file directory locator */

/*
 * ASCII name
 * Number of test elements in list
 * Size of list payload in bytes
 *
 * NOTE: 1.) Remove pad -- 8 bytes.
 */
typedef struct list_hdr
{
	char		name[32];	/* Test list name */
	/*
	 * "IDS\0", -- currently this but maybe ...
	 * "\001\002\003\004" -- something like this
	 */
	char		magic[4];	/* Test list identifier */
	/*
	 * CAUTION!!!  Because of the limitation of the menu
	 * structure in the header file src/menu.h for only
	 * 16 items and one of the items has to be the "RUN"
	 * item; there can only be 15 tests max!!!
	 */
	char		tests[4];	/* Number of tests in test list */
	char		size[16];	/* Size in bytes including payload */
	/*
	 * Change to counter 06-01-2009
	 */
	char		cntr[8];	/* PAD to 64 bytes */
} list_hdr_t;

/*
 * ASCII name
 * ASCII pass vector
 * ASCII fail vector
 * main()		-- deprecated
 * Argument vector
 *
 * NOTE: 1.) Change name, pass, and fail to 32 bytes to match the
 * 	     internal structures in src/menu.h.			     -- DONE
 * 	 2.) Remove appl -- 16 bytes.                                -- DONE
 * UPDATE: appl[16] became cntr1[8] and cntr2[8].                    -- DONE
 * 	 3.) Change argc to 4 bytes -- enforce "15" as maximum.	     -- DONE
 * 	 4.) Remove pad -- 56 bytes.				     -- DONE
 * 	 5.) Change help to 128 bytes to match src/menu.h help size.
 *	 6.) Change size from 16 to 8.				     -- DONE
 * UPDATE: changed size from 16 to 8.				     -- DONE
 *	 7.) Remove argv and leave to be build by mktest
 *	     (remove 32 byte limit).
 */
typedef struct test_hdr
{
	char		name[32];	/* Test name */
	char		pass[32];	/* Pass vector */
	char		fail[32];	/* Fail vector */
	char		size[8];	/* Size in bytes including payload */
	char		cntr1[8];	/* Changed from appl[16] 06-01-2009 */
	char		cntr2[8];	/* Changed from appl[16] 06-01-2009 */
	char		argc[4];	/* Argument count */
	char		argv[15][32];	/* Argument vector */
#ifdef COMMENT
	char		help[256];	/* Help Message (256 bytes) */
	char		_[56];		/* PAD to 768 bytes */
#endif /* COMMENT */
} test_hdr_t;

/*
 * From fileio.c
 */
void fileio_dir(ids_dir_t *dirp);

list_hdr_t *next_tls(list_hdr_t *tlsp);	/* Return pointer to next tls header */
test_hdr_t *next_ids(test_hdr_t *idsp);	/* Return pointer to next ids header */

int ids_bootelf(unsigned long addr, int argc, char *argv[]);

void cp_loader(void *, unsigned long, const void *, int);
void a6_loader(void *, unsigned long , const void *, int);

/*
 * FLASH flash.c
 */
void ids_flash_store(unsigned char *buffer, int count);
void ids_flash_erase(void);

#endif /* !_idsfs_h */
