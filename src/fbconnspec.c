/*
 *  Copyright 2006-2024 Carnegie Mellon University
 *  See license information in LICENSE.txt.
 */
/**
 *  @file fbconnspec.c
 *  IPFIX Connection Specifier implementation
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

#define _FIXBUF_SOURCE_
#include <fixbuf/private.h>



#define HAVE_GETADDRINFO 1

#if !HAVE_GETADDRINFO
struct addrinfo {
    int ai_family;              /* protocol family for socket */
    int ai_socktype;            /* socket type */
    int ai_protocol;            /* protocol for socket */
    socklen_t ai_addrlen;       /* length of socket-address */
    struct sockaddr *ai_addr;   /* socket-address for socket */
    struct addrinfo *ai_next;   /* pointer to next in list */
};
#endif

#if HAVE_GETADDRINFO

static void fbConnSpecFreeAI(
    fbConnSpec_t        *spec)
{
    if (spec->vai) {
        freeaddrinfo((struct addrinfo *)spec->vai);
        spec->vai = NULL;
    }
}

gboolean fbConnSpecLookupAI(
    fbConnSpec_t        *spec,
    gboolean            passive,
    GError              **err)
{
    struct addrinfo     hints;
    struct addrinfo *   tempaddr = NULL;
    int                 ai_err;

    /* free old addrinfo if necessary */
    fbConnSpecFreeAI(spec);

    /* set up hints */
    memset(&hints, 0, sizeof(hints));

        /* some ancient linuxen won't let you specify this */
#ifdef AI_ADDRCONFIG
    hints.ai_flags = AI_ADDRCONFIG;
#endif
    if (passive) hints.ai_flags |= AI_PASSIVE;
    hints.ai_family = AF_UNSPEC;

    /* determine socket type and protocol */
    switch (spec->transport) {
/*
 * Yeah, this is wrong. Use SOCK_STREAM/IPPROTO_TCP for SCTP.
 * getaddrinfo(2) doesn't take SCTP hints. We'll rewrite the socktype and
 * protocol later at connection time.
 */
#if FB_ENABLE_SCTP
    case FB_SCTP:
#if HAVE_OPENSSL_DTLS_SCTP
    case FB_DTLS_SCTP:
#endif
#endif
    case FB_TCP:
#if HAVE_OPENSSL
    case FB_TLS_TCP:
#endif
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        break;
    case FB_UDP:
#if HAVE_OPENSSL_DTLS
    case FB_DTLS_UDP:
#endif
        hints.ai_socktype = SOCK_DGRAM;
        hints.ai_protocol = IPPROTO_UDP;
        break;
    default:
        g_assert_not_reached();
    }

    /* get addrinfo for host/port */
    if ((ai_err = getaddrinfo(spec->host, spec->svc, &hints, &tempaddr))) {
        g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                    "error looking up address %s:%s: %s",
                    spec->host ? spec->host : "*", spec->svc,
                    gai_strerror(ai_err));
        return FALSE;
    }
    spec->vai = tempaddr;

    /* lookup succeeded. */
    return TRUE;
}

#else

static void fbConnSpecFreeAI(
    fbConnSpec_t        *spec)
{
    struct addrinfo     *ai;

    if (spec->vai) {
        ai = (struct addrinfo *)spec->vai;
        g_free(ai->ai_addr);
        g_free(ai);
        spec->vai = NULL;
    }
}

gboolean fbConnSpecLookupAI(
    fbConnSpec_t        *spec,
    gboolean            passive,
    GError              **err)
{
    struct sockaddr_in  *sa = NULL;
    struct hostent      *he = NULL;
    struct servent      *se = NULL;
    unsigned long       svcaddrlong;
    char                *svcaddrend;
    struct addrinfo     *ai = NULL;

    /* free old addrinfo if necessary */
    fbConnSpecFreeAI(spec);

    /* create a sockaddr */
    sa = g_new0(struct sockaddr_in, 1);

    /* get service address */
    svcaddrlong = strtoul(spec->svc, &svcaddrend, 10);
    if (svcaddrend != svcaddr) {
        /* Convert long to net-order uint16_t */
        sa->sin_port = g_htons((uint16_t)svcaddrlong);
    } else {
        struct servent *se;
        /* Do service lookup */
        if (!(se = getservbyname(spec->svc, "udp"))) {
            g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                    "error looking up service %s", spec->svc);
            g_free(sa);
            return FALSE;
        }
        sa->sin_port = se->s_port;
    }

    /* get host address */
    if (spec->host) {
        if (!(he = gethostbyname(spec->host))) {
            g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                        "error looking up host %s: %s",
                        spec->host, hstrerror(h_errno));
            g_free(sa);
            return FALSE;
        }
        sa->sin_addr.s_addr = *(he->h_addr);
    } else {
        if (passive) {
            sa->sin_addr.s_addr = htonl(INADDR_ANY);
        } else {
            g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                        "cannot connect() without host address");
            g_free(sa);
            return FALSE;
        }
    }

    /* fake up a struct addrinfo */
    ai = g_new0(struct addrinfo, 1);
    ai->ai_family = AF_INET;
    ai->ai_addrlen = sizeof(struct sockaddr_in);
    ai->ai_addr = sa;

    /* get socktype and protocol from transport */
    switch (spec->transport) {
#if FB_ENABLE_SCTP
    case FB_SCTP:
#if HAVE_OPENSSL_DTLS_SCTP
    case FB_DTLS_SCTP:
#endif
        ai->ai_socktype = SOCK_SEQPACKET;
        ai->ai_protocol = 0;
        break;
#endif  /* FB_ENABLE_SCTP */
    case FB_TCP:
#if HAVE_OPENSSL
    case FB_TLS_TCP:
#endif
        ai->ai_socktype = SOCK_STREAM;
        ai->ai_protocol = IPPROTO_TCP;
        break;
    case FB_UDP:
#if HAVE_OPENSSL_DTLS
    case FB_DTLS_UDP:
#endif
        ai->ai_socktype = SOCK_DGRAM;
        ai->ai_protocol = IPPROTO_UDP;
        break;
    default:
        g_assert_not_reached();
    }

    spec->vai = ai;
    return TRUE;
}

