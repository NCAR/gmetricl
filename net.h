#ifndef GMETRICL_NET_H
#define GMETRICL_NET_H
 
/**
 * @brief resolve host ip
 * @param host host string (will be set upto MAX_DATA_LEN)  
 * @param ip host ip (will be set upto MAX_DATA_LEN)
 * @return true on success
 */
bool resolve_host(char * host, char *ip);

#endif
