/*
 * Integrated Diagnostics Subsystem
 *
 * IDS <--> TLS/IDS/ELF application mailbox execution/status facility.
 *
 * Created to remove the jmpvec structure that now exists.  A simple
 * dynamic linked library by stashing the addresses of oft used u-boot
 * routines in well known memory locations.  The mailbox system replaces
 * that with ONE routine address, mb_start, in ONE well known location.
 *
 *      MB_NOP --> return ack count
 *      MB_ACK --> inc ack count and return ack count
 *     MB_PING --> ack the ping using the callback
 *     MB_SUSP --> exec the suspend function
 *    MB_RELOC --> move an IDS memory serivce region to somewhere else
 *  MB_GETFILE --> tftp download file to memory
 *  MB_PGMFILE --> program file in memory to device
 *  MB_PUTFILE --> tftp upload file from memory
 * MB_EXEC_CMD --> execute program found in the command table
 *      MB_END --> terminate mailbox session with application
 *
 * rfa - 090506-07-08
 */
#include <common.h>
#include <idsio.h>
#include <idsfs.h>
#include <ids_jmpvec.h>
#include <test_api.h>		/* Mailbox definitions are in test_api.h */
#include <menu.h>

#define FORWARD	/**/

FORWARD int mb_suspend(int id);
FORWARD int (*mb_start(int (*callback)(mb_op_enum_t, ...)))(mb_op_enum_t, ...);
FORWARD int mb_dispatch(mb_op_enum_t op, ...);

/*
 * This is required to send back something
 * other than an 'int' to the calling test.
 */
int (*mb_callback)(mb_op_enum_t, ...) = 0;

/*
 * Call for downloaded test to pause itself.
 */
int mb_suspend(int id)
{
	for (;;)
	{
		menu_input(&log_menu);
		printf("Type ESC/u to resume or ^C to terminate test\n");
		if (getchar() == '\003')
			return(-1);
	}
	return(id);
}

/*
 * Start a mailbox session between a downloaded test and the IDS core
 * via the u_boot_vector MB_START().
 *
 * The test application provides a service response callback vector for
 * IDS to callback on and mb_start returns a service dispatch vector for
 * the test application to call and make service requests.
 *
 * Once the test application is finished then the mailbox session can be
 * ended with the mb_dispatch service request MB_END.
 */
int (*mb_start(int (*callback)(mb_op_enum_t, ...)))(mb_op_enum_t, ...)
{
	mb_callback = callback;		/* Set the call back function */
	return(mb_dispatch);		/* Return dispatcher to the caller */
}

/*
 * IDS resident dispatcher.  Called by the running test after the address
 * of this function was returned from the mb_start function.
 */
int mb_dispatch(mb_op_enum_t op, ...)
{
	static int ack = 0;		/* The mailbox acknowledge */
	int status = 0;

	va_list args;

	va_start (args, op);

	if (!mb_callback)
		return(-1);

	switch (op)
	{
	case MB_ACK:
		++ack;			/* IDS has been ack'd */
		status = ack;		/* Return the ack count */
	case MB_NOP:
		break;
	case MB_PING:
		status = (*mb_callback)(MB_ACK, ack);
		break;
	case MB_SUSP:
		status = mb_suspend(0);
		break;
	case MB_RELOC:
		if ((status = (*mb_callback)(op, u_boot_vectors)) == -1)
			break;
		break;
	case MB_GETFILE:
	{
		int arg;

		/*
		 * Grab the do_tftp stuff from the SR release and put it here.
		 */
		arg = va_arg(args, int);	/* Example */
		break;
	}
	case MB_PGMFILE:
		break;
	case MB_PUTFILE:
		break;
	case MB_EXEC_CMD:
	{
		cmd_t *cmdp = 0;
		int argc;
		char **argv;

		argc = va_arg(args, int);
		argv = va_arg(args, char **);

		/*
		 * Look up the command.
		 */
		if (!(cmdp = findcmd(argv[0])))
		{
			printf("Command %s not found\n", argv[0]);
			(void)exec_pause();
			status = -1;
			break;
		}

		/*
		 * Jump to the command and pass the argument vector.
		 */
		status = (*cmdp->command)(argc, argv);
		(void)exec_pause();
		break;
	}
	case MB_END:
		status = (*mb_callback)(MB_END);
		ack = 0;
		mb_callback = (int (*)(mb_op_enum_t, ...))0;
		break;
	}
	va_end (args);
	return(status);
}
