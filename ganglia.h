#ifndef GMETRICL_GANGLIA_H
#define GMETRICL_GANGLIA_H
 
/**
 * @brief Load and Setup Ganglia
 */
bool load_ganglia();

/**
 * @brief unload ganglia
 */
bool unload_ganglia();

/** 
 * @brief send a single (spoof) metric
 */
bool send_metric( char *host, char* ip, char* group, char* desc, char* title, char *name, char *value, char *type, char *units, char* slope, char* str_tmax, char* str_dmax);

#endif
