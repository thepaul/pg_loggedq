from ctypes import *

scan_so = CDLL('./scan.so')

scanner_init = scan_so.scanner_init
scanner_init.argtypes = [c_char_p]
scanner_init.restype = None

yylex = scan_so.base_yylex
yylex.argtypes = []
yylex.restype = c_int

yylval = c_char_p.in_dll(scan_so, 'yylval')

def tokenize_sql(sql):
    scanner_init(sql)
    while True:
        result = yylex()
        if result == 0:
            break
        elif result == 1:
            yield '?'
        elif result == 2:
            yield yylval.value
