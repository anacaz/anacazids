/*
 * Integrated Diagnostics Subsystem
 *
 * This file contains the common file creation and manipulation functions
 * and routines for TLS files and LOG files.
 *
 * Copyright (c)2006-2009 Anacaz Networks Inc., ALL RIGHTS RESERVED
 *
 * rfa - 060724-0816-17-25-090518-0612-16-17-0727-28-29
 */
#ifdef LINUX
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <termios.h>
char BootFile[128];
typedef enum { BOOTP, RARP, ARP, TFTP, DHCP, PING, DNS, NFS, CDP, NETCONS, SNTP } proto_t;
unsigned long NetBootFileXferSize;
#else /* LINUX */
#include <common.h>				/* extern load_addr */
#include <net.h>				/* extern BootFile */
#include <linux/types.h>
#include <linux/stat.h>
#include <linux/ctype.h>
#include <idsio.h>
#endif /* LINUX */
#include <idsfs.h>
#include <menu.h>
#include <elf.h>
#include <idshw.h>

extern void setenv(char *, char *);

#define FORWARD	/**/

FORWARD void fileio_init(void);
FORWARD void fileio_tftp(menu_t *p);
FORWARD void fileio_xmodem(menu_t *p);
FORWARD void fileio_upload(menu_t *p);
FORWARD void fileio_dir(ids_dir_t *dirp);
FORWARD void menu_show_appl(menu_t *p);

FORWARD list_hdr_t *next_tls(list_hdr_t *tlsp);
FORWARD test_hdr_t *next_ids(test_hdr_t *idsp);

FORWARD unsigned long ids_load_elf_image(unsigned long, unsigned long,
	void (*loader)(void *, unsigned long, const void *, int));
FORWARD int ids_valid_elf_image (unsigned long addr);
FORWARD int ids_bootelf(unsigned long addr, int argc, char *argv[]);

FORWARD void cp_loader(void *, unsigned long, const void *, int);
FORWARD void a6_loader(void *, unsigned long , const void *, int);

/*
 * This table must have the identical ordering as the ram_carve_menu
 * in menutab.c.  Menus keep all data as ASCII data including numeric
 * values.  The directory table keeps values in digital numberic form.
 *
 *	INST_PARM(U-BOOT,    0x00200000, U-Boot memory region),
 *	INST_PARM(MENU POOL, 0x00201000, Menu Pool memory region),
 *	INST_PARM(LOG FILES, 0x00240000, Log Files memory region),
 *	INST_PARM(RAM,       0x00300000, Scratch RAM memory region),
 *	INST_PARM(TLS FILES, 0x10000000, .tls download memory region),
 */
ids_dir_t dir_tbl[] =
{
	{ 0 },					/* UBOOT */
	{ &list_menu,		0, 0, 0, 0 },	/* MENUPOOL */
	{ &log_menu,		0, 0, 0, 0 },	/* LOGFILES */
	{ 0 },					/* UNUSED2 */
	{ &file_mgr_menu,	0, 0, 0, 0 },	/* TLSFILES */
	{ 0 }
};

ids_dir_t log_dir;		/* LOG file directory locator */

/*
 * TLS FILE MANAGER:
 *
 * Base location set by ram_carve_menu TLS FILES.  See GET_RAM() useage
 * below.
 *
 * The current available load address is held in 'load_addr'.
 *
 * The current number of TLS files in memory is held in 'dir_tbl[5].entries'.
 */
