#ifndef dummy_scansup_h
#define dummy_scansup_h

#include "postgres.h"

/* just a place for scan.l globals that need to be accessible outside */

extern int seen_error;

#define YY_BREAK if (seen_error) { yyterminate(); } break;

#endif