#endif /* HAVE_GETADDRINFO */

#if HAVE_OPENSSL

static int fbConnSpecGetTLSPassword(
    char                *pwbuf,
    int                 pwsz,
    int                 rwflag,
    void                *vpwstr)
{
    (void)rwflag;

    if (vpwstr) {
        strncpy(pwbuf, (const char *)vpwstr, pwsz);
        return strlen(pwbuf);
    } else {
        *pwbuf = '\0';
        return 0;
    }
}

static int fbConnSpecVerifyTLSCert(
    int                 pvok,
    X509_STORE_CTX      *x509_ctx)
{
    (void)pvok;
    (void)x509_ctx;
    return 1;
}

gboolean fbConnSpecInitTLS(
    fbConnSpec_t        *spec,
    gboolean            passive,
    GError              **err)
{
    const SSL_METHOD    *tlsmeth = NULL;
    SSL_CTX             *ssl_ctx = NULL;
    char                errbuf[FB_SSL_ERR_BUFSIZ];
    gboolean            ok = TRUE;

    /* Initialize the library and error strings */
    SSL_library_init();
    SSL_load_error_strings();

    /*
     * Select a TLS method based on passivity and transport.
     * Shortcircuit on no TLS initialization necessary for sockets.
     */
    switch (spec->transport) {
#if FB_ENABLE_SCTP
    case FB_SCTP:
#endif
    case FB_TCP:
    case FB_UDP:
        return TRUE;
#if HAVE_OPENSSL_DTLS_SCTP
    case FB_DTLS_SCTP:
        tlsmeth = passive ? DTLS_server_method() : DTLS_client_method();
        break;
#endif
    case FB_TLS_TCP:
#if OPENSSL_VERSION_NUMBER < 0x10100000L
        tlsmeth = passive ? SSLv23_server_method() : SSLv23_client_method();
#else
        tlsmeth = passive ? TLS_server_method() : TLS_client_method();
#endif
        break;
#if HAVE_OPENSSL_DTLS
    case FB_DTLS_UDP:
        tlsmeth = passive ? DTLS_server_method() : DTLS_client_method();
        break;
#endif
    default:
        g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_IMPL,
                    "Unsupported TLS method.");
        return FALSE;
    }

    /* Verify we have all the files we need */
    g_assert(spec->ssl_ca_file);
    g_assert(spec->ssl_cert_file);
    g_assert(spec->ssl_key_file);

    /* nuke the old context if there is one */
    if (spec->vssl_ctx) {
        SSL_CTX_free((SSL_CTX *)spec->vssl_ctx);
        spec->vssl_ctx = NULL;
    }

    /* create an SSL_CTX object */
    ssl_ctx = SSL_CTX_new(tlsmeth);

    if (!ssl_ctx) {
        ERR_error_string_n(ERR_get_error(), errbuf, sizeof(errbuf));
        g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                    "Cannot create SSL context: %s", errbuf);
        ERR_clear_error();
        ok = FALSE;
        goto end;
    }

    /* Set up password callback */
    SSL_CTX_set_default_passwd_cb(ssl_ctx, fbConnSpecGetTLSPassword);
    SSL_CTX_set_default_passwd_cb_userdata(ssl_ctx, spec->ssl_key_pass);

    /* Load CA certificate */
    if (SSL_CTX_load_verify_locations(ssl_ctx, spec->ssl_ca_file, NULL) != 1) {
        ok = FALSE;
        ERR_error_string_n(ERR_get_error(), errbuf, sizeof(errbuf));
        g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                    "Failed to load certificate authority file %s: %s",
                    spec->ssl_ca_file, errbuf);
        ERR_clear_error();
        goto end;
    }

    /* Load certificate */
    if (SSL_CTX_use_certificate_chain_file(ssl_ctx, spec->ssl_cert_file) != 1) {
        ok = FALSE;
        ERR_error_string_n(ERR_get_error(), errbuf, sizeof(errbuf));
        g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                    "Failed to load certificate file %s: %s",
                    spec->ssl_cert_file, errbuf);
        ERR_clear_error();
        goto end;
    }

    /* Load private key */
    if (SSL_CTX_use_PrivateKey_file(ssl_ctx, spec->ssl_key_file,
                                    SSL_FILETYPE_PEM) != 1)
    {
        ok = FALSE;
        ERR_error_string_n(ERR_get_error(), errbuf, sizeof(errbuf));
        g_set_error(err, FB_ERROR_DOMAIN, FB_ERROR_CONN,
                    "Failed to load private key file %s: %s",
                    spec->ssl_key_file, errbuf);
        ERR_clear_error();
        goto end;
    }

    /* Require verification */
    SSL_CTX_set_verify(ssl_ctx,
                       SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT,
                       fbConnSpecVerifyTLSCert);

    /* Stash SSL context in specifier */
    spec->vssl_ctx = ssl_ctx;

