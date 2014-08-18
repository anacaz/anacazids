#ifndef _idssm_h
#define _idssm_h
/*
 * Integrated Diagnostics Subsystem
 *
 * IDS hardware state machine.
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
 * rfa - 061013-16-17
 *
 *     reset --> init hw --> boot sw --> (config sw) --> run
 *
 * pres --> noinit --> noboot --> ready
 */

#ifndef LINUX
typedef vu_char *jmp_ctx;

int ppc_setjmp(jmp_ctx env);
void ppc_longjmp(jmp_ctx env, int val);

#define setjmp ppc_setjmp
#define longjmp ppc_longjmp
#else /* !LINUX */
#define setjmp
#define longjmp
#endif /* !LINUX */

typedef enum sm_event
{
	EV_NOEVENT = -1,
	EV_RESET = 0,	EV_INIT,	EV_BOOT,	EV_RUN,
	EV_PASS,	EV_FAIL,	EV_IGNORE,	EV_HALT,
	EV_MAX
} sm_event_t;

typedef enum sm_state
{
	ST_NOSTATE = -1,
	ST_PRES = 0,	ST_NOINIT,	ST_NOBOOT,
	ST_READY,	ST_BUSY,	ST_FAULT,
	ST_MAX
} sm_state_t;

#define isstate(st)	    (ST_PRES <= (st) && (st) < ST_MAX)
#define isevent(ev)	    (EV_RESET <= (ev) && (ev) < EV_MAX)
#define setstate(x,st)	    ((x) = (isstate(st) ? st : (x)))

typedef void (*sm_arcs_t[ST_MAX][EV_MAX])(void *);

#define ARC_PRESENT(name){ name##_reset, 0,0,0,0, name##_fail, 0,0 }
#define ARC_NOINIT(name) { name##_reset, name##_init, 0,0,0, name##_fail, 0,0 }
#define ARC_NOBOOT(name) { name##_reset, 0, name##_boot, 0,0, name##_fail, 0,0 }
#define ARC_READY(name)  { 0,0,0, name##_run, 0,0,0,0 }
#define ARC_BUSY(name)	 { name##_reset, 0,0,0, name##_pass, name##_fail, 0, name##_halt }
#define ARC_FAULT(name)  { name##_reset, 0,0,0,0,0, name##_ignore, name##_halt }

typedef struct ids_sm
{
	sm_state_t	curr_state;
	sm_event_t	last_event;
	sm_arcs_t	arcs;
#ifndef LINUX
	jmp_ctx		env;
#endif /* !LINUX */
} ids_sm_t;

#define SM_INSTANCE(name) \
{ \
	ST_NOSTATE, \
	EV_NOEVENT, \
	{ \
		ARC_PRESENT(name), \
		ARC_NOINIT(name), \
		ARC_NOBOOT(name), \
		ARC_READY(name), \
		ARC_BUSY(name), \
		ARC_FAULT(name) \
	} \
}

void sm_show(ids_sm_t *smp, sm_event_t event, char *tag);
int sm_run_raw(ids_sm_t *smp, sm_event_t event, void *p);
int sm_run(ids_sm_t *smp, void *p);
int sm_pass(ids_sm_t *smp, void *p);
int sm_fail(ids_sm_t *smp, void *p);
int sm_ignore(ids_sm_t *smp, void *p);
void sm_watchdog(int reason);

#endif /* !_idssm_h */
