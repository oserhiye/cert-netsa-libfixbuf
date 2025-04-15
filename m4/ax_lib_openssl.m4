dnl -*- mode: autoconf -*-
dnl Copyright (C) 2004-2024 Carnegie Mellon University
dnl See license information in LICENSE.txt.

dnl ------------------------------------------------------------------------
dnl ax_lib_openssl.m4
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

# ---------------------------------------------------------------------------
# AX_LIB_OPENSSL
#
#   Check for the OpenSSL library (-lssl -lcrypt) and header files.
#
#   Expects three arguments:
#
#   1. First should be "yes", "no", or "auto". "yes" means to fail if
#   OpenSSL cannot be found unless the user explicitly disables it.
#   "no" means only use OpenSSL when requested by the user. "auto" (or
#   any other value) means to check for OpenSSL unless disabled by the
#   user, but do not error if it is not found.  It is a fatal error if
#   the user specifies --with-openssl and it cannot be found.
#
#   2. Second is the minimum version to accept.
#
#   3. Third is the help string to print for the --with-openssl argument.
#
#   Output definitions: HAVE_EVP_MD5, HAVE_EVP_MD_FETCH, HAVE_EVP_Q_DIGEST,
#   HAVE_EVP_SHA1, HAVE_EVP_SHA256, HAVE_MD5, HAVE_OPENSSL,
#   HAVE_OPENSSL_EVP_H, HAVE_OPENSSL_MD5_H, HAVE_OPENSSL_SHA_H, HAVE_SHA1,
#   HAVE_SHA256
#
AC_DEFUN([AX_LIB_OPENSSL],[
    default="$1"
    ssl_min_version="$2"
    m4_define(openssl_helpstring,[[$3]])

    FB_HAVE_OPENSSL=

    if test "x${default}" = xyes
    then
        request_require=required
    else
        request_require=requested
    fi

    AC_ARG_WITH([openssl],
    [AS_HELP_STRING([--with-openssl@<:@=DIR@:>@],dnl
        openssl_helpstring)],[],
    [
        # Option not given, use default
        if test "x${default}" = xyes || test "x${default}" = xno
        then
            with_openssl="${default}"
        fi
    ])

    if test "x${with_openssl}" = xno
    then
        AC_MSG_NOTICE([not checking for openssl])
    else
        # If an argument is given, prepend it to PKG_CONFIG_PATH
        fb_save_PKG_CONFIG_PATH="${PKG_CONFIG_PATH}"
        if test -n "${with_openssl}" && test "x${with_openssl}" != xyes
        then
            PKG_CONFIG_PATH="${with_openssl}${PKG_CONFIG_PATH+:${PKG_CONFIG_PATH}}"
            export PKG_CONFIG_PATH

            if expr "x${with_openssl}" : '.*/pkgconfig$' > /dev/null
            then
                :
            else
                AC_MSG_WARN([Argument to --with-openssl should probably end with '/pkgconfig'])
            fi
        fi

        # Check for the module
        PKG_CHECK_MODULES([openssl],
            [openssl >= ${ssl_min_version}],
            [FB_HAVE_OPENSSL=yes],dnl
        [
            if test "x${with_openssl}" != x
            then
                AC_MSG_WARN([pkg-config cannot find a suitable openssl (>= ${ssl_min_version}). Do you need to install openssl-devel or adjust PKG_CONFIG_PATH?: $openssl_PKG_ERRORS])
                AC_MSG_ERROR([openssl is ${request_require} but is not found; use --with-openssl=no to disable])
            else
                AC_MSG_NOTICE([not building with OpenSSL support])
            fi
        ])

        if test -n "${fb_save_PKG_CONFIG_PATH}"
        then
            PKG_CONFIG_PATH="${fb_save_PKG_CONFIG_PATH}"
        fi
    fi

    if test "x${FB_HAVE_OPENSSL}" = xyes
    then
        # pkg-config found openssl; try to compile a program using it
        fb_save_CFLAGS="${CFLAGS}"
        fb_save_LIBS="${LIBS}"
        CFLAGS="${CFLAGS} ${openssl_CFLAGS}"
        LIBS="${openssl_LIBS} ${LIBS}"

        AC_MSG_CHECKING([usability of openssl library and headers])
        AC_LINK_IFELSE(
            [AC_LANG_PROGRAM([
#include <openssl/ssl.h>
                ],[[
const SSL_METHOD *tlsmeth = NULL;
SSL_CTX *ssl_ctx = NULL;

#if OPENSSL_VERSION_NUMBER < 0x10100000L
tlsmeth = SSLv23_server_method();
#else
tlsmeth = TLS_server_method();
#endif

ssl_ctx = SSL_CTX_new(tlsmeth);
                 ]])],
             [],[FB_HAVE_OPENSSL=]
        )

        if test "x${FB_HAVE_OPENSSL}" != xyes
        then
            AC_MSG_RESULT([no])
            AC_MSG_WARN([pkg-config found openssl but configure cannot compile a program that uses it. Details in config.log.])
            if test "x${with_openssl}" != x
            then
                AC_MSG_ERROR([openssl is ${request_require} but unable to use it; use --with-openssl=no to disable])
            else
                AC_MSG_NOTICE([building without OpenSSL])
            fi
        else
            AC_MSG_RESULT([yes])
            AC_MSG_NOTICE([building with OpenSSL support])
            AC_DEFINE(HAVE_OPENSSL, 1, [Define to 1 to enable OpenSSL support])
            AC_SUBST([FIXBUF_PC_OPENSSL],["openssl >= ${ssl_min_version}"])
            FIXBUF_REQ_LIBSSL=1

            # Additional function to check for; no error
            AC_CHECK_FUNC([DTLS_method],[
                FIXBUF_HAVE_DTLS=1
                AC_DEFINE([HAVE_OPENSSL_DTLS],[1],
                          [Define to 1 to enable DTLS support])
            ],[
                AC_MSG_NOTICE([OpenSSL does not support DTLS])
            ])
        fi

        # Restore values
        CFLAGS="${fb_save_CFLAGS}"
        LIBS="${fb_save_LIBS}"
    fi
])
