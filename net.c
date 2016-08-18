#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "gmetricl.h"
#include "csv.h"
 
/**
 * @brief resolve host ip
 * @param host host string (will be set)
 * @param ip host ip (will be set upto MAX_DATA_LEN)
 * @return true on success
 */
bool resolve_host(char * host, char *ip)
{
    struct addrinfo hint;
    struct addrinfo *addy;
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_INET;
    if(resolve_host_canon_name)
	hint.ai_flags = AI_CANONNAME;
    addy = NULL;

    {
	const int result = getaddrinfo(host, NULL, &hint, &addy); 
	if(result)
	{
	    fprintf(stderr, "Unable to resolve %s with error: %s\n", host, gai_strerror(result));
	    return false;
	}
    }

    if(!addy || addy->ai_family != AF_INET || !addy->ai_addr || addy->ai_addr->sa_family != AF_INET || (resolve_host_canon_name && !addy->ai_canonname))
    {
	fprintf(stderr, "Unable to resolve %s. Resolve returned but no address returned.\n", host);
	return false;
    }

    assert(MAX_DATA_LEN >= INET_ADDRSTRLEN);
    struct sockaddr_in *sinp = (struct sockaddr_in *) addy->ai_addr;
    if(inet_ntop(AF_INET, &sinp->sin_addr, ip, MAX_DATA_LEN) != ip)
    {
	fprintf(stderr, "Unable to convert resolved ip to string for host %s.\n", host);
	return false;
    }

    ///copy over full hostname
    if(resolve_host_canon_name)
	if(addy->ai_canonname)
	{
	    strncpy(host, addy->ai_canonname, MAX_DATA_LEN);
	    host[MAX_DATA_LEN - 1] = '\0';
	    if(debug > 7)
		fprintf(stderr, "Hostname resolved to  %s\n", host);
	}

    freeaddrinfo(addy);

    return true;
}
 
