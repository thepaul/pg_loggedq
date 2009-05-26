from ctypes import *

scan_so = CDLL('./scan.so')

scanner_init = scan_so.scanner_init
scanner_init.argtypes = [c_char_p]
scanner_init.restype = None

scanner_finish = scan_so.scanner_finish
scanner_finish.argtypes = []
scanner_finish.restype = None

yylex = scan_so.base_yylex
yylex.argtypes = []
yylex.restype = c_int

yylval = c_char_p.in_dll(scan_so, 'yylval')

def tokenize_sql(sql):
    scanner_init(sql.strip())
    last_was_whitespace = True
    while True:
        result = yylex()
        if result == 0:
            break
        elif result == 1:
            last_was_whitespace = False
            yield '?'
        elif result == 2:
            val = yylval.value
            if val.isspace():
                if last_was_whitespace:
                    continue
                else:
                    last_was_whitespace = True
            else:
                last_was_whitespace = False
            yield yylval.value
    scanner_finish()
