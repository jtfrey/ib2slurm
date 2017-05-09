ib2slurm 0.2
============

A program that attempts to generate a slurm style topology.conf(5) file using
infiniband network discovery services.

This version is based on the original version that can be found at [bringhurst/ib2slurm on github](https://github.com/bringhurst/ib2slurm).

Building
--------

This version includes both a standard Unix Makefile as well as a CMakeLists.txt file for building the program using CMake.

Variables that control the CMake build include:

| Variable            | Type | Description                                                   |
| ------------------- | ---- | ------------------------------------------------------------- |
| ```OFED_PREFIX```         | Path | Path at which OFED has been installed; defaults to ```/usr``` |
| ```USE_SLURM_HOSTLISTS``` | Bool | When enabled, use the hostlist functionality in libslurm to   |
|                     |      | write in compact form; defaults to false                      |
| ```SLURM_PREFIX```        | Path | Path at which SLURM has been installed; no default            |

An example invocation might be:

```
$ mkdir build
$ cd build
$ cmake -DOFED_PREFIX=/usr \
>   -DUSE_SLURM_HOSTLISTS:BOOL=true \
>   -DSLURM_PREFIX=/home/1001/slurm/17.02.2 \
>   ..
-- The C compiler identification is GNU 4.4.7
-- Check for working C compiler: /usr/lib64/ccache/cc
-- Check for working C compiler: /usr/lib64/ccache/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Configuring done
-- Generating done
-- Build files have been written to: /home/1001/ib2slurm/build
$ make
   :
```

Usage
-----

```
  ib2slurm {options}

  [Fabric discovery]

    -C, --Ca <ca_name>           use the named CA
    -P, --Port <ca_port>         use the given port number on the CA
    -p, --progress               display progress information while fabric
                                 is discovered

  [Cached fabric]

    -l, --load-cache <path>      read a cached fabric definition from
                                 the file at the given path

  -o, --output <path>            write output topology configuration
                                 to the file at the given path
  -m, --node-name-map <path>     read CA-to-node-name map from the
                                 file at the given path
  -s, --lookup-names             map node GUIDs to names in the output
  -R, --deranged-lists           do not produce ranged name lists a'la SLURM
  -L, --linkspeed                include LinkSpeed values for switches
  -v, --verbose                  display additional information to stderr

  [version 0.2]

```

The program requires an in-memory representation of the InfiniBand fabric.  The ```ibnetdiscover``` command includes a ```--cache``` option that writes that in-memory representation to a file:
```
$ ibnetdiscover ... --cache ib-topology.cache
$ ls -l ib-topology.cache
-rw-r--r-- 1 user group 134907 May  8 14:12 ib-topology.cache
```
The ```--load-cache``` option to **ib2slurm** allows such a file to be reused:
```
$ ib2slurm --load-cache ib-topology.cache
```
Lacking that option, the program will use the InfiniBand fabric discovery library to create an in-memory representation of the network.  The ```--Ca``` and ```--Port``` options can be used to restrict the enumeration to a specific InfiniBand adapter or port on the adapter.

_name_map_ is the location of a node name map file formatted as described in the **ibnetdiscover**(8) man page. The node name map entries must be compatible with SLURM's node naming scheme.  If a _name_map_ is not provided, the program will attempt to infer hostnames from the description field associated with host adapters by using the first word present.  For example, the HCA description ```n000 HCA-1``` would yield a node name of ```n000```.  Lacking any name mapping, the GUID of the node will be used (and may not be acceptable to SLURM as-is).

If the ```--linkspeed``` option is used, the program will lookup the native link speed and width for a switch and use the product of those two integers as the relative speed of the switch.  For example, a Mellanox FDR SX6025 has a link width of 56 and a speed of 10, which would produce a ```LinkSpeed=560```.  Yes, this is 100% arbitrary and may not always work.  But it's a start.
