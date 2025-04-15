##  ax_fb_print_config.m4       -*- mode: autoconf -*-

dnl Copyright 2006-2024 Carnegie Mellon University
dnl See license information in LICENSE.txt.

dnl Print a summary of the configuration

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


AC_DEFUN([AX_FB_PRINT_CONFIG],[

    FB_BUILD_CONFIG="
    * Configured package:           ${PACKAGE_STRING}
    * Host type:                    ${build}
    * Source files (\$top_srcdir):   ${srcdir}
    * Install directory:            ${prefix}"

    FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * pkg-config path:              ${PKG_CONFIG_PATH}
    * GLIB:                         ${GLIB_LIBS}"

    # OpenSSL
    if test "x${FIXBUF_REQ_LIBSSL}" = "x1"
    then
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * OpenSSL Support:              YES (${openssl_LIBS})"
    else
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * OpenSSL Support:              NO"
    fi

    # DTLS
    if test "x${FIXBUF_HAVE_DTLS}" = "x1"
    then
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * DTLS Support:                 YES"
    else
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * DTLS Support:                 NO"
    fi

    # SCTP
    if test "x${FIXBUF_REQ_SCTPDEV}" = "x1"
    then
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * SCTP Support:                 YES"
    else
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * SCTP Support:                 NO"
    fi

    # SPREAD
    if test "x${FIXBUF_REQ_LIBSPREAD}" = "x1"
    then
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * Spread Toolkit Support:       YES"
    else
        FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * Spread Toolkit Support:       NO"
    fi

    # Remove leading whitespace
    fb_msg_cflags="${AM_CPPFLAGS} ${CPPFLAGS} ${WARN_CFLAGS} ${DEBUG_CFLAGS} ${CFLAGS}"
    fb_msg_cflags=`echo "${fb_msg_cflags}" | sed 's/^ *//' | sed 's/  */ /g'`

    fb_msg_ldflags="${FB_LDFLAGS} ${LDFLAGS}"
    fb_msg_ldflags=`echo "${fb_msg_ldflags}" | sed 's/^ *//' | sed 's/  */ /g'`

    fb_msg_libs="${LIBS}"
    fb_msg_libs=`echo "${fb_msg_libs}" | sed 's/^ *//' | sed 's/  */ /g'`

    FB_BUILD_CONFIG="${FB_BUILD_CONFIG}
    * Compiler (CC):                ${CC}
    * Compiler flags (CFLAGS):      ${fb_msg_cflags}
    * Linker flags (LDFLAGS):       ${fb_msg_ldflags}
    * Libraries (LIBS):             ${fb_msg_libs}
"

    # First argument becomes option to config.status
    AC_CONFIG_COMMANDS([print-config],[
        echo "${FB_BUILD_CONFIG}"
    ],[FB_BUILD_CONFIG='${FB_BUILD_CONFIG}'])
])
