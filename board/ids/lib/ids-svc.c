/*
 * tftp
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
