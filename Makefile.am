##  Copyright 2006-2024 Carnegie Mellon University
##  See license information in LICENSE.txt.

##  Process this file with automake to produce Makefile.in
##  ------------------------------------------------------------------------
##  Makefile.am (toplevel)
##  autotools build system for libfixbuf
##  ------------------------------------------------------------------------
##  Authors: Brian Trammell
##  ------------------------------------------------------------------------
##  @DISTRIBUTION_STATEMENT_BEGIN@
##  libfixbuf 2.5
##
##  Copyright 2024 Carnegie Mellon University.
##
##  NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
##  INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
##  UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
##  IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
##  FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
##  OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT
##  MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
##  TRADEMARK, OR COPYRIGHT INFRINGEMENT.
##
##  Licensed under a GNU-Lesser GPL 3.0-style license, please see
##  LICENSE.txt or contact permission@sei.cmu.edu for full terms.
##
##  [DISTRIBUTION STATEMENT A] This material has been approved for public
##  release and unlimited distribution.  Please see Copyright notice for
##  non-US Government use and distribution.
##
##  This Software includes and/or makes use of Third-Party Software each
##  subject to its own license.
##
##  DM24-1020
##  @DISTRIBUTION_STATEMENT_END@
##  ------------------------------------------------------------------------

ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src include
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libfixbuf.pc

include doxygen.am

MOSTLYCLEANFILES = $(DX_CLEANFILES) $(RELEASES_XML)
MAINTAINERCLEANFILES = $(SPECFILE)

RELEASES_XML = doc/releases.xml
SPECFILE = libfixbuf.spec

# HTML documentation is only created as part of making a release
#
# When making a release, update the document markings on file in $(distdir).
#
# Next, manually run Doxygen---line beginning with $(DX_ENV).  The Doxyfile
# configuration tells Doxygen to use $(SRCDIR) for source files (which is set
# to $(distdir) in the invocation) and $(DOCDIR) as the output directory
# (which is set to $(distdir)/doc).  Doxygen runs over the files with the
# updated markings and applies the Doxyhead template with updated markings.
#
# Clean up (repair) the generated documentation.
#
# Remove include/fixbuf/version.h; it is there for Doxygen to process but it
# is not part of the release.
#
# Finally, ensure the tar command produces a file of reasonable size.
#
dist-hook: $(RELEASES_XML) $(DX_CONFIG)
	@echo "############### updating doc markings"
	$(UPDATE_DOC_MARKINGS)
	@echo "############### running doxygen"
	$(MKDIR_P) "$(distdir)/doc/html/$(DX_PROJECT)"
	$(DX_ENV) SRCDIR=$(distdir) DOCDIR=$(distdir)/doc $(DX_DOXYGEN) $(DX_CONFIG)
	@echo "############### running the repair script"
	rm -f $(distdir)/include/fixbuf/version.h
	repair_dir="$(distdir)/" ; $(REPAIR_DOXY_DOCS)
	@echo "############### checking length of tar output"
	$(AMTAR) cof - $(distdir) | $(CHECK_TAR_LEN)
	@echo "############### finishing the dist-hook"


# If $(UPDATE_MARKINGS) is available, run it over the files in the $(distdir)
#
UPDATE_DOC_MARKINGS = \
  if test -f "$(UPDATE_MARKINGS)" ; then \
    find $(distdir) \
      -name html -prune -o \
      -type f -print0 \
    | xargs -0 $(PERL) $(UPDATE_MARKINGS) ; \
  fi

# Doxygen adds both id="foo" and name="foo" attributes. This removes the name
# attribute.
#
REPAIR_DOXY_DOCS = \
  if test -d "$${repair_dir}doc/html/$(DX_PROJECT)" ; then \
    for i in `find "$${repair_dir}doc/html/$(DX_PROJECT)" -name '*.html'` ; do \
      $(PERL) -i -lpwe 's/\b(id=("[^">]+")) +name=\2/$$1/g; s/\bname=("[^">]+") +(id=\1)/$$2/g;' $$i ; \
    done ; \
  fi

# This reads its standard input and assures its length is at least 16k. It is
# used by a dist-hook to check the length of the tar output in hopes of having
# make return an error when tar fails due to a the UID being too large.
# Without this, we get a small tar file and make returns successfully.
#
CHECK_TAR_LEN = \
  $(PERL) -e ' \
    binmode STDIN; \
    die "\n\t\tERROR: tar output appears too short\n" \
        if read(STDIN, $$_, 0x4000) < 0x4000;'

# Find the news2xhtml.pl script and run it over the source file (NEWS) to
# create the output file (RELEASES_XML == doc/releases.xml)
#
RUN_NEWS2XHTML = \
  srcdir='' ; \
  test -f ./doc/news2xhtml.pl || srcdir=$(srcdir)/ ; \
  if $(AM_V_P) ; then \
  echo "$(PERL) $${srcdir}doc/news2xhtml.pl < '$<' > '$@'" ; \
  else echo "  GEN     " $@ ; fi ; \
  $(PERL) $${srcdir}doc/news2xhtml.pl < "$<" > "$@" || \
  { rm "$@" ; exit 1 ; }

$(RELEASES_XML): NEWS
	@test -d doc || $(MKDIR_P) doc
	@$(RUN_NEWS2XHTML)


$(SPECFILE): $(SPECFILE).in Makefile.in
	$(MAKE_SPEC)

# SED command when generating the specfile
#
make_spec_edit = sed \
  -e 's|@FIXBUF_MIN_GLIB2[@]|$(FIXBUF_MIN_GLIB2)|g' \
  -e 's|@FIXBUF_MIN_OPENSSL[@]|$(FIXBUF_MIN_OPENSSL)|g' \
  -e 's|@PACKAGE_VERSION[@]|$(PACKAGE_VERSION)|g' \
  -e 's|@configure_input[@]|$@: Generated from $@.in by make.|g'

# Generate the RPM specfile using the above SED invocation
#
MAKE_SPEC = $(AM_V_GEN) \
  rm -f $@ ; \
  srcdir='' ; \
  test -f ./$@.in || srcdir=$(srcdir)/ ; \
  $(make_spec_edit) "$${srcdir}$@.in" > $@ || { rm -f $@ ; exit 1 ; }


EXTRA_DIST = \
    Doxyfile.in \
    LICENSE.txt \
    $(SPECFILE) \
    $(SPECFILE).in \
    doc/Doxyhead.html \
    doc/Doxyfoot.html \
    doc/DoxygenLayout.xml \
    doc/doxygen.css \
    doc/news2xhtml.pl
