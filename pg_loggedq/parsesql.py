# parsesql.py

from _parsesql import sqlscanner

def parse(sql):
    s = sqlscanner(sql)
    while True:
        ret = s.getnext()
        if ret is None:
            break
        yield ret

def repr_BCONST(val):
    assert val[0] == 'b'
    assert "'" not in val
    return "b'%s'" % val

def repr_XCONST(val):
    assert val[0] == 'x'
    assert "'" not in val
    return "x'%s'" % val

def repr_ICONST(val):
    return str(val)

def repr_FCONST(val):
    return val

def repr_SCONST(val):
    return "'%s'" % val.replace("'", "''")

def repr_KEYWORD(val):
    return val

def repr_TYPECAST(val):
    return '::'

def repr_PARAM(val):
    return '$%d' % val

def repr_IDENT(val):
    if val.islower() and '"' not in val:
        return val
    return '"%s"' % val.replace('"', '""')

def repr_OP(val):
    return val

def tokenrepr(tokname, val):
    return globals()['repr_' + tokname](val)

def tokenrepr_noconst(tokname, val):
    if tokname.endswith('CONST'):
        return '?'
    return tokenrepr(tokname, val)

def normalize_q(sql, remove_const=False):
    parts = tuple(parse(sql))
    if len(parts) == 0:
        return ''
    if remove_const:
        my_tokenrepr = tokenrepr_noconst
    else:
        my_tokenrepr = tokenrepr
    reprs = [my_tokenrepr(*pair) for pair in parts]
    return ' '.join(reprs)