void fileio_init(void)
{
	sprintf(tftp_menu.itemlist[FILEIO_GATEWAY].data.cache,
		"%s", getenv("gatewayip"));
	sprintf(tftp_menu.itemlist[FILEIO_NETMASK].data.cache,
		"%s", getenv("netmask"));
	sprintf(tftp_menu.itemlist[FILEIO_HOSTNAME].data.cache,
		"%s", getenv("hostname"));
	sprintf(tftp_menu.itemlist[FILEIO_ROOTPATH].data.cache,
		"%s", getenv("rootpath"));
	sprintf(tftp_menu.itemlist[FILEIO_IPADDR].data.cache,
		"%s", getenv("ipaddr"));
	sprintf(tftp_menu.itemlist[FILEIO_SERVER].data.cache,
		"%s", getenv("serverip"));
	sprintf(tftp_menu.itemlist[FILEIO_DNS].data.cache,
		"%s", getenv("dnsip"));
	sprintf(tftp_menu.itemlist[FILEIO_DOMAIN].data.cache,
		"%s", getenv("domain"));

	/*
	 * MENUPOOL resource initialization.
	 */
	DIR_TBL(MENUPOOL).linkp		= &list_menu;
	DIR_TBL(MENUPOOL).base_addr	= GET_RAM(MENUPOOL);
	DIR_TBL(MENUPOOL).load_addr	= GET_RAM(MENUPOOL);
	DIR_TBL(MENUPOOL).entries	= 0;

	/*
	 * TLS file initialization.
	 */
	DIR_TBL(TLSFILES).linkp		= &file_mgr_menu;
	DIR_TBL(TLSFILES).base_addr	= GET_RAM(TLSFILES);
	DIR_TBL(TLSFILES).load_addr	= GET_RAM(TLSFILES);
	DIR_TBL(TLSFILES).entries	= 0;

	/*
	 * LOG file initialization.
	 */
	log_dir.linkp = &log_menu;
	log_dir.entries = 0;
	log_dir.base_addr = GET_RAM(LOGFILES);
	log_dir.load_addr = GET_RAM(LOGFILES);
	log_dir.limit_addr = GET_RAM(MEMORY);

	DIR_TBL(LOGFILES).linkp = &log_menu;
	DIR_TBL(LOGFILES).base_addr = GET_RAM(LOGFILES);
	DIR_TBL(LOGFILES).load_addr = GET_RAM(LOGFILES);
	DIR_TBL(LOGFILES).entries = 0;
	DIR_TBL(LOGFILES).limit_addr = GET_RAM(MEMORY);

#ifdef COMMENT
	sprintf(file_mgr_menu.itemlist[FILEIO_LOADADDR].data.cache,
		"0x%08X", DIR_TBL(TLSFILES).load_addr);
#endif /* COMMENT */
}

void fileio_env(menu_t *p)
{
	/*
	 * Update U-Boot environment variables from the tftp_menu.
	 */
	setenv("gatewayip", p->itemlist[FILEIO_GATEWAY].data.cache);
	setenv("netmask",   p->itemlist[FILEIO_NETMASK].data.cache);
	setenv("hostname",  p->itemlist[FILEIO_HOSTNAME].data.cache);
	setenv("rootpath",  p->itemlist[FILEIO_ROOTPATH].data.cache);
	setenv("ipaddr",    p->itemlist[FILEIO_IPADDR].data.cache);
	setenv("serverip",  p->itemlist[FILEIO_SERVER].data.cache);
	setenv("dnsip",	    p->itemlist[FILEIO_DNS].data.cache);
	setenv("domain",    p->itemlist[FILEIO_DOMAIN].data.cache);
}

/*
 * SERVICES
 *
 * TFTP service routine.
 *
 * A down loaded tls file containing an ids test with a menu item of
 * the following will lead to this service:
 *
 * service=tftp
 */
void fileio_tftp_svc(menu_t *p)
{
	unsigned long memory;
	item_t *itemp;
	int filesize;

	fileio_env(&tftp_menu);
	memory = GET_RAM(MEMORY);
	if (!(itemp = find_item(p, "filename")))
	{
		printf("No filename specified\n");
		return;
	}
	sprintf(BootFile, "%s", itemp->data.cache);
	/*
	 * Somewhere in u-boot where NetLoop() lives there is a global
	 * variable 'load_addr'.  This needs to be set in order to
	 * specifiy the download address.  Otherwise it defaults to
	 * the first location in ram after u-boot itself.
	 */
	load_addr = memory;
	printf("[ TFTP %s file %s @ %08X ]\n\t", p->name, BootFile, load_addr);
	if ((filesize = NetLoop(TFTP)) == -1)
	{
		printf("\t[ TFTP failed! ]\n");
	}
	if (!(itemp = find_item(p, "filesize")))
	{
		return;
	}
	sprintf(itemp->data.cache, "%d", filesize);
	printf("\t[ %s tftp file %s filesize %s ]\n",
		p->name, BootFile, itemp->data.cache);
}

/*
 * Download TLS files.
 */
