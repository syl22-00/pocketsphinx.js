try:
    from setuptools import setup, Extension
except:
    from distutils.core import setup, Extension

import os

setup(name = 'SphinxBase',
      version = '0.6',
      author = 'David Huggins-Daines',
      author_email = 'dhuggins@cs.cmu.edu',
      description = 'Python interface to CMU Sphinx base libraries',
      license = 'BSD',
      url = 'http://cmusphinx.org',
      ext_modules = [
        Extension('sphinxbase',
                   sources=['sphinxbase.c'],
                   libraries=['sphinxbase'],
                   define_macros = [('WIN32', '1')],
                   include_dirs=['../include',
                                 '../include/win32'],
                   library_dirs=['../lib/debug'])
        ],
     ) 
