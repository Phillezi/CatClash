#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../include/getDefaultGateway.h"

#ifdef _WIN32
#define INET_ADDRSTRLEN 16
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#elif defined(__linux__)
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#elif defined(__APPLE__)
#include <arpa/inet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sysctl.h>
#include <unistd.h>
#endif

int getDefaultGateway(char *gateway_ip)
{
    //char gateway_ip[INET_ADDRSTRLEN];
    memset(gateway_ip, 0, sizeof(gateway_ip));

// Platform-specific code to retrieve default gateway IP
#ifdef _WIN32
    // Windows code
    PMIB_IPFORWARDTABLE pIpForwardTable;
    DWORD dwSize = 0;

    if (GetIpForwardTable(NULL, &dwSize, false) == ERROR_INSUFFICIENT_BUFFER)
    {
        pIpForwardTable = (PMIB_IPFORWARDTABLE)malloc(dwSize);
        if (pIpForwardTable == NULL)
        {
            fprintf(stderr, "Error: Memory allocation failed.\n");
            return EXIT_FAILURE;
        }
    }

    if (GetIpForwardTable(pIpForwardTable, &dwSize, false) != NO_ERROR)
    {
        fprintf(stderr, "Error: GetIpForwardTable failed.\n");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < pIpForwardTable->dwNumEntries; i++)
    {
        if (pIpForwardTable->table[i].dwForwardDest == 0 && pIpForwardTable->table[i].dwForwardProto == 3)
        {
            // Default gateway found
            struct in_addr gateway_addr;
            gateway_addr.S_un.S_addr = (u_long)pIpForwardTable->table[i].dwForwardNextHop;
            inet_ntop(AF_INET, &gateway_addr, gateway_ip, INET_ADDRSTRLEN);
            break;
        }
    }

    free(pIpForwardTable);
#elif defined(__linux__)
    // Linux code
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0)
    {
        fprintf(stderr, "Error: socket creation failed.\n");
        return EXIT_FAILURE;
    }

    struct rtentry route;
    memset(&route, 0, sizeof(route));
    route.rt_dst.sa_family = AF_INET;

    struct sockaddr_in *addr = (struct sockaddr_in *)&route.rt_gateway;
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;

    ioctl(sock, SIOCRTMSG, &route);

    struct sockaddr_in *gw_addr = (struct sockaddr_in *)&route.rt_gateway;
    inet_ntop(AF_INET, &gw_addr->sin_addr, gateway_ip, INET_ADDRSTRLEN);

    close(sock);
#elif defined(__APPLE__)
    // macOS code
    int mib[6];
    size_t len;
    char *buf;
    struct in_addr gateway_addr;

    mib[0] = CTL_NET;
    mib[1] = AF;
    mib[2] = 0;
    mib[3] = AF_INET;
    mib[4] = NET_RT_FLAGS;
    mib[5] = RTF_GATEWAY;

    if (sysctl(mib, 6, NULL, &len, NULL, 0) < 0)
    {
        fprintf(stderr, "Error: sysctl failed (1).\n");
        return EXIT_FAILURE;
    }

    buf = (char *)malloc(len);
    if (buf == NULL)
    {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return EXIT_FAILURE;
    }

    if (sysctl(mib, 6, buf, &len, NULL, 0) < 0)
    {
        fprintf(stderr, "Error: sysctl failed (2).\n");
        return EXIT_FAILURE;
    }

    char *lim = buf + len;
    for (char *next = buf; next < lim;)
    {
        struct rt_msghdr *rtm = (struct rt_msghdr *)next;
        struct sockaddr *sa = (struct sockaddr *)(rtm + 1);
        struct sockaddr_in *gw_sa = NULL;

        next += rtm->rtm_msglen;

        if (rtm->rtm_version != RTM_VERSION || rtm->rtm_type != RTM_GET ||
            rtm->rtm_flags & RTF_DONE || rtm->rtm_errno != 0)
        {
            continue;
        }

        for (; sa < (struct sockaddr *)next; sa = (struct sockaddr *)((char *)sa + SA_SIZE(sa)))
        {
            if (sa->sa_family == AF_INET && sa->sa_len == sizeof(struct sockaddr_in))
            {
                struct sockaddr_in *sin = (struct sockaddr_in *)sa;

                if (sin->sin_addr.s_addr == INADDR_ANY)
                {
                    continue;
                }

                if (rtm->rtm_addrs & RTA_GATEWAY)
                {
                    gw_sa = (struct sockaddr_in *)(sa + SA_SIZE(sa));
                }

                if (gw_sa == NULL || gw_sa->sin_addr.s_addr == INADDR_ANY)
                {
                    continue;
                }

                gateway_addr = gw_sa->sin_addr;
                inet_ntop(AF_INET, &gateway_addr, gateway_ip, INET_ADDRSTRLEN);
                goto done;
            }
        }
    }

    fprintf(stderr, "Error: Default gateway not found.\n");
    return EXIT_FAILURE;
done:
    free(buf);
#else
#error Unsupported operating system
#endif
    //printf("Default gateway IP: %s\n", gateway_ip);

    return EXIT_SUCCESS;
}