void fileio_tftp(menu_t *p)
{
	char save_mode[64];
	static unsigned long ram_load_addr = 0;
	list_hdr_t *tlsp;
	char *cp;
	int file;
	ids_dir_t *tlsdp;
	menu_t *file_mgr;

	get_ids_mode(save_mode);
	fileio_env(&tftp_menu);
	tlsdp = &DIR_TBL(TLSFILES);
	file_mgr = p->itemlist[0].menu;
//	tlsdp->entries = 0;	/* Force restart */
//	for (file = 0; file < file_mgr_menu.itemcount; ++file)
	for (file = 0; file < file_mgr->itemcount; ++file)
	{
		/*
		 * Set the file name to download.
		 */
		sprintf(BootFile, "%s",
			file_mgr->itemlist[file].data.cache);
//			file_mgr_menu.itemlist[file].data.cache);
		/*
		 * Always start from the begining and use the itemcount
		 * variable to advance to the next avaiable location.
		 *
		 * This is ineffiecient but very reliable.
		 */
		tlsdp->load_addr = tlsdp->base_addr;

		if (!(cp = strchr(BootFile, '.')))
			continue;
		if (strcmp(cp, ".tls"))
			continue;
		if (!strcmp("default.tls", BootFile))
			continue;
		/*
		 * If the load_addr has changed then start over.
		 */
		if (tlsdp->load_addr != ram_load_addr)
		{
			tlsdp->entries = 0;
			ram_load_addr = tlsdp->load_addr;
		}
		else if (tlsdp->entries >= 15)
		{
			int c;

			if (strcmp("autorun", save_mode))
			{
				printf("Maximum number of test lists "
					" (%d) already loaded!!!\n",
					tlsdp->entries);
				printf("Do you want to clear list? [y] ");
				c = getchar();
				printf("\n");
				if (c != 'y' && c != 'Y')
					return;
			}
			tlsdp->entries = 0;
			file = -1;
			continue;			/* Restart */
		}
		if (tlsdp->entries)
		{
			register int index, limit;

			/*
			 * Advance the load_addr to the next available
			 * location to append the newly requested TLS file.
			 */
			tlsp = (list_hdr_t *)tlsdp->load_addr;
			limit = tlsdp->entries;
			for (index = 0; index < limit; ++index)
			{
				tlsp = next_tls(tlsp);
			}
			/*
			 * Update the load_addr with next avaiable location.
			 */
			tlsdp->load_addr = (unsigned long)tlsp;
		}
#ifdef COMMENT
		else
			++tlsdp->entries;
#endif /* COMMENT */
		++tlsdp->entries;

		/*
		 * Somewhere in u-boot where NetLoop() lives there is a global
		 * variable 'load_addr'.  This needs to be set in order to
		 * specifiy the download address.  Otherwise it defaults to
		 * the first location in ram after u-boot itself.
		 */
		load_addr = tlsdp->load_addr;
		if (NetLoop(TFTP) == -1)
		{
			printf("[ TEST LIST %d DOWNLOAD **** FAILED **** ]\n",
				tlsdp->entries);
#ifdef COMMENT
			if (--tlsdp->entries == 1)
#endif /* COMMENT */
				--tlsdp->entries;
			continue;
		}
		tlsp = (list_hdr_t *)tlsdp->load_addr;
		sprintf(tlsp->size, "%d", NetBootFileXferSize);
//		sprintf(file_mgr_menu.itemlist[file].name, "tls @ %08X %s",
		sprintf(file_mgr->itemlist[file].name, "tls @ %08X %s",
			tlsdp->load_addr, tlsp->size);
		// printf("File loaded size %s at 0x%X\n",
		//	tlsp->size, tlsdp->load_addr);
		tlsdp->load_addr = ram_load_addr;
		printf("[ TEST LIST %s %s %d DOWNLOAD **** PASSED **** ]\n",
			BootFile, file_mgr->itemlist[file].name, tlsdp->entries);
	}
	if (strcmp("autorun", save_mode))
		(void)exec_pause();
}

void fileio_xmodem(menu_t *p)
{
}

void fileio_upload(menu_t *p)
{
}

/*
 * Display directory record.
 */
void fileio_dir(ids_dir_t *dirp)
{
	printf("directory(%08X): entries(%d) base(%08X) load(%08X) size(%d)\n",
		dirp->linkp, dirp->entries, dirp->base_addr, dirp->load_addr,
		dirp->load_addr - dirp->base_addr);
	(void)exec_pause();
}

