#ifndef dummy_scansup_h
#define dummy_scansup_h

#include "postgres.h"

#define YY_BREAK if (yyextra->_seen_error) { yyterminate(); } break;

#endif
