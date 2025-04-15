##  package_version_split.m4    -*- mode: autoconf -*-

dnl Copyright 2015-2024 Carnegie Mellon University
dnl See license information in LICENSE.txt.

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

AC_DEFUN([PACKAGE_VERSION_SPLIT],[
    PACKAGE_VERSION_MAJOR=`echo "$PACKAGE_VERSION"   | $SED 's/\([[^.]]*\).*/\1/'`
    PACKAGE_VERSION_MINOR=`echo "$PACKAGE_VERSION"   | $SED 's/[[^.]]*\.\([[^.]]*\).*/\1/'`
    PACKAGE_VERSION_RELEASE=`echo "$PACKAGE_VERSION" | $SED 's/[[^.]]*\.[[^.]]*\.\([[^.]]*\).*/\1/'`
    PACKAGE_VERSION_BUILD=`echo "$PACKAGE_VERSION"   | $SED 's/[[^.]]*\.[[^.]]*\.[[^.]]*\.\(.*\)/\1/'`
    if test "x$PACKAGE_VERSION_BUILD" = "x$PACKAGE_VERSION"; then
        PACKAGE_VERSION_BUILD="0"
    fi
    AC_SUBST(PACKAGE_VERSION_MAJOR)
    AC_SUBST(PACKAGE_VERSION_MINOR)
    AC_SUBST(PACKAGE_VERSION_RELEASE)
    AC_SUBST(PACKAGE_VERSION_BUILD)
])