/*
 * LOG FILE MANAGEMENT
 *
 * The log name will be the start date and time of the first test.  Subsequent
 * test names for each log will be in the format of start date and time for
 * each test.  The first test name will be the same as the default log name.
 *
 * 1) should names be editable?
 * 2) how to upload ???
 * 3) save in flash ???
 *
 * Log name format:
 *
 * test-name:instance
 */

void menu_show_appl(menu_t *p)
{
	printf("\n");
	if (p)
	{
		char *cp;

		cp = (char *)p;
		while (strcmp(cp, "end-of-file"))
		{
			printf("%s", cp);
			cp = &cp[strlen(cp) + 1];
		}
	}
	else
		printf("menu_show_appl: log pointer is null\n");
	(void)exec_pause();
}

void menu_del_appl(menu_t *p)
{
	extern int exec_ask(char *question);

	printf("\n");
	if (!exec_ask("Do you really want to erase all log files?"))
		return;
	log_menu.itemcount = 0;
	log_dir.load_addr = log_dir.base_addr;
	printf("All logs have been cleared!!!\n");
	(void)exec_pause();
}

/*
 * Open the specified for viewing or transmittal purposes.
 */
int fileio_open(char *name)
{
	int log;

	for (log = 0; log < log_menu.itemcount; ++log)
	{
		/*
		 * Look for the log file name in the log_menu.
		 */
		if (!strcmp(name, log_menu.itemlist[log].name))
			return(log);
	}
	return(-1);
}

/*
 * Create a new log file to capture this master-list, list, or individual
 * test execution RUN.  All printf() output will be captured and recorded
 * in the newly created log file.  A time stamp will be recorded for any
 * and all detected failures and at threshold inflection points.
 */
static int fileio_log_active = 0;		/* BUG!!! */

int fileio_create(char *name)
{
	ids_dir_t *dirp;
	menu_t *p;

	p = &log_menu;

	dirp = (ids_dir_t *)&log_dir;
	if (p->itemcount >= (sizeof(p->itemlist) / sizeof(item_t)))
		return(-1);
	/*
	 * This needs to move to the test execution code
	 * in exec.c.  This should mimic the TLS downloader
	 * code in terms of advancing the log_dir.load_addr
	 * value to the next unused location after this log file
	 * is closed.
	 */
	p->itemlist[p->itemcount].menu = (menu_t *)dirp->load_addr;
	p->itemlist[p->itemcount].type = ITEM_LOG;
	p->itemlist[p->itemcount].func = menu_show_appl;
	/*
	 * Uniqueify the log name.
	 */
	sprintf(p->itemlist[p->itemcount].name, "%s:%d",
		name, p->itemcount + 1);
	++p->itemcount;
	++fileio_log_active;
	return(p->itemcount - 1);
}

/*
 * Log printf:  When an open log is active (ie; during a test execution
 * cycle) then lprintf will also print to the log file as well as the
 * console.
 *
 * NOTE(1):
 * When the cycle count is > 1 then only display all of the first cycle
 * in it's entirety and then display the cycle header and any 'failed'
 * items in order to conserve the log file buffer.
 *
 * NOTE(2):
 * Eventually the log files need to be tftp'd (syslog?) out of memory and
 * stored off target.
 */
void lprintf(const char *fmt, ...)
{
	va_list args;
	uint index;
	char printbuffer[CFG_PBSIZE];
	char *pbuf;

	va_start(args, fmt);
	/*
	 * For this to work, printbuffer must be
	 * larger than anything we ever want to print.
	 */
	pbuf = printbuffer;
	index = vsprintf(printbuffer, fmt, args);
	pbuf[index] = '\0';
	if (fileio_log_active)
	{
		ids_dir_t *dirp;

		dirp = (ids_dir_t *)&log_dir;
		/*
		 * BUG!!!  If log file overruns then stop logging.
		 */
		if (!(dirp->load_addr + index + 1 >= dirp->limit_addr))
		{
			memcpy((char *)dirp->load_addr, printbuffer, index);
			dirp->load_addr += index;
			*((volatile char *)dirp->load_addr) = '\0';
			++dirp->load_addr;
			if (dirp->load_addr >= dirp->limit_addr)
			{
				printf("<PANIC: LOG FILE OVERRUN!!!>\n");
				(void)getchar();
			}
		}
	}
	va_end(args);
	/* Send to desired file */
	printf(pbuf);
}

/*
 * At the end of a test execution RUN this function closes the log
 * file to any further additions.  The log file is now available to be
 * viewed as a detailed record of what happened and when.
 */
