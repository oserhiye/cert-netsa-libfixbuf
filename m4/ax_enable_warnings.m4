##  ax_enable_warnings.m4       -*- mode: autoconf -*-

dnl Copyright 2006-2024 Carnegie Mellon University
dnl See license information in LICENSE.txt.

dnl Determines the compiler flags to use for warnings.  User may use
dnl --enable-warnings to provide their own or override the default.
dnl
dnl OUTPUT VARIABLE:  WARN_CFLAGS

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


AC_DEFUN([AX_ENABLE_WARNINGS],[
    AC_SUBST([WARN_CFLAGS])

    WARN_CFLAGS=
    default_warn_flags="-Wall -Wextra -Wshadow -Wpointer-arith -Wformat=2 -Wunused -Wduplicated-cond -Wwrite-strings -Wmissing-prototypes -Wstrict-prototypes"
    # do not include -Wundef until the #if conditions are changed to #ifdef

    AC_ARG_ENABLE([warnings],
    [AS_HELP_STRING([[--enable-warnings[=FLAGS]]],
        [enable compile-time warnings [default=yes]; if value given, use those compiler warnings instead of default])],
    [
        if test "X${enable_warnings}" = Xno
        then
            AC_MSG_NOTICE([disabled all compiler warning flags])
        elif test "X${enable_warnings}" != Xyes
        then
            WARN_CFLAGS="${enable_warnings}"
        fi
    ],[
        enable_warnings=yes
    ])

    if test "x${enable_warnings}" = xyes
    then
        save_cflags="${CFLAGS}"
        for f in ${default_warn_flags} ; do
            AC_MSG_CHECKING([whether ${CC} supports ${f}])
            CFLAGS="${save_cflags} -Werror ${f}"
            AC_COMPILE_IFELSE([
                AC_LANG_PROGRAM([[@%:@include <stdio.h>]],[[
                    printf("Hello, World\n");
                ]])
            ],[
                AC_MSG_RESULT([yes])
                WARN_CFLAGS="${WARN_CFLAGS} ${f}"
            ],[
                AC_MSG_RESULT([no])
            ])
        done
        CFLAGS="${save_cflags}"
    fi

])
