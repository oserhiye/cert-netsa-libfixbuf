/*
 *  Copyright 2005-2024 Carnegie Mellon University
 *  See license information in LICENSE.txt.
 */
/**
 *  @file autoinc.h
 *  Convenience include file for libfixbuf.
 */
/*
 *  ------------------------------------------------------------------------
 *  Authors: Brian Trammell
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
 *  ------------------------------------------------------------------------
 */

#ifndef _FIX_AUTOINC_H_
#define _FIX_AUTOINC_H_

#ifdef _FIXBUF_SOURCE_
#ifdef HAVE_CONFIG_H
#include <fixbuf/config.h>
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>

#include <time.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <glib.h>
#include <glib/gstdio.h>

#ifdef _FIXBUF_SOURCE_
/* These headers are only needed by libfixbuf source files; tools that link
 * against libfixbuf do not need them */

#if HAVE_OPENSSL
#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <openssl/x509v3.h>
#endif  /* HAVE_OPENSSL */

#if FB_ENABLE_SCTP
#if FB_INCLUDE_SCTP_H
#include <netinet/sctp.h>
#endif
#if FB_INCLUDE_SCTP_UIO_H
#include <netinet/sctp_uio.h>
#endif
#endif  /* FB_ENABLE_SCTP */
#endif  /* _FIXBUF_SOURCE_ */

#endif  /* _FIX_AUTOINC_H_ */