void fileio_close(int log)
{
	if (0 <= log && log < log_menu.itemcount)
	{
		char *time;
		ids_dir_t *dirp;
		int index;
		char *cp;

		dirp = (ids_dir_t *)&log_dir;
		cp = (char *)dirp->load_addr;
		sprintf(cp, "end-of-file");
		index = strlen(cp);
		dirp->load_addr += index;
		cp[index] = '\0';
		++dirp->load_addr;		/* Preserve the '\0' */
		/*
		 * This records the elapsed time for this test
		 * execution lists, list, or test for all cycles
		 * run.
		 */
		time = execution_menu.itemlist[EXEC_TIME].data.cache;
		sprintf(log_menu.itemlist[log].data.cache, "%s", time);
		fileio_log_active = 0;
	}
	else
		printf("fileio_close: invalid log id %d\n", log);
}

/*
 * Subordinate support routines start here.
 */

list_hdr_t *next_tls(list_hdr_t *tlsp)
{
	int size = (int)strtol(tlsp->size, (char **)0, 0);

	return((list_hdr_t *)&((unsigned char *)tlsp)[size]);
}

test_hdr_t *next_ids(test_hdr_t *idsp)
{
	int size = (int)strtol(idsp->size, (char **)0, 0);

	return((test_hdr_t *)&((unsigned char *)idsp)[size]);
}

/*
 * Interpreter command to boot an arbitrary ELF image from memory.
 */
int ids_bootelf(unsigned long addr, int argc, char *argv[])
{
	unsigned long rc;		/* Return value from user code  */

	if (!ids_valid_elf_image(addr))
		return(-1);
	addr = ids_load_elf_image(addr, 0, cp_loader);
#ifdef COMMENT
	printf("## Starting application at 0x%08lx ...\n", addr);
	/*
	 * QNX images require the data cache is disabled.
	 * Data cache is already flushed, so just turn it off.
	 * -- Enable for the POC only.
	 */
	if (dcache_status())
		dcache_disable();
#endif /* COMMENT */

	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */
	rc = (*(unsigned long (*)(int, char *[]))addr)(argc, &argv[0]);
#ifdef COMMENT
	printf("## Application terminated, rc = 0x%lx\n", rc);
#endif /* COMMENT */
	return(rc ? -1 : 0);
}

/*
 * Determine if a valid ELF image exists at the given memory location.
 * First looks at the ELF header magic field, the makes sure that it is
 * executable and makes sure that it is for a PowerPC.
 */
int ids_valid_elf_image (unsigned long addr)
{
	Elf32_Ehdr *ehdr;		/* Elf header structure pointer */

	ehdr = (Elf32_Ehdr *)addr;

	if (!IS_ELF(*ehdr))
	{
		printf ("## No elf image at address 0x%08lx\n", addr);
		return 0;
	}
	if (ehdr->e_type != ET_EXEC)
	{
		printf ("## Not a 32-bit elf image at address 0x%08lx\n", addr);
		return 0;
	}
#if 0
	if (ehdr->e_machine != EM_PPC)
	{
		printf ("## Not a PowerPC elf image at address 0x%08lx\n", addr);
		return 0;
	}
#endif
	return(1);
}

void ids_print_elf_hdr(Elf32_Ehdr *ehdr)
{
	int n;

	for (n = 0; n < EI_NIDENT; ++n)
	{
		printf("ehdr->e_ident[%d] = 0x%02X(%c)\n", n, ehdr->e_ident[n], isascii(ehdr->e_ident[n]) ? ehdr->e_ident[n] : '.');
	}

	printf("ehdr->e_type     =%04X\n", ehdr->e_type);
	printf("ehdr->e_machine  =%04X\n", ehdr->e_machine);
	printf("ehdr->e_version  =%08X(%d)\n", ehdr->e_version, ehdr->e_version);
	printf("ehdr->e_entry    =%08X\n", ehdr->e_entry);
	printf("ehdr->e_phoff    =%08X\n", ehdr->e_phoff);
	printf("ehdr->e_shoff    =%08X\n", ehdr->e_shoff);
	printf("ehdr->e_flags    =%08X\n", ehdr->e_flags);
	printf("ehdr->e_ehsize   =%04X(%d)\n", ehdr->e_ehsize, ehdr->e_ehsize);
	printf("ehdr->e_phentsize=%04X(%d)\n", ehdr->e_phentsize, ehdr->e_phentsize);
	printf("ehdr->e_phnum    =%04X\n", ehdr->e_phnum);
	printf("ehdr->e_shentsize=%04X(%d)\n", ehdr->e_shentsize, ehdr->e_shentsize);
	printf("ehdr->e_shnum    =%04X\n", ehdr->e_shnum);
	printf("ehdr->e_shstrndx =%04X\n", ehdr->e_shstrndx);
	printf("--------------------------\n");
}

