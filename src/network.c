/*
 * network.c — Missing networking APIs for Solaris 7
 *
 * getaddrinfo/freeaddrinfo/gai_strerror/getnameinfo, 
 * inet_ntop/inet_pton, getifaddrs/freeifaddrs
 *
 * getaddrinfo implementation is based on gethostbyname/getservbyname
 * which are available on Solaris 7.  IPv6 is not supported (Solaris 7
 * has experimental IPv6 but we don't rely on it).
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sockio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <net/if.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>

/* Unhide — we define the real functions here */
#undef getaddrinfo
#undef freeaddrinfo
#undef gai_strerror
#undef getnameinfo

/* Use our fixed snprintf */
extern int solcompat_snprintf(char *, size_t, const char *, ...);

/* Pull in our own struct definitions */
#include "solcompat/network.h"

/*
 * getaddrinfo — resolve hostname and service to socket addresses.
 * This is an IPv4-only implementation using gethostbyname/getservbyname.
 */
int
getaddrinfo(const char *node, const char *service,
            const struct addrinfo *hints, struct addrinfo **res)
{
    struct hostent *he = NULL;
    struct servent *se = NULL;
    struct addrinfo *head = NULL, **tail = &head;
    struct sockaddr_in *sin;
    int port = 0;
    int socktype = 0;
    int protocol = 0;
    int flags = 0;
    int family = AF_UNSPEC;
    int i;

    if (!node && !service)
        return EAI_NONAME;

    if (hints) {
        flags = hints->ai_flags;
        family = hints->ai_family;
        socktype = hints->ai_socktype;
        protocol = hints->ai_protocol;

        if (family != AF_UNSPEC && family != AF_INET)
            return EAI_FAMILY;
    }

    /* Resolve service */
    if (service) {
        char *endptr;
        long portnum = strtol(service, &endptr, 10);
        if (*endptr == '\0' && portnum >= 0 && portnum <= 65535) {
            port = htons((unsigned short)portnum);
        } else {
            const char *proto = NULL;
            if (socktype == SOCK_STREAM) proto = "tcp";
            else if (socktype == SOCK_DGRAM) proto = "udp";
            se = getservbyname(service, proto);
            if (!se)
                return EAI_SERVICE;
            port = se->s_port;
        }
    }

    /* Resolve hostname */
    if (node) {
        if (flags & AI_NUMERICHOST) {
            struct in_addr addr;
            if (inet_aton(node, &addr) == 0)
                return EAI_NONAME;

            /* Create single result */
            struct addrinfo *ai = (struct addrinfo *)calloc(1, sizeof(*ai) + sizeof(struct sockaddr_in));
            if (!ai)
                return EAI_MEMORY;

            sin = (struct sockaddr_in *)(ai + 1);
            sin->sin_family = AF_INET;
            sin->sin_port = port;
            sin->sin_addr = addr;

            ai->ai_family = AF_INET;
            ai->ai_socktype = socktype ? socktype : SOCK_STREAM;
            ai->ai_protocol = protocol;
            ai->ai_addrlen = sizeof(struct sockaddr_in);
            ai->ai_addr = (struct sockaddr *)sin;
            ai->ai_next = NULL;

            *res = ai;
            return 0;
        }

        he = gethostbyname(node);
        if (!he) {
            switch (h_errno) {
            case HOST_NOT_FOUND: return EAI_NONAME;
            case TRY_AGAIN:      return EAI_AGAIN;
            case NO_RECOVERY:    return EAI_FAIL;
            default:             return EAI_NONAME;
            }
        }
    }

    if (he) {
        /* Create a result for each address */
        for (i = 0; he->h_addr_list[i]; i++) {
            struct addrinfo *ai = (struct addrinfo *)calloc(1, sizeof(*ai) + sizeof(struct sockaddr_in));
            if (!ai) {
                freeaddrinfo(head);
                return EAI_MEMORY;
            }

            sin = (struct sockaddr_in *)(ai + 1);
            sin->sin_family = AF_INET;
            sin->sin_port = port;
            memcpy(&sin->sin_addr, he->h_addr_list[i], (size_t)he->h_length);

            ai->ai_family = AF_INET;
            ai->ai_socktype = socktype ? socktype : SOCK_STREAM;
            ai->ai_protocol = protocol;
            ai->ai_addrlen = sizeof(struct sockaddr_in);
            ai->ai_addr = (struct sockaddr *)sin;
            ai->ai_next = NULL;

            if ((flags & AI_CANONNAME) && i == 0 && he->h_name) {
                ai->ai_canonname = strdup(he->h_name);
            }

            *tail = ai;
            tail = &ai->ai_next;
        }
    } else {
        /* No node specified — passive or loopback */
        struct addrinfo *ai = (struct addrinfo *)calloc(1, sizeof(*ai) + sizeof(struct sockaddr_in));
        if (!ai)
            return EAI_MEMORY;

        sin = (struct sockaddr_in *)(ai + 1);
        sin->sin_family = AF_INET;
        sin->sin_port = port;
        sin->sin_addr.s_addr = (flags & AI_PASSIVE) ? INADDR_ANY : htonl(INADDR_LOOPBACK);

        ai->ai_family = AF_INET;
        ai->ai_socktype = socktype ? socktype : SOCK_STREAM;
        ai->ai_protocol = protocol;
        ai->ai_addrlen = sizeof(struct sockaddr_in);
        ai->ai_addr = (struct sockaddr *)sin;
        ai->ai_next = NULL;

        *tail = ai;
    }

    if (!head)
        return EAI_NONAME;

    /* If no socktype specified, duplicate results for TCP and UDP */
    if (hints && hints->ai_socktype == 0) {
        struct addrinfo *p;
        struct addrinfo *udp_head = NULL, **udp_tail = &udp_head;
        for (p = head; p; p = p->ai_next) {
            struct addrinfo *dup = (struct addrinfo *)calloc(1, sizeof(*dup) + sizeof(struct sockaddr_in));
            if (!dup) break;
            memcpy(dup, p, sizeof(*dup));
            sin = (struct sockaddr_in *)(dup + 1);
            memcpy(sin, p->ai_addr, sizeof(struct sockaddr_in));
            dup->ai_addr = (struct sockaddr *)sin;
            dup->ai_socktype = SOCK_DGRAM;
            dup->ai_protocol = IPPROTO_UDP;
            dup->ai_canonname = NULL;
            dup->ai_next = NULL;
            *udp_tail = dup;
            udp_tail = &dup->ai_next;
        }
        /* Append UDP results after TCP */
        for (p = head; p->ai_next; p = p->ai_next)
            ;
        p->ai_next = udp_head;
    }

    *res = head;
    return 0;
}

