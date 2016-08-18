#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "gmetricl.h"
#include "ganglia.h"
#include "csv.h"

int debug;
char * config_file_name;
char * input_file_name;
bool resolve_host_canon_name;         

/**
 * @brief read command options
 */
bool read_opts(int argc, char **argv)
{
    extern char *optarg;
    extern int optind, optopt;
    int c;

    while ((c = getopt(argc, argv, "rd:c:i:")) != -1) 
    {
	switch (c)
	{
	    case 'r': //resolve host cannon name
		resolve_host_canon_name = true;   
		break;
	    case 'd': //debug mode
		sscanf(optarg, "%i", &debug);
		if(debug < 0 || debug > 10)
		{
		    fprintf(stderr,"Unexpected debug level %i\n", debug);
		    return false;
		}
		break;
	    case 'c': ///config
		config_file_name = optarg;
		assert(config_file_name);
		break;
	    case 'i': ///input file
		input_file_name = optarg;
		assert(input_file_name);
		break;
	    default:
		fprintf(stderr,"Unexpected argument %c\n", optopt);
		return false; 	
	}
    } 

    return true;
}

int main(int argc, char **argv)
{   
    config_file_name = NULL;
    input_file_name = NULL;
    debug = false;
    resolve_host_canon_name = false;

    if(!read_opts(argc, argv) || !config_file_name)
    {
	fprintf(stderr,
	    "%s [-r] [-d 1-10] {-c config_file} [-i input_file [rfc4180 without new lines]]\n"
	    "-r\t\t\t\tResolve Host Canon Name. This allow short hand names to be used in input file while full name is given to Ganglia\n"
	    "-d {1-10}\t\t\tDebug level. Must be 1 to 10 with 10 being most verbose.\n"
	    "-c {config file name}\t\tPath to gmond config file.\n"
	    "-i {input file name}\t\tPath to CSV input file. Optional. Will default to stdin. Format:\n"
	    "\t\thost, ip, group, description, title, name, value, type, units, slope, tmax, dmax\n"
	    "\t\t\thost\t\tExplicit Name of host (allows spoofing). Optional. Leave blank to use local hostname.\n"
	    "\t\t\tip\t\tExplicit IP of host (allows spoofing). Optional. Leave blank to automatically"
		" resolve ip associated with host. Note that ganglia requires that host have an assocatied ip.\n"
	    "\t\t\tgroup\t\tGanglia Group. Optional.\n"
	    "\t\t\tdescription\t\tGanglia Metric Description. Optional.\n"
	    "\t\t\ttitle\t\tGanglia Metric Title. Optional.\n"
	    "\t\t\tname\t\tGanglia Metric name\n"
	    "\t\t\tvalue\t\tSensed Metric Value\n"
	    "\t\t\ttype\t\tMetric Data type: string|int8|uint8|int16|uint16|int32|uint32|float|double\n"
	    "\t\t\tunits\t\tMetric unit of measure.\n"
	    "\t\t\tslope\t\tSlope of metric: zero|positive|negative|both Default: both\n"
	    "\t\t\ttmax\t\tThe maximum time in seconds between gmetric calls. Default: 60\n"
	    "\t\t\tdmax\t\tThe lifetime in seconds of this metric. Default: 0 (never times out).\n"
	    "\t\tNote: csv file is rfc4180 compliant with the following exceptions:\n"
	    "\t\t\tNo Quotes (in Quotes): Quotes can be used in formating, but Ganglia can't handle quotes and they will be ignored\n"
	    "\t\t\tNo New lines (in Quotes): New lines make no sense in this context and will result in an error.\n"
	    "\t\t\tOptional Fields can be empty but must still have field seperator ',' to ensure data is ordered correctly.\n"
	    "\t\t\tComments: Comments will be ignored when line starts with '#' character.\n"
	    , argv[0]);
	return EXIT_FAILURE;
    }

    //fprintf(stderr,"%s %s\n", config_file_name, input_file_name);

    if(load_ganglia())
    {
	if(debug >= 2)
	    fprintf(stderr,"Ganglia Loaded successfully.\n");
    }
    else //failed
    {
	fprintf(stderr,"Ganglia Load failure\n");
	return EXIT_FAILURE;
    }

    if(!parse_input_file(input_file_name))
    {
	fprintf(stderr,"Input File parse failure\n");
	return EXIT_FAILURE;
    }

    unload_ganglia();

    return EXIT_SUCCESS;
}

