# analyze.py

import loglines
from parsesql import normalize

def tally_queries(querytuples, max_tblsize=64000):
    """
    Expects tuples of the sort returned by loglines.filter_out_queries.

    Returns a dictionary mapping normalized statements to 3-element lists
    of the form [avg_duration, usernames, numtimes], where avg_duration
    is the average duration taken by the statement in milliseconds,
    usernames is the set of usernames which were seen to run that statement,
    and numtimes is the number of times it was seen.

    Example:

    >>> tally_queries([
    ...     (31800, 1242880122, 'spartacus', 0.375,
    ...      'SELECT alias FROM machines WHERE user_id = 1534352'),
    ...     (31801, 1242880123, 'phoenix', 1.375,
    ...      'SELECT alias FROM machines WHERE user_id = 1462928'),
    ... ])
    {'select alias from machines where user_id = ?': [0.875, set(['spartacus', 'phoenix']), 2]}
    """

    knownq = {}

    for pid, tstamp, uname, duration, stmt in querytuples:
        nstmt = normalize(stmt, remove_const=True)
        rec = knownq.setdefault(nstmt, [])
        if len(rec) == 0:
            rec.extend([duration, set([uname]), 1])
            if len(knownq) >= max_tblsize:
                break
        else:
            rec[0] += duration
            rec[1].add(uname)
            rec[2] += 1

    for v in knownq.itervalues():
        v[0] = v[0] / float(v[2])

    return knownq
