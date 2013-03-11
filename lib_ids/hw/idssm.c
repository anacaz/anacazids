/*
 * Integrated Diagnostics Subsystem
 *
 * IDS hardware state machine.
 *
 * Copyright (c)2006-2009 Anacaz Networks, Inc., ALL RIGHTS RESERVED
 *
 * rfa - 061016-17-18
 */
#ifndef LINUX
#include <common.h>
#include <exports.h>
#endif /* !LINUX */
#include <idssm.h>

#define FORWARD	/* */

FORWARD static int sm_hw_startup(ids_sm_t *smp, void *p);
FORWARD static int sm_hw_init(ids_sm_t *smp, void *p);
FORWARD static int sm_sw_startup(ids_sm_t *smp, void *p);
FORWARD static int sm_sw_exec(ids_sm_t *smp, void *p);
FORWARD static int sm_hw_fault(ids_sm_t *smp, void *p);
FORWARD static int sm_dispatch(ids_sm_t *smp, sm_event_t event, void *p);

static char *states[] = 
{
	"PRESENT", "!INIT", "!BOOT", "READY", "BUSY", "FAULT", 0
};

static char *events[] =
{
	"RESET", "INIT", "BOOT", "RUN", "PASS", "FAIL", "IGNORE", "HALT", 0
};

/*
 * Public functions.
 */
void sm_show(ids_sm_t *smp, sm_event_t event, char *tag)
{
	printf("SM(%s):\t", tag);
	if (isstate(smp->curr_state))
		printf("STATE=%s\t", states[smp->curr_state]);
	else
		printf("STATE=%d\t\t", smp->curr_state);
	if (isevent(smp->last_event))
		printf("LAST=%s\t", events[smp->last_event]);
	else
		printf("LAST=%d\t\t", smp->last_event);
	if (isevent(event))
		printf("EVENT=%s\n", events[event]);
	else
		printf("EVENT=%d\n", event);
}

/*
 * This call allows a single event to be processed by the state machine.
 */
int sm_run_raw(ids_sm_t *smp, sm_event_t event, void *p)
{
	return(sm_dispatch(smp, event, p));
}

/*
 * This call attempts to advance the state machine to the ready state for
 * command processing.  If the state machine is already in the fault state
 * then proceed with command procesing regardless.
 *
 * Returns 
 */
int sm_run(ids_sm_t *smp, void *p)
{
	int status;

	switch (smp->curr_state)
	{
	case ST_PRES:
		if ((status = sm_hw_startup(smp, p)) == -1)
			break;
	case ST_NOINIT:
		if ((status = sm_hw_init(smp, p)) == -1)
			break;
	case ST_NOBOOT:
		if ((status = sm_sw_startup(smp, p)) == -1)
			break;
	case ST_READY:
		status = sm_sw_exec(smp, p);
		break;
	case ST_BUSY:
		return(1);
	case ST_FAULT:
		status = sm_hw_fault(smp, p);
		break;
	default:
		return(-1);
	}

	/*
	 * Drive the failures to the ST_FAULT state.
	 */
	if (status == -1)
		(void)sm_fail(smp, p);
	return(status);
}

int sm_pass(ids_sm_t *smp, void *p)
{
	return(sm_dispatch(smp, EV_PASS, p));
}

int sm_fail(ids_sm_t *smp, void *p)
{
	return(sm_dispatch(smp, EV_FAIL, p));
}

int sm_ignore(ids_sm_t *smp, void *p)
{
	return(sm_dispatch(smp, EV_IGNORE, p));
}

/*
 * Private functions.
 */
static int sm_hw_startup(ids_sm_t *smp, void *p)
{
	return(sm_dispatch(smp, EV_RESET, p));
}

static int sm_hw_init(ids_sm_t *smp, void *p)
{
	return(sm_dispatch(smp, EV_INIT, p));
}

static int sm_sw_startup(ids_sm_t *smp, void *p)
{
	return(sm_dispatch(smp, EV_BOOT, p));
}

static int sm_sw_exec(ids_sm_t *smp, void *p)
{
	int status;

	status = sm_dispatch(smp, EV_RUN, p);		/* Indicate BUSY */

	if (status == 1)
	{
		/*
		 * Software is not responding. ???
		 */
		return(sm_hw_startup(smp, p));
	}

	return(status);
}

/*
 * This is the bastard child.
 */
static int sm_hw_fault(ids_sm_t *smp, void *p)
{
	int status;

	status = sm_dispatch(smp, EV_RUN, p);
	/*
	 * Software is not responding. ???
	 */
	return(sm_hw_startup(smp, p));
}

static ids_sm_t *active = 0;

/*
 * State Machine Dispatch routine.
 *
 * BUG!!!  Hook up sm_watchdog for software timeouts.
 */
static int sm_dispatch(ids_sm_t *smp, sm_event_t event, void *p)
{
	sm_state_t old_state;

	if (!isstate(smp->curr_state))
		return(-1);
	old_state = smp->curr_state;
	if (smp->arcs[smp->curr_state][event])
	{
#ifndef LINUX
		if (setjmp(smp->env))
			return(1);		/* Thread terminated */
#endif /* !LINUX */
		/*
		 * Arc to the next state.
		 */
		active = smp;
		(*smp->arcs[smp->curr_state][event])(p);
		active = 0;
		if (smp->curr_state != old_state)
		{
			smp->last_event = event;
			return(0);
		}
	}
	return(-1);
}

void sm_watchdog(int reason)
{
#ifndef LINUX
	if (active)
		longjmp(active->env, 1);
#endif /* !LINUX */
}
