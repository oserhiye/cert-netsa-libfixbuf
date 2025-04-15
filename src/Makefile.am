##  Copyright 2006-2024 Carnegie Mellon University
##  See license information in LICENSE.txt.

##  Process this file with automake to produce Makefile.in
##  ------------------------------------------------------------------------
##  Makefile.am (source)
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

lib_LTLIBRARIES = libfixbuf.la

AM_CFLAGS = $(WARN_CFLAGS) $(DEBUG_CFLAGS) $(SPREAD_CFLAGS) $(GLIB_CFLAGS) $(openssl_CFLAGS)

libfixbuf_la_SOURCES =  fbuf.c       fbinfomodel.c fbtemplate.c  fbsession.c \
                        fbconnspec.c fbexporter.c  fbcollector.c fbcollector.h \
                        fblistener.c fbnetflow.c   fbsflow.c     fbxml.c
nodist_libfixbuf_la_SOURCES = $(MAKE_INFOMODEL_OUTPUTS)
libfixbuf_la_LDFLAGS = -version-info $(LIBCOMPAT)
libfixbuf_la_LIBADD = $(GLIB_LDADD) $(SPREAD_LDFLAGS) $(SPREAD_LIBS) $(GLIB_LIBS) $(openssl_LIBS)

EXTRA_DIST = xml2fixbuf.xslt make-infomodel

SUBDIRS = infomodel

MAKE_INFOMODEL_OUTPUTS = infomodel.c infomodel.h
RUN_MAKE_INFOMODEL = \
  srcdir='' ; \
  test -f ./make-infomodel || srcdir=$(srcdir)/ ; \
  $(PERL) "$${srcdir}make-infomodel" --package $(PACKAGE) \
    $(INFOMODEL_REGISTRY_PREFIXES) \
  || { rm -f $(MAKE_INFOMODEL_OUTPUTS) ; exit 1 ; }

BUILT_SOURCES = $(MAKE_INFOMODEL_OUTPUTS)
CLEANFILES = $(BUILT_SOURCES)

infomodel.c : infomodel.h
infomodel.h : make-infomodel Makefile
	$(AM_V_GEN)$(RUN_MAKE_INFOMODEL)
