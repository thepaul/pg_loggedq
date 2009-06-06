# parsesql.py

from _parsesql import sqlscanner

def parse(sql):
    s = sqlscanner(sql)
    while True:
        ret = s.getnext()
        if ret is None:
            break
        yield ret