#define bswap_16(x)	((((x) >> 8) & 0xFF) | (((x) & 0xFF) << 8))
#define bswap_32(x)	((((x) >> 16) & 0xFFFF) | (((x) & 0xFFFF) << 16))

/*
 * A very simple elf loader, assumes the image is valid, returns the
 * entry point address.
 */
unsigned long ids_load_elf_image(unsigned long addr, unsigned long off,
   void (*loader)(void *, unsigned long, const void *, int))
{
	Elf32_Ehdr *ehdr;		/* Elf header structure pointer     */
	Elf32_Shdr *shdr;		/* Section header structure pointer */
	unsigned char *strtab = 0;	/* String table pointer             */
	unsigned char *image;		/* Binary image pointer             */
	int i;				/* Loop counter                     */

	ehdr = (Elf32_Ehdr *)addr;
// printf("load_elf_image: ehdr=0x%X\n", ehdr);
	/*
	 * Find the section header string table for output info.
	 */
// printf("load_elf_image: addr=0x%X\n", addr);
// printf("load_elf_image: ehdr->e_shoff=0x%X\n", ehdr->e_shoff);
	shdr = (Elf32_Shdr *)(addr + ehdr->e_shoff +
		(ehdr->e_shstrndx * sizeof(Elf32_Shdr)));
// printf("load_elf_image: shdr=0x%X\n", shdr);
	strtab = 0;
	if (shdr->sh_type == SHT_STRTAB)
		strtab = (unsigned char *)(addr + shdr->sh_offset);
// printf("load_elf_image: strtab=0x%X shnum=%d\n", strtab, ehdr->e_shnum);
	/*
	 * Load each appropriate section.
	 */
	for (i = 0; i < ehdr->e_shnum; ++i)
	{
		shdr = (Elf32_Shdr *)(addr + ehdr->e_shoff +
			(i * sizeof(Elf32_Shdr)));
		if (!(shdr->sh_flags & SHF_ALLOC) || shdr->sh_addr == 0 ||
		    shdr->sh_size == 0)
		{
			continue;
		}
#if 0
		if (strtab)
		{
			printf("%sing %s @ 0x%08lx (%ld bytes)\n",
				shdr->sh_type == SHT_NOBITS ? "Clear" : "Load",
				&strtab[shdr->sh_name],
				(unsigned long)shdr->sh_addr,
				(long)shdr->sh_size);
		}
#endif
		if (shdr->sh_type == SHT_NOBITS)
		{
			memset((void *)(unsigned long)shdr->sh_addr, 0, shdr->sh_size);
		}
		else
		{
			image = (unsigned char *)(addr + shdr->sh_offset);
// printf("loader=%X\n", loader);
			(*loader)((void *)shdr->sh_addr, off,
				(const void *)image, shdr->sh_size);
#ifdef COMMENT
			memcpy((void *)(unsigned long)shdr->sh_addr,
				(const void *)image, shdr->sh_size);
#endif /* COMMENT */
		}
		// flush_cache(shdr->sh_addr, shdr->sh_size);
	}
	return(ehdr->e_entry);
}

/*
 * LOADERS
 *
 * Place all down loaders for specific hw here.
 */
void cp_loader(void *loc, unsigned long off, const void *image, int size)
{
	memcpy(loc, image, size);
	flush_cache((unsigned long)loc, size);
}

void a6_loader(void *loc, unsigned long off, const void *image, int size)
{
	unsigned long addr, limit;
	const unsigned long *p;

// printf("a6_loader: loc=%X off=%X image=%X size=%ld\n", loc, off, image, size);
	p = (const unsigned long *)image;
	addr = (unsigned long)loc;
	limit = addr + size;
// printf("a6_loader: addr=%X off=%X image=%X size=%d\n", loc, off, image, size);
	while (addr < limit)
	{
		a6_wrreg(off + 4, addr);
		a6_wrreg(off + 8, *p);
		addr += 4;
		++p;
	}
}
