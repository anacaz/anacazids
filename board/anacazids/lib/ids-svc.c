/*
 * tftp
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
 * Integrated Diagnostics Subsystem
 *
 * rfa - 090618-0715-21
 */
#include <common.h>
#include <test_api.h>
#include <ids_jmpvec.h>
#include <idshw_rgb.h>
#include <idsfs.h>
#include <menu.h>

/*
 * ids> program service loadaddr filename filesize
 */
int ids_svc_main(int argc, char **argv)
{
	return(0);
}

#ifdef IDS_SA
int _start(int argc, char **argv)
{
	int rv = 0;

	if (ids_svc_main(argc, argv))
		++rv;
#ifdef COMMENT
	mb_callback = mb_start(tftp_svc_callback);
#endif /* COMMENT */
	return(rv);
}
#endif /* IDS_SA */
