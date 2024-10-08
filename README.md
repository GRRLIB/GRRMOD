# GRRMOD

[![Actions Status](https://github.com/GRRLIB/GRRMOD/workflows/Continuous%20Integration/badge.svg)](https://github.com/GRRLIB/GRRMOD/actions)
[![Documentation](https://codedocs.xyz/GRRLIB/GRRMOD.svg)](https://codedocs.xyz/GRRLIB/GRRMOD/)

GRRMOD is a MOD player library for Wii application developers.

It supports the following formats:

* 669 (Composer 669)
* AMF (DSMI Advanced Module Format)
* AMF (ASYLUM Music Format V1.0)
* APUN (APlayer)
* DSM (DSIK internal format)
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
* STX (Scream Tracker Music Interface Kit)
* ULT (UltraTracker)
* UNI (MikMod)
* XM (FastTracker 2)

## Installing GRRMOD

GRRMOD can be installed using Make or CMake.

With Make:

```bash
cd GRRMOD
make clean && make && make install
```

With CMake:

```bash
/opt/devkitpro/portlibs/wii/bin/powerpc-eabi-cmake -B build
cmake --build build --target install
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

## Licence

See the [LICENCE](LICENCE.md) file for licence rights and limitations (MIT).
