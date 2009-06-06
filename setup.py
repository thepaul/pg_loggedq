from distutils.core import setup, Extension

setup(name='pg_loggedq',
      version='0.2.0',
      ext_modules=[Extension('_parsesql',
                             sources=['_parsesqlmodule.c'],
                             libraries=['sqlscanner'],
                             library_dirs=['.'])])
