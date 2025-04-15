dnl Copyright 2006-2024 Carnegie Mellon University
dnl See license information in LICENSE.txt.

dnl Process this file with autoconf to produce a configure script
dnl ------------------------------------------------------------------------
dnl configure.ac
dnl autotools build system for libfixbuf
dnl ------------------------------------------------------------------------
dnl Authors: Brian Trammell, Dan Ruef, Chris Inacio
dnl ------------------------------------------------------------------------
dnl @DISTRIBUTION_STATEMENT_BEGIN@
dnl libfixbuf 2.5
dnl
dnl Copyright 2024 Carnegie Mellon University.
dnl
dnl NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
dnl INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
dnl UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
dnl IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
dnl FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
dnl OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT
dnl MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
dnl TRADEMARK, OR COPYRIGHT INFRINGEMENT.
dnl
dnl Licensed under a GNU-Lesser GPL 3.0-style license, please see
dnl LICENSE.txt or contact permission@sei.cmu.edu for full terms.
dnl
dnl [DISTRIBUTION STATEMENT A] This material has been approved for public
dnl release and unlimited distribution.  Please see Copyright notice for
dnl non-US Government use and distribution.
dnl
dnl This Software includes and/or makes use of Third-Party Software each
dnl subject to its own license.
dnl
dnl DM24-1020
dnl @DISTRIBUTION_STATEMENT_END@
dnl ------------------------------------------------------------------------

