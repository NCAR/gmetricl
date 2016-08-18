#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <errno.h>
#include "csv.h"
#include "ganglia.h"
#include "gmetricl.h"

bool parse_input_file(char* file_name)
{
    FILE * file;

    errno = 0;
   
    if(!file_name)
	file = stdin;
    else
	file = fopen(file_name, "r");
    if(!file)
    {
	perror("Unable to open input file.");
	fprintf(stderr,"Input file: %s\n", file_name);
        return false;
    }

    ///alloc the buffers
    char* lbuffer = malloc(MAX_LINE_LEN);
    char** dbuffer = malloc(DATA_FIELD_CNT * sizeof(void*));
    if(!dbuffer) return false;
    for(int i = 0; i < DATA_FIELD_CNT; ++i)
    {
	dbuffer[i] = malloc(MAX_DATA_LEN + 1);
	if(!dbuffer[i]) return false;
	dbuffer[i][0] = '\0';
    }

    ///make sure buffer is clear, this may not be needed
    for(int i = 0; i < MAX_LINE_LEN; ++i)
	lbuffer[i] = '\0';

    ///read one line at a time
    int linecnt = 0;
    while(fgets(lbuffer, MAX_LINE_LEN, file) != NULL)
    {
	int i; ///index of lbuffer
	int f; ///index of data field
	int fi; ///index inside of data field
	bool quote; ///reading quote?
	char read; ///character read (null for none read)

	i = f = fi = 0;
	quote = false;
	read = '\0';
	++linecnt;
 
	for(int i = 0; i < DATA_FIELD_CNT; ++i)
	    dbuffer[i][0] = '\0';
 
	while(true)
	{
	    ///no quotes allowed (this was only found out after tested with ganglia and ganglia went nuts)
	    if(read == '"')
	    { 
		fprintf(stderr,"Warning: Ganglia can not handle quotes in string. Ignoring qoute on character %i on line %i\n", i-1, linecnt);
		read = '\0';
	    }

	    if(read)
	    {
		if(fi >= MAX_DATA_LEN)
		{
		    fprintf(stderr,"too many characters in field %i on line %i\n", f, linecnt);
		    return false;
		}

		assert(dbuffer);
		assert(dbuffer[f]);
		assert(f < DATA_FIELD_CNT);

		dbuffer[f][fi] = read;
		dbuffer[f][fi + 1] = '\0'; ///always make next null
		if(debug >= 10)
		    fprintf(stderr,"read %c (char %i) on field %i[%i]\n", read, i, f, fi);
		++fi;
		read = '\0';
	    }

	    ///end of line?
	    if(lbuffer[i] == '\0' || lbuffer[i] == '\n' || lbuffer[i] == '\r')
	    {
		if(debug >= 10)
		    fprintf(stderr,"new line %i\n", i);
		break;
	    }
	    
	    assert(i <= MAX_LINE_LEN);
	    if(i >= MAX_LINE_LEN)
		break;

	    ///comment?
	    if(lbuffer[i] == '#' && f == 0 && fi == 0)
		break;
 
	    if(quote)
	    {
		read = lbuffer[i];
		++i; 

		///another quote
		if(read == '"')
		{
		    if(i < MAX_LINE_LEN && lbuffer[i] == '"')
		    {
			if(debug >= 10)
			    fprintf(stderr,"read quotted quote on char %i\n", i+1); 

			///peak if its just a quote ""
			read = '"';
			++i;
		    }
		    else
		    {
			read = '\0';
			///out of chars
			///must be quote to end last field
			quote = false;

			if(debug >= 10)
			    fprintf(stderr,"stop quote on char %i\n", i);
		    }
		}
	    }
	    else ///not in quotes
	    {
		if(lbuffer[i] == '"' && fi == 0)
		{
		    if(debug >= 10)
			fprintf(stderr,"start quote on char %i\n", i);

		    quote = true;
		    ++i;
		}
		else if(lbuffer[i] == ',')
		{
		    ++i;
		    ++f;
		    fi = 0;
		    if(f >= DATA_FIELD_CNT)
		    {
			fprintf(stderr,"too many fields %i on line %i\n", f+1, linecnt);
			return false;
		    }
		}
		else ///not control chars
		{
		    read = lbuffer[i];
		    ++i;

		    ///chomp
		    if(read == ' ' || read == '\t')
		    {
			if(fi == 0)
			{
			    ///ignore leading space
			    read = '\0';
			}
			else
			{
			    ///ignore trailing space
			    for(int ii = i - 1; ii < MAX_LINE_LEN; ++ii)
			    {
				///seq
				if(lbuffer[ii] == ',' || lbuffer[ii] == '\n' || lbuffer[ii] == '\r')
				{
				    i = ii;
				    read = '\0';
				    break;
				}
				if(lbuffer[ii] != ' ' && lbuffer[ii] != '\t')
				{
				    ///not trailing ws, keep space
				    break;
				}
			    }
			}
		    }
		}

	    }
	}

	if(f) ///ignore comments
	{
	    if(f != DATA_FIELD_CNT - 1)
	    {
		fprintf(stderr,"too few fields %i (instead of %i) on line %i\n", f+1, DATA_FIELD_CNT, linecnt);
		return false; 
	    }

	    for(int i = 0; i < DATA_FIELD_CNT; ++i)
		if(i == 0 && i > 4 && i < 9)///allow ip,group,desc,title,slope,tmax,dmax to be empty
		if(dbuffer[i][0] == '\0')
		{
		    fprintf(stderr,"Empty field %i on line %i\n", i+1, linecnt);
		    return false;  
		}

	    if(!send_metric(
		dbuffer[0],
		dbuffer[1],
		dbuffer[2],
		dbuffer[3],
		dbuffer[4],
		dbuffer[5],
		dbuffer[6],
		dbuffer[7],
		dbuffer[8],
		dbuffer[9],
		dbuffer[10],
		dbuffer[11]))
	    {
		fprintf(stderr,"Unable to send metric\n");
		return false;
	    }
	}

    }

    ///cleanup
    for(int i = 0; i < DATA_FIELD_CNT; ++i)
	free(dbuffer[i]);
    free(dbuffer);
    free(lbuffer);
    fclose(file);
    return true;
}

