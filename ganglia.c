#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <apr.h>
#include <apr_strings.h>
#include <apr_pools.h>
#include <ganglia.h>
#include <assert.h>
#include <stdbool.h>
#include "gmetricl.h"
#include "net.h"
#include "ganglia.h"
#include "csv.h"

Ganglia_pool global_context;
Ganglia_gmond_config gmond_config;
Ganglia_udp_send_channels send_channels;     

bool load_ganglia()
{
    /* create the global context */
    global_context = Ganglia_pool_create(NULL);
    if(!global_context)
    {
      fprintf(stderr,"Unable to create global context. Exiting.\n");
      return false;
    }             

    /* parse the configuration file */
    gmond_config = Ganglia_gmond_config_create(config_file_name, 0);
    if(!gmond_config)
    {
      fprintf(stderr,"Problem parsing ganglia config. Exiting.\n");
      return false; 
    }

    /* build the udp send channels */
    send_channels = Ganglia_udp_send_channels_create(global_context, gmond_config);
    if(!send_channels)
    {
      fprintf(stderr,"Unable to create ganglia send channels. Exiting.\n");
      return false;
    }        

    return true;
}

bool unload_ganglia()
{
    Ganglia_pool_destroy(global_context);
}

bool send_metric( char *host, char* ip, char* group, char* desc, char* title, char *name, char *value, char *type, char *units, char* slope, char* str_tmax, char* str_dmax)
{
    unsigned int tmax, dmax;
    if(str_tmax[0] == '\0')
	tmax = 60;
    else if(sscanf(str_tmax, "%u", &tmax) != 1)
    {
	fprintf(stderr,"Unable to parse tmax %s into unsigned int.\n", str_tmax);
	return false; 
    }
    if(str_dmax[0] == '\0')
	dmax = 0;     
    else if(sscanf(str_dmax, "%u", &dmax) != 1)
    {
	fprintf(stderr,"Unable to parse dmax %s into unsigned int.\n", str_dmax);
	return false; 
    }
    if(slope[0] == '\0')
	strncpy(slope, "both", MAX_DATA_LEN);

    if(debug >= 5)
	fprintf(stderr,"metric:\n"
	    "\thost: %s\n"
	    "\tip: %s\n"
	    "\tgroup: %s\n"
	    "\tdesc: %s\n"
	    "\ttitle: %s\n"
	    "\tname: %s\n"
	    "\tvalue: %s\n"
	    "\ttype: %s\n"
	    "\tunits: %s\n"
	    "\tslop: %s\n"
	    "\ttmax: %u\n"
	    "\tdmax: %u\n",
	    host,
	    ip,
	    group,
	    desc,
	    title,
	    name,
	    value,
	    type,
	    units,
	    slope,
	    tmax,
	    dmax);
    
   
    Ganglia_metric gmetric;

    /* create the message */
    gmetric = Ganglia_metric_create(global_context);
    if(!gmetric)
    {
      fprintf(stderr,"Unable to allocate gmetric structure. Exiting.\n");
      return false;
    }
    apr_pool_t *gm_pool = (apr_pool_t*)gmetric->pool;     

    {
	int rval = Ganglia_metric_set( gmetric, name, value, type, units, cstr_to_slope(slope), tmax, dmax);
	if(rval)
	{
	  fprintf(stderr,"Unable to setup gmetric structure. Exiting.\n");
	  return false;
	}
    }

    if(host[0] != '\0')
    {
	///resolve ip if not set
	if(ip[0] == '\0')
	    if(!resolve_host(host, ip))
		return false;
 
	char hostip[MAX_DATA_LEN*2];
	if(snprintf(hostip, MAX_DATA_LEN*2, "%s:%s", ip, host) < 0)
	{
	    fprintf(stderr,"Unable to generate ip:host string. out of memory?\n");
	    return false; 
	}
    
	if(debug > 7)
	    fprintf(stderr,"Metric Host spoof: %s\n", hostip);

	Ganglia_metadata_add(gmetric, SPOOF_HOST, hostip);  
    }
    if(group[0] != '\0')
	Ganglia_metadata_add(gmetric, "GROUP", group);
    if(desc[0] != '\0')
	Ganglia_metadata_add(gmetric, "DESC", desc);
    if(title[0] != '\0')
	Ganglia_metadata_add(gmetric, "TITLE", title);       

    {
	int rval = Ganglia_metric_send(gmetric, send_channels);
	if(rval)
	{
	  fprintf(stderr,"There was an error sending to %d of the send channels.\n", rval);
	  return false;
	}
    }

    /* cleanup */
    Ganglia_metric_destroy(gmetric); 

    return true;
}

