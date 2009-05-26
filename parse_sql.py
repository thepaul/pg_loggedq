from ctypes import *

scan_so = CDLL('./scan.so')

class PerScannerData(Structure):
    _fields_ = [('internal0', c_int),
                ('internal1', c_char_p),
                ('internal2', c_int),
                ('lval', c_char_p)]

yylex_init_extra = scan_so.base_yylex_init_extra
yylex_init_extra.argtypes = [POINTER(PerScannerData), POINTER(c_void_p)]
yylex_init_extra.restype = c_int

yylex_destroy = scan_so.base_yylex_destroy
yylex_destroy.argtypes = [c_void_p]
yylex_destroy.restype = c_int

scanner_init = scan_so.scanner_init
scanner_init.argtypes = [c_char_p, c_void_p]
scanner_init.restype = None

scanner_finish = scan_so.scanner_finish
scanner_finish.argtypes = [c_void_p]
scanner_finish.restype = None

yylex = scan_so.base_yylex
yylex.argtypes = [c_char_p, c_void_p]
yylex.restype = c_int

def tokenize_sql(sql):
    dat = PerScannerData()
    scanner = c_void_p()
    yylex_init_extra(pointer(dat), pointer(scanner))
    try:
        scanner_init(sql.strip(), scanner)
        try:
            while True:
                result = yylex("", scanner)
                if result == 0:
                    break
                elif result == 1:
                    yield '?'
                elif result == 2:
                    yield dat.lval
       finally:
            scanner_finish(scanner)
    finally:
        yylex_destroy(scanner)