void
freeaddrinfo(struct addrinfo *res)
{
    struct addrinfo *next;
    while (res) {
        next = res->ai_next;
        if (res->ai_canonname)
            free(res->ai_canonname);
        free(res);
        res = next;
    }
}

const char *
gai_strerror(int errcode)
{
    switch (errcode) {
    case 0:            return "Success";
    case EAI_AGAIN:    return "Temporary failure in name resolution";
    case EAI_BADFLAGS: return "Invalid value for ai_flags";
    case EAI_FAIL:     return "Non-recoverable failure in name resolution";
    case EAI_FAMILY:   return "ai_family not supported";
    case EAI_MEMORY:   return "Memory allocation failure";
    case EAI_NONAME:   return "Name or service not known";
    case EAI_SERVICE:  return "Servname not supported for ai_socktype";
    case EAI_SOCKTYPE: return "ai_socktype not supported";
    case EAI_SYSTEM:   return "System error";
    default:           return "Unknown error";
    }
}

int
getnameinfo(const struct sockaddr *sa, socklen_t salen,
            char *host, socklen_t hostlen,
            char *serv, socklen_t servlen, int flags)
{
    const struct sockaddr_in *sin;
    struct hostent *he;
    struct servent *se;

    if (sa->sa_family != AF_INET)
        return EAI_FAMILY;

    sin = (const struct sockaddr_in *)sa;

    if (host && hostlen > 0) {
        if (flags & NI_NUMERICHOST) {
            char *ip = inet_ntoa(sin->sin_addr);
            if (strlen(ip) >= hostlen)
                return EAI_OVERFLOW;
            strcpy(host, ip);
        } else {
            he = gethostbyaddr((const char *)&sin->sin_addr,
                               sizeof(sin->sin_addr), AF_INET);
            if (he) {
                if (strlen(he->h_name) >= hostlen)
                    return EAI_OVERFLOW;
                strcpy(host, he->h_name);
            } else {
                if (flags & NI_NAMEREQD)
                    return EAI_NONAME;
                char *ip = inet_ntoa(sin->sin_addr);
                if (strlen(ip) >= hostlen)
                    return EAI_OVERFLOW;
                strcpy(host, ip);
            }
        }
    }

    if (serv && servlen > 0) {
        if (flags & NI_NUMERICSERV) {
            solcompat_snprintf(serv, servlen, "%d", ntohs(sin->sin_port));
        } else {
            const char *proto = (flags & NI_DGRAM) ? "udp" : "tcp";
            se = getservbyport(sin->sin_port, proto);
            if (se) {
                if (strlen(se->s_name) >= servlen)
                    return EAI_OVERFLOW;
                strcpy(serv, se->s_name);
            } else {
                solcompat_snprintf(serv, servlen, "%d", ntohs(sin->sin_port));
            }
        }
    }

    return 0;
}

