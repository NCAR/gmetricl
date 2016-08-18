# gmetricl
Ganglia gmetric implementation for long lists of metrics using CSV files

To Build:

1. Install Ganglia-core

  * option 1: Install from source [https://github.com/ganglia/monitor-core]
  * option 2: Ubuntu Install:  sudo apt-get install libganglia1-dev libapr1-dev libaprutil1-dev libconfuse-dev libpcre3-dev
  * option 3: Other Distro: install all of Ganglia's dependencies
    *  ganglia-core: [https://github.com/ganglia/monitor-core]
    *  apr1: [https://apr.apache.org/download.cgi]
    *  aprutil1: [git://git.apache.org/apr-util.git]
    *  libconfuse: [https://github.com/martinh/libconfuse]
    *  libpcre: [http://www.pcre.org/]

2. Configure Ganglia

  It doesn't matter how you configure Ganglia to use gmetricl. You only need a local instance of gmond running on the host you execute gmetricl on for it to work.

3. Build gmetricl

```
  git clone [https://github.com/NCAR/gmetricl.git] gmetricl
  mkdir gmetricl/build/
  cd gmetricl/build/
  cmake .
  make
  make install
```

4. Call gmetricl to get help

  Default install location: /usr/local/bin/gmetricl
  ```
  $ /usr/local/bin/gmetricl
  ./gmetricl [-r] [-d 1-10] {-c config_file} [-i input_file [rfc4180 without new lines]]
  -r                              Resolve Host Canon Name. This allow short hand names to be used in input file while full name is given to Ganglia
  -d {1-10}                       Debug level. Must be 1 to 10 with 10 being most verbose.
  -c {config file name}           Path to gmond config file.
  -i {input file name}            Path to CSV input file. Optional. Will default to stdin. Format:
                  host, ip, group, description, title, name, value, type, units, slope, tmax, dmax
                          host            Explicit Name of host (allows spoofing). Optional. Leave blank to use local hostname.
                          ip              Explicit IP of host (allows spoofing). Optional. Leave blank to automatically resolve ip associated with host. Note that ganglia requires that host have an assocatied ip.
                          group           Ganglia Group. Optional.
                          description             Ganglia Metric Description. Optional.
                          title           Ganglia Metric Title. Optional.
                          name            Ganglia Metric name
                          value           Sensed Metric Value
                          type            Metric Data type: string|int8|uint8|int16|uint16|int32|uint32|float|double
                          units           Metric unit of measure.
                          slope           Slope of metric: zero|positive|negative|both Default: both
                          tmax            The maximum time in seconds between gmetric calls. Default: 60
                          dmax            The lifetime in seconds of this metric. Default: 0 (never times out).
                  Note: csv file is rfc4180 compliant with the following exceptions:
                          No Quotes (in Quotes): Quotes can be used in formating, but Ganglia can't handle quotes and they will be ignored
                          No New lines (in Quotes): New lines make no sense in this context and will result in an error.
                          Optional Fields can be empty but must still have field seperator ',' to ensure data is ordered correctly.
                          Comments: Comments will be ignored when line starts with '#' character.
   ```

5. Example CSV File:

```
	#host, ip, group, description, title, name, value, type, units, slope, tmax, dmax
	node0,,test group, test description, test title, test metric, 1.2, float, test units,,,
	node1,,test group, test description, test title 2, test metric, 4.2, float, test units,,,
	node2,,test group, test description, test title 3, test metric 3, 0.2, float, test units,,,
```


