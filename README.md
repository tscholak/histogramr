# histogramr
### multivariate histograms of continuous data
histogramr processes data from members (named columns) of HDF5 data sets of compound data type. It can use data from compound members spread over different data sets. histogramr produces a multivariate histogram, i.e. an approximate multivariate probability density function (PDF) discretized on a multidimensional rectangular regular grid of predefined shape. histogramr offers control over the histogram limits, the binning (grid spacing), and whether or not log-transformed data is used. histogramr creates an HDF5 output file with the PDF.

## Current status
The current branch continuous integration status:
[![Build Status](https://travis-ci.org/tscholak/histogramr.png)](https://travis-ci.org/tscholak/histogramr)

## Installation
histogramr has been tested on 64-bit Linux and Mac OS X. It depends on the following libraries:
* [HDF5](http://www.hdfgroup.org/HDF5/)
* [GNU Scientific Library](https://www.gnu.org/software/gsl/)

### Obtaining the source code
```
$ git clone git@github.com:tscholak/histogramr.git
```

### Building histogramr
histogramr uses the autotools. Run:
```
$ autogen.sh
$ ./configure
$ make
```
Should you prefer the Intel compiler, run:
```
$ env CC=icc make
```
You may also want to create a soft link to the executable:
```
$ ln -sf ./src/histogramr ~/bin
```

## Usage
histogramr reads in the input files one-by-one and commits the data to the histogram data structure. The output file is written multiple times, after a predetermined number of  

### Command line arguments
```
histogramr: create multivariate histograms of continuous data

Usage: histogramr -d <dsname1> -m <mname1[:mname2...]>
  -b <size1[:size2...]> -l <range1[:range2...]>
  [-L <boolean1[:boolean2...]>] [-d <dsname2> ...] [-e <number>]
  -o <outfile> <infile1> [<infile2> ...]

Mandatory options:
  -d, --dataset <dsname>     data set(s) must be specified first
  -m, --member <mname>       data set member(s)
  -b, --binning <size>       histogram binning(s)
  -l, --limit <range>        histogram limits
  -o, --output <outfile>     name the output file

Optional options:
  -e, --save-every <number>  save every <number> of files
                             (default: 1)
  -L, --l10 <boolean>        logarithmic transform (default: false)

Other options:
  -h|--help                  print this help message and quit
  -v|--version               print version information and quit

Report bugs to: torsten.scholak+histogramr@googlemail.com
histogramr home page: <https://github.com/tscholak/histogramr>
```

### Examples
So far, histogramr has processed data for the following publications:
* Torsten Scholak, Thomas Wellens, Andreas Buchleitner, "Spectral Backbone of Excitation Transport in Ultra-Cold Rydberg Gases", Phys. Rev. A 90, 063415 (2014)
* Tobias Zech, Mattia Walschaers, Torsten Scholak, Roberto Mulet, Thomas Wellens, Andreas Buchleitner, "Quantum transport in biological functional units: noise, disorder, structure", Fluct. Noise Lett. 12, 1340007 (2013)
* Torsten Scholak, Tobias Zech, Thomas Wellens and Andreas Buchleitner, "Disorder-assisted exciton transport", Acta Phys. Pol. A 120, 89 (2011)
* Torsten Scholak, Thomas Wellens, and Andreas Buchleitner, "Optimal networks for excitonic energy transport", J. Phys. B: At. Mol. Opt. Phys. 44, 184012 (2011)
* Torsten Scholak, Thomas Wellens, and Andreas Buchleitner, "The optimization topography of exciton transport", Europhys. Lett. 96, 10001 (2011)
* Torsten Scholak, Fernando de Melo, Thomas Wellens, Florian Mintert, and Andreas Buchleitner, "Efficient and coherent excitation transfer across disordered molecular networks", Phys. Rev. E 83, 021912 (2011)
* Torsten Scholak, Florian Mintert, Thomas Wellens and Andreas Buchleitner, "Transport and entanglement", Semiconductors and Semimetals 83, 1 (2010)
