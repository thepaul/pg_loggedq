from ctypes import *

scan_so = CDLL('./scan.so')
libc = CDLL('libc.so.6')

class yystype(Union):
    _fields_ = [('str', c_char_p),
                ('voidp', c_void_p),
                ('ival', c_int)]

lexer_init = scan_so.lexer_init
lexer_init.argtypes = [c_char_p]
lexer_init.restype = c_void_p

lexer_lval = scan_so.lexer_lval
lexer_lval.argtypes = [c_void_p]
lexer_lval.restype = POINTER(yystype)

lexer_next = scan_so.base_yylex
lexer_next.argtypes = [c_void_p]
lexer_next.restype = c_int

lexer_destroy = scan_so.lexer_destroy
lexer_destroy.argtypes = [c_void_p]
lexer_destroy.restype = None

free = libc.free
free.argtypes = [c_void_p]
free.restype = None

tokens = {
    'BCONST':   (1001, 'str'),
    'XCONST':   (1002, 'str'),
    'ICONST':   (1003, 'ival'),
    'FCONST':   (1004, 'str'),
    'SCONST':   (1005, 'str'),
    'KEYWORD':  (1006, 'str'),
    'TYPECAST': (1009, None),
    'PARAM':    (1010, 'ival'),
    'IDENT':    (1011, 'str'),
    'Op':       (1012, 'str')
}

inv_tokens = dict([(n, (t, f)) for (t, (n, f)) in tokens.items()])

def tokenize_sql(sql):
    scanner = lexer_init(sql)
    try:
        lval = lexer_lval(scanner)[0]
        while True:
            result = lexer_next(scanner)
            if result == 0:
                break
            try:
                name, field = inv_tokens[result]
            except KeyError:
                name = 'Op'
                val = chr(result)
            else:
                if name is 'TYPECAST':
                    val = '::'
                else:
                    val = getattr(lval, field)
                    if field == 'str':
                        free(lval.voidp)
            yield (name, val)
    finally:
        lexer_destroy(scanner)

if __name__ == "__main__":
    for name, lval in tokenize_sql("select foo, 'blah', 9.2 from bar;"):
        if name is None:
            name = chr(res)
        print "[%-9s] %r" % (name, lval)
