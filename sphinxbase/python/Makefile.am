EXTRA_DIST = sphinxbase.c \
	sphinxbase.pyx \
	setup_win32.py \
	sb_test.py \
	hufftest2.py

pkginclude_HEADERS = sphinxbase.pxd

if BUILD_PYTHON
# Setuptools isn't really compatible with distutils, unfortunately
SETUPTOOLS_CRAP = `python -c 'import setuptools' 2>/dev/null \
	&& echo --single-version-externally-managed --record filelist.txt`

all-local: pymod-build-stamp

install-exec-local: pymod-build-stamp
	$(PYTHON) setup.py install --prefix $(DESTDIR)$(prefix) $(SETUPTOOLS_CRAP)

uninstall-local:
	$(PYTHON) setup.py bogus_uninstall --prefix $(DESTDIR)$(prefix)

clean-local:
	$(PYTHON) setup.py clean --all
# This is dumb, but distutils is completely incapable of VPATH building
	test -z "$(VPATH)" || $(RM) sphinxbase.c
	$(RM) -r *.egg-info
	$(RM) pymod-build-stamp filelist.txt

pymod-build-stamp: $(srcdir)/sphinxbase.c
# This is dumb, but distutils is completely incapable of VPATH building
	test -z "$(VPATH)" || cp "$(srcdir)/sphinxbase.c" sphinxbase.c
	$(PYTHON) setup.py build
	touch $@
endif

if BUILD_CYTHON
$(srcdir)/sphinxbase.c: $(srcdir)/sphinxbase.pyx $(srcdir)/sphinxbase.pxd
	cython -o $@ $<
endif
