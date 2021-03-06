from distutils.core import setup, Extension

setup(name='pg_loggedq',
      version='0.2.0',
      description='Processing of PostgreSQL logged queries',
      author='paul cannon',
      author_email='paul@mozy.com',
      packages=['pg_loggedq'],
      ext_modules=[
          Extension('pg_loggedq._parsesql',
              sources=['_parsesqlmodule.c'],
              libraries=['sqlscanner'],
              library_dirs=['.'])])