AC_INIT([libfixbuf], [2.5.0], [netsa-help@cert.org], [libfixbuf], [https://tools.netsa.cert.org/fixbuf2/])
AC_CONFIG_AUX_DIR([autoconf])
AC_COPYRIGHT([Copyright (C) 2006-2024 Carnegie Mellon University
GNU General Public License (GPL) Rights pursuant to Version 3, June 2007])

AM_INIT_AUTOMAKE([1.6.3 foreign nostdinc])

dnl change this version number every new release. (1:0:0 starts at 1.3.0)
dnl the first number should be changed if the interface has changed
dnl the second number should be changed for bug fixes, small changes
dnl the third number should be changed if the first number changes and
dnl the release is backwards compatible with the previous release.
dnl https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html

# Previous release: 2.5.0, LIBCOMPAT 10:0:1
LIBCOMPAT="10:0:1"
AC_SUBST([LIBCOMPAT])

AC_CONFIG_HEADERS([include/fixbuf/config.h])
AH_TOP([/*
 *  Copyright 2006-2024 Carnegie Mellon University
 *  See license information in LICENSE.txt.
 */
/*
 *  autoheader generated header file
 *  ------------------------------------------------------------------------
 *  @DISTRIBUTION_STATEMENT_BEGIN@
 *  libfixbuf 2.5
 *
 *  Copyright 2024 Carnegie Mellon University.
 *
 *  NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *  INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 *  UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *  IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *  FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *  OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES NOT
 *  MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *  TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 *
 *  Licensed under a GNU-Lesser GPL 3.0-style license, please see
 *  LICENSE.txt or contact permission@sei.cmu.edu for full terms.
 *
 *  [DISTRIBUTION STATEMENT A] This material has been approved for public
 *  release and unlimited distribution.  Please see Copyright notice for
 *  non-US Government use and distribution.
 *
 *  This Software includes and/or makes use of Third-Party Software each
 *  subject to its own license.
 *
 *  DM24-1020
 *  @DISTRIBUTION_STATEMENT_END@
 */])

AC_CONFIG_MACRO_DIR([m4])
LT_INIT

AM_MAINTAINER_MODE([enable])
AM_MISSING_PROG(XSLTPROC, xsltproc)

# Minimum library versions
AC_SUBST([FIXBUF_MIN_GLIB2],[2.34.0])
AC_SUBST([FIXBUF_MIN_OPENSSL], [1.0.2])

AC_LANG([C])
AC_PROG_CC
AC_DEBUG
AX_ENABLE_WARNINGS

if test "X${srcdir}" = "X." ; then
    # building in place
    AC_SUBST([AM_CPPFLAGS], ['-I. -I$(top_srcdir)/include'])
else
    AC_SUBST([AM_CPPFLAGS], ['-I. -I$(srcdir) -I$(top_builddir)/include -I$(top_srcdir)/include'])
fi

AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_MKDIR_P

AC_CHECK_HEADERS([unistd.h stdint.h errno.h arpa/inet.h netinet/in.h sys/errno.h sys/socket.h pthread.h])

AM_WITH_DMALLOC


dnl ----------------------------------------------------------------------
dnl Set up doxygen
dnl ----------------------------------------------------------------------

DX_HTML_FEATURE([ON])
DX_CHM_FEATURE([OFF])
DX_CHI_FEATURE([OFF])
DX_MAN_FEATURE([OFF])
DX_RTF_FEATURE([OFF])
DX_XML_FEATURE([OFF])
DX_PDF_FEATURE([OFF])
DX_PS_FEATURE([OFF])
DX_INIT_DOXYGEN(AC_PACKAGE_NAME, [Doxyfile], [doc])

dnl ----------------------------------------------------------------------
dnl Check for aligned access required
dnl ----------------------------------------------------------------------

AX_CHECK_ALIGNED_ACCESS_REQUIRED

dnl ----------------------------------------------------------------------
dnl Check for a suitable glib
dnl ----------------------------------------------------------------------

GLIB_PRESENT=NO
GLIB_LDADD=

AM_PATH_GLIB_2_0([${FIXBUF_MIN_GLIB2}],[GLIB_PRESENT=YES],,[gthread])

AC_ARG_WITH(glib-static,
[  --with-glib-static=prefix   use static glib tree],[
GLIB_STATIC_LIBGLIB2_A="${withval}/lib/libglib-2.0.a ${withval}/lib/libgthread-2.0.a"
GLIB_STATIC_INCLUDE="${withval}/include/glib-2.0"
GLIB_STATIC_LIBINCLUDE="${withval}/lib/glib-2.0/include"
if test -f ${GLIB_STATIC_LIBGLIB2_A}; then
    if test -f "${GLIB_STATIC_INCLUDE}/glib.h"; then
        GLIB_LDADD=${GLIB_STATIC_LIBGLIB2_A}
        GLIB_CFLAGS="-I${GLIB_STATIC_INCLUDE} -I${GLIB_STATIC_LIBINCLUDE}"
        AC_SUBST([GLIB_LIBS])
        AC_SUBST([GLIB_CFLAGS])
        AC_SUBST([GLIB_LDADD])
        GLIB_PRESENT=YES
        AC_MSG_NOTICE([assuming static glib in ${withval} is good...])
    fi
fi
])

if test x"$GLIB_PRESENT" != "xYES"; then
  AC_MSG_ERROR([Cannot find a suitable glib2 (>= ${FIXBUF_MIN_GLIB2})])
fi

dnl ----------------------------------------------------------------------
dnl Check for getaddrinfo
dnl ----------------------------------------------------------------------

AC_CHECK_FUNCS([getaddrinfo])

dnl ---------------------------------------------------------------------
dnl Check for pthread
dnl --------------------------------------------------------------------
AC_CHECK_LIB([pthread],[pthread_mutex_lock],[],[
    AC_MSG_ERROR([Required pthread library not found])
])


dnl ----------------------------------------------------------------------
dnl check for all of the Solaris specific libraries
dnl ----------------------------------------------------------------------
AC_SEARCH_LIBS([nanosleep], [rt])
AC_SEARCH_LIBS([inet_ntoa], [nsl])
AC_SEARCH_LIBS([socket], [socket])

dnl ----------------------------------------------------------------------
dnl check sizeof types
dnl ----------------------------------------------------------------------
AC_CHECK_SIZEOF([double])
AC_CHECK_SIZEOF([float])
if test "x${ac_cv_sizeof_double}" != x8
then
    AC_MSG_ERROR([double has unexpected size ${ac_cv_sizeof_double}; expected 8])
fi
if test "x${ac_cv_sizeof_float}" != x4
then
    AC_MSG_ERROR([float has unexpected size ${ac_cv_sizeof_float}; expected 4])
fi


dnl ----------------------------------------------------------------------
dnl Aborting on defaulted size specifications
dnl ----------------------------------------------------------------------
fb_abort_on_default_sizespec=1
AC_ARG_ENABLE([abort-on-default-sizespec],
    [AS_HELP_STRING([--disable-abort-on-default-sizespec],
        [disable abort when using defaulted (zero-sized) information element specifications for internal templates.  This will cause fBufSetInternalTemplate() to return an error instead.])[]dnl
    ],[
        if test "x$enableval" = "xno"
        then
          fb_abort_on_default_sizespec=0
        fi
    ])
AC_DEFINE_UNQUOTED([FB_ABORT_ON_DEFAULTED_LENGTH],
                   [$fb_abort_on_default_sizespec],
                   [Define to 1 to enable aborts of defaulted spec lengths])

dnl ----------------------------------------------------------------------
dnl Check for SCTP support
dnl ----------------------------------------------------------------------

AC_ARG_WITH([sctp],dnl
[AS_HELP_STRING([[--with-sctp[=DIR]]],
                [Enable SCTP support using libsctp [default=no]; if DIR given, find libsctp in that directory])],
[
    if test "x$withval" != "xno"; then
        if test -d ${withval}; then
            LDFLAGS="-L${withval}/lib ${LDFLAGS}"
            CPPFLAGS="-I${withval}/include ${CPPFLAGS}"
        fi

        # look for libsctp
        AC_CHECK_LIB([sctp],[sctp_sendmsg],[
            AC_DEFINE([FB_ENABLE_SCTP], [1],
                      [Define to 1 to enable SCTP support])
            LIBS="-lsctp ${LIBS}"
            AC_SUBST(FIXBUF_REQ_LIBSCTP, [1])
        ],[
            AC_MSG_FAILURE([--with-sctp given but cannot find libsctp])
        ])

        # look for SCTP header files
        AC_CHECK_HEADER([netinet/sctp.h],[
            AC_DEFINE([FB_INCLUDE_SCTP_H],[1],
                      [Define to 1 to use sctp.h header])
            AC_SUBST([FIXBUF_REQ_SCTPDEV],[1])
        ],[],[AC_INCLUDES_DEFAULT([
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
        ])])

        AC_CHECK_HEADER([netinet/sctp_uio.h],[
            AC_DEFINE([FB_INCLUDE_SCTP_UIO_H],[1],
                      [Define to 1 to use sctp_uio.h header])
        ])
    fi
])


dnl ----------------------------------------------------------------------
dnl Check for OpenSSL TLS and DTLS support
dnl ----------------------------------------------------------------------
AX_LIB_OPENSSL([no],[${FIXBUF_MIN_OPENSSL}],dnl
    [use OpenSSL for TLS/DTLS support [default=no]; if DIR given, prepend it to PKG_CONFIG_PATH])

dnl ----------------------------------------------------------------------
dnl Check for Spread support
dnl ----------------------------------------------------------------------

AC_PATH_SPREAD([4.1])

dnl ----------------------------------------------------------------------
dnl Determine infomodel information
dnl ----------------------------------------------------------------------
INFOMODEL_AC_COLLECT_REGISTRIES([src/infomodel])
AC_PATH_PROG([PERL], [perl])

dnl automake 1.9 doesn't substitute these automatically, so we do it ourselves
dnl just in case
AC_SUBST([abs_top_srcdir])
AC_SUBST([abs_top_builddir])

dnl ----------------------------------------------------------------------
dnl Check for script to update the distribution statement
dnl ----------------------------------------------------------------------
update_markings="scripts/update-license.pl"
AC_CHECK_FILE([${srcdir}/${update_markings}], [
    AC_SUBST([UPDATE_MARKINGS], ['${top_srcdir}'/${update_markings}])
])
AM_CONDITIONAL([HAVE_UPDATE_MARKINGS], [test -n "${UPDATE_MARKINGS}"])

dnl ----------------------------------------------------------------------
dnl Miscellaneous checks
dnl ----------------------------------------------------------------------

PACKAGE_VERSION_SPLIT

AX_FB_PRINT_CONFIG

AC_CONFIG_FILES([
    Makefile
    src/Makefile
    src/infomodel/Makefile
    include/Makefile
    include/fixbuf/version.h
    libfixbuf.pc
    Doxyfile])

AC_OUTPUT
