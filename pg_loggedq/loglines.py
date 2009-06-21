# pg_loggedq.py
#
# the paul mozy 2009

import re
import time
from warnings import warn

class ThrownAwayWarning(UserWarning):
    pass

def syslog_to_loglines(linesource, wait_for_msgend=5000, check_for_dead=14):
    """
    Expects input lines from the given iterator of the form:

       May 20 15:28:42 hostname postgres[31800]: [8-1] 2009-05-20 15:28:42 MD
       T spartacus LOG:  duration: 0.375 ms  statement: SELECT * FROM machine
       s WHERE (machines.user_id = 1534352

       May 20 15:28:42 hostname postgres[31800]: [8-2]  AND NOT machines.dele
       ted) ORDER BY alias

    Returns a generator which outputs tuples representing reassembled log
    messages paired with backend PIDs, e.g.:

       (31800, "2009-05-20 15:28:42 MDT spartacus LOG:  duration: 0.375 ms  s
       tatement: SELECT * FROM machines WHERE (machines.user_id = 1534352\n A
       ND NOT machines.deleted) ORDER BY alias")

    @param wait_for_msgend If we have processed this many lines without
               seeing another message from pid X, consider its last message
               complete
    @param check_for_dead An exponent to be applied to 2. Checks will be made
               for expired backends every 2**check_for_dead lines.
    """

    fmt_re = re.compile(r'''
        ^
        [A-Z][a-z]{2} \s+ \d{1,2} \s+ \d{1,2}:\d{2}:\d{2} \s
        (?P<hostname>[a-zA-Z0-9-]+) \s
        (?P<progname>[a-zA-Z0-9.-]+)
        \[ (?P<pid>\d+) \]: \s
        \[ (?P<msgnum>\d+) - (?P<msgpart>\d+) \] \s
        (?P<msg>.*)
        $
    ''', re.X)

    backends = {}
    throwaway = 0
    linenum = 0
    mask = (1 << check_for_dead) - 1

    for line in linesource:
        linenum += 1

        # periodically clean out lines from backends which died (since
        # they never logged another message, we were never sure when the
        # last message finished)
        if linenum & mask == 0:
            threshhold = linenum - wait_for_msgend
            newbackends = {}
            for b, v in backends.iteritems():
                if v:
                    if v[0] < threshhold:
                        yield (b, '\n'.join(v[1]))
                    else:
                        newbackends[b] = v
            backends = newbackends

        # parse the line
        mch = fmt_re.match(line)
        if mch is None:
            #warn("threw away unparseable line %r" % line, ThrownAwayWarning)
            throwaway += 1
            continue
        pid = int(mch.group('pid'))
        msgpart = int(mch.group('msgpart'))

        # see if we have preceding lines part of the same message
        backend = backends.setdefault(pid, [])
        if not backend:
            backend.extend((linenum, []))
        msglist = backend[1]
        if msgpart == 1 and msglist:
            yield (pid, '\n'.join(msglist))
            backend[0] = linenum
            del msglist[:]
        elif msgpart != len(msglist) + 1:
            #warn("threw away out-of-place line %r" % line, ThrownAwayWarning)
            throwaway += 1
            del backend[:]
            continue
        msglist.append(mch.group('msg'))

    # clear out messages in remaining backends
    for b, v in backends.iteritems():
        if v:
            yield (b, '\n'.join(v[1]))

    if throwaway:
        warn("threw away %d lines" % throwaway, ThrownAwayWarning)

def loglines_to_logtuples(msgsource):
    """
    Expects tuples of the form:

       (31800, "2009-05-20 15:28:42 MDT spartacus LOG:  duration: 0.375 ms  s
       tatement: SELECT * FROM machines WHERE (machines.user_id = 1534352\n A
       ND NOT machines.deleted) ORDER BY alias")

    (Quite coincidentally, that is the same format that is returned by
    syslog_to_loglines.) Returns a generator which yields tuples with more
    parsed data, e.g.:

       (31800, 1242880122, 'spartacus', 'LOG', "duration: 0.375 ms  statement
       : SELECT * FROM machines WHERE (machines.user_id = 1534352\nAND NOT ma
       chines.deleted) ORDER BY alias")

    Fields are pid, timestamp, dbuser, msglevel, msg.
    """

    msg_re = re.compile(r'''
        (?P<tstamp>\d{4}-\d{2}-\d{2} \s \d{2}:\d{2}:\d{2} \s [A-Z]{3,4})
        \s
        (?P<uname>[a-zA-Z0-9_]+)
        \s
        (?P<level>LOG|ERROR|NOTICE)
        : \s\s
        (?P<msg>.*)
        $
    ''', re.X | re.S)

    throwaway = 0
    for pid, msg in msgsource:
        mch = msg_re.match(msg)
        if not mch:
            #warn("threw away unparseable message %r" % msg, ThrownAwayWarning)
            throwaway += 1
            continue
        tstamp, uname, level, submsg = mch.group('tstamp', 'uname', 'level', 'msg')
        try:
            t = time.strptime(tstamp.rsplit(None, 1)[0], '%Y-%m-%d %H:%M:%S')
            t = int(time.mktime(t))
        except TypeError:
            #warn("threw away message w/ unparseable time %r" % msg, ThrownAwayWarning)
            throwaway += 1
            continue
        yield (pid, t, uname, level, submsg)

    if throwaway:
        warn("threw away %d lines" % throwaway, ThrownAwayWarning)

def filter_out_queries(msgsource):
    """
    Expects tuples from the input iterator of the form:

       (31800, 1242880122, 'spartacus', 'LOG', "duration: 0.375 ms  statement
       : SELECT alias FROM machines WHERE (machines.user_id = 153435\nAND NOT
        machines.deleted) ORDER BY alias")

    Returns a generator which filters out tuples representing logged SQL
    queries and their durations (such as the example above), and outputs
    those in the form:

       (31800, 1242880122, 'spartacus', 0.375, "SELECT alias FROM machines WH
       ERE (machines.user_id = 1534352\nAND NOT machines.deleted) ORDER BY al
       ias")
    """

    statement_re = re.compile(r'''
        duration: \s
        (?P<dur>[0-9][0-9.]+) \s ms \s\s
        statement: \s
        (?P<stmt>.*) $
    ''', re.X | re.S)

    for pid, tstamp, uname, level, msg in msgsource:
        if level != 'LOG':
            continue
        mch = statement_re.match(msg)
        if not mch:
            continue
        duration = float(mch.group('dur'))
        yield (pid, tstamp, uname, duration, mch.group('stmt'))