/*
 * inet_ntop — convert binary address to presentation string
 */
const char *
inet_ntop(int af, const void *src, char *dst, socklen_t size)
{
    if (af == AF_INET) {
        const unsigned char *p = (const unsigned char *)src;
        int n = solcompat_snprintf(dst, size, "%u.%u.%u.%u",
                                   p[0], p[1], p[2], p[3]);
        if (n < 0 || (socklen_t)n >= size) {
            errno = ENOSPC;
            return NULL;
        }
        return dst;
    }
    errno = EAFNOSUPPORT;
    return NULL;
}

/*
 * inet_pton — convert presentation string to binary address
 */
int
inet_pton(int af, const char *src, void *dst)
{
    if (af == AF_INET) {
        struct in_addr addr;
        if (inet_aton(src, &addr) == 0)
            return 0;
        memcpy(dst, &addr, sizeof(addr));
        return 1;
    }
    errno = EAFNOSUPPORT;
    return -1;
}

/*
 * getifaddrs — enumerate network interfaces
 * Uses SIOCGIFCONF ioctl which is available on Solaris 7
 */
int
getifaddrs(struct ifaddrs **ifap)
{
    int sock;
    struct ifconf ifc;
    struct ifreq *ifr;
    char buf[4096];
    struct ifaddrs *head = NULL, **tail = &head;
    int i, n;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
        return -1;

    ifc.ifc_len = sizeof(buf);
    ifc.ifc_buf = buf;

    if (ioctl(sock, SIOCGIFCONF, &ifc) < 0) {
        close(sock);
        return -1;
    }

    n = ifc.ifc_len / sizeof(struct ifreq);
    ifr = ifc.ifc_req;

    for (i = 0; i < n; i++) {
        struct ifaddrs *ifa;
        struct sockaddr_in *addr, *mask;
        struct ifreq flagreq, maskreq;

        ifa = (struct ifaddrs *)calloc(1, sizeof(struct ifaddrs));
        if (!ifa) {
            freeifaddrs(head);
            close(sock);
            errno = ENOMEM;
            return -1;
        }

        ifa->ifa_name = strdup(ifr[i].ifr_name);

        /* Get flags */
        memset(&flagreq, 0, sizeof(flagreq));
        strcpy(flagreq.ifr_name, ifr[i].ifr_name);
        if (ioctl(sock, SIOCGIFFLAGS, &flagreq) == 0)
            ifa->ifa_flags = flagreq.ifr_flags;

        /* Address */
        addr = (struct sockaddr_in *)calloc(1, sizeof(*addr));
        if (addr) {
            memcpy(addr, &ifr[i].ifr_addr, sizeof(struct sockaddr_in));
            ifa->ifa_addr = (struct sockaddr *)addr;
        }

        /* Netmask */
        memset(&maskreq, 0, sizeof(maskreq));
        strcpy(maskreq.ifr_name, ifr[i].ifr_name);
        if (ioctl(sock, SIOCGIFNETMASK, &maskreq) == 0) {
            mask = (struct sockaddr_in *)calloc(1, sizeof(*mask));
            if (mask) {
                memcpy(mask, &maskreq.ifr_addr, sizeof(struct sockaddr_in));
                ifa->ifa_netmask = (struct sockaddr *)mask;
            }
        }

        ifa->ifa_next = NULL;
        *tail = ifa;
        tail = &ifa->ifa_next;
    }

    close(sock);
    *ifap = head;
    return 0;
}

void
freeifaddrs(struct ifaddrs *ifa)
{
    struct ifaddrs *next;
    while (ifa) {
        next = ifa->ifa_next;
        if (ifa->ifa_name)   free(ifa->ifa_name);
        if (ifa->ifa_addr)   free(ifa->ifa_addr);
        if (ifa->ifa_netmask) free(ifa->ifa_netmask);
        if (ifa->ifa_broadaddr) free(ifa->ifa_broadaddr);
        free(ifa);
        ifa = next;
    }
}