end:
    /* free incomplete SSL context */
    if (!ok) SSL_CTX_free(ssl_ctx);
    return ok;
}

#endif /* HAVE_OPENSSL */

fbConnSpec_t *fbConnSpecCopy(
    fbConnSpec_t    *spec)
{
    fbConnSpec_t    *newspec = g_slice_new0(fbConnSpec_t);

    newspec->transport = spec->transport;
    newspec->host = g_strdup(spec->host);
    newspec->svc = g_strdup(spec->svc);
    newspec->ssl_ca_file = g_strdup(spec->ssl_ca_file);
    newspec->ssl_cert_file = g_strdup(spec->ssl_cert_file);
    newspec->ssl_key_file = g_strdup(spec->ssl_key_file);
    newspec->ssl_key_pass = g_strdup(spec->ssl_key_pass);
    newspec->vai = NULL;
    newspec->vssl_ctx = NULL;

    return newspec;
}

void fbConnSpecFree(
    fbConnSpec_t    *spec)
{
    if (!spec) {
        return;
    }
    g_free(spec->host);
    g_free(spec->svc);
    g_free(spec->ssl_ca_file);
    g_free(spec->ssl_cert_file);
    g_free(spec->ssl_key_file);
    g_free(spec->ssl_key_pass);
    fbConnSpecFreeAI(spec);
#if HAVE_OPENSSL
    if (spec->vssl_ctx) {
        SSL_CTX_free((SSL_CTX *)spec->vssl_ctx);
    }
#endif
    g_slice_free1(sizeof(fbConnSpec_t), spec);
}

#if HAVE_SPREAD

fbSpreadSpec_t *fbConnSpreadCopy(
    fbSpreadParams_t *params)
{
    int n = 0;
    char **g = 0;
    fbSpreadSpec_t *spec = g_slice_new0( fbSpreadSpec_t );

    spec->session = params->session;
    spec->daemon  = g_strdup( params->daemon );

    for (g=params->groups; *g; ++g)
    {
        if (*g[0])
            ++spec->num_groups;
    }

    spec->groups = g_new0( sp_groupname_t, spec->num_groups );
    for (g=params->groups; *g; ++g)
    {
        /* have to copy one less than max.  Template groups will
        be automatically created by appending 'T' to the groups
        specified here. */
        if (*g[0])
            strncpy( spec->groups[n++].name, *g, MAX_GROUP_NAME-1 );
    }

    spec->recv_max_groups = FB_SPREAD_NUM_GROUPS;
    spec->recv_groups = g_new0( sp_groupname_t, spec->recv_max_groups );

    spec->recv_max = FB_SPREAD_MTU;
    spec->recv_mess = g_new0( char, spec->recv_max );
    spec->num_groups_to_send = 0;
    fbSessionSetGroupParams(spec->session, spec->groups, spec->num_groups);

    return spec;
}

void fbConnSpreadFree(
    fbSpreadSpec_t *spec)
{
    g_free(spec->daemon);
    g_free(spec->groups);
    g_free(spec->recv_groups);
    g_free(spec->recv_mess);
    g_free(spec->groups_to_send);
    g_slice_free(fbSpreadSpec_t, spec);
}

const char * fbConnSpreadError(
    int err)
{
    switch (err)
    {
        case ILLEGAL_GROUP: return "illegal group";
        case ILLEGAL_SESSION: return "illegal session";
        case CONNECTION_CLOSED: return "connection closed";
        case ILLEGAL_SPREAD: return "illegal daemon name";
        case COULD_NOT_CONNECT: return "could not connect";
        case REJECT_VERSION: return "client/daemon version mismatch";
        case REJECT_NO_NAME: return "name with no name length, or no name and name length";
        case REJECT_ILLEGAL_NAME: return "illegal name (length or character)";
        case REJECT_NOT_UNIQUE: return "name not unique";
        default:
            break;
    }
    return "unknown error";
}

#endif /* HAVE_SPREAD */
