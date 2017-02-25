# GRRMOD

[![Build Status](https://travis-ci.org/GRRLIB/GRRMOD.svg?branch=master)](https://travis-ci.org/GRRLIB/GRRMOD)

GRRMOD is a MOD player library for Wii application developers.

It supports the following format:
 * 669 (Composer 669)
 * FAR (Farandole Composer)
 * GDM (General DigiMusic)
 * IMF (Imago Orpheus)
 * IT (Impulse Tracker)
 * MED (OctaMED)
 * MOD (15 and 31 instruments)
 * MTM (MultiTracker Module editor)
 * OKT (Amiga Oktalyzer)
 * S3M (Scream Tracker 3)
 * STM (Scream Tracker)
 * ULT (UltraTracker)
 * XM (FastTracker 2)

## Installing GRRMOD

To install GRRMOD in a single command:
```bash
  cd GRRMOD
  make clean && make && make install
```

This process may take some time depending on the speed of your PC.

## Using GRRMOD

After everything is installed, simply put
```c
    #include <grrmod.h>
```
at the top of your .c/.cpp file and use the functions as required

You will also need to add
```make
  -lgrrmod -laesnd
```
to the libs line in your makefile

...Remember the order of the libraries is critical.  You may (need to) insert
other libraries in the middle of the list, you may need to add others to the
start, or even the end - but do _not_ change the order of the libraries shown
here.
