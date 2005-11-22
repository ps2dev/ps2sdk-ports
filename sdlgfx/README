
SDL_gfx - SDL graphics drawing primitives and other support functions
=============================================================================

Email aschiffler@appwares.ca to contact the author or better check
author's homepage at http://www.ferzkopp.net for the most up-to-date
contact information.

This library is licenced under the LGPL, see the file LICENSE for details. 


Intro
-----

The SDL_gfx library evolved out of the SDL_gfxPrimitives code which
provided basic drawing routines such as lines, circles or polygons for 
SDL Surfaces and adding a couple other useful functions for zooming 
images for example and doing basic image processing on byte arrays.

The current components of the SDL_gfx library are:
- Graphic Primitives (SDL_gfxPrimitves.h)
- Rotozoomer (SDL_rotozoom.h)
- Framerate control (SDL_framerate.h)
- MMX image filters (SDL_imageFilter.h)

See ./Docs directory for a longer HTML version of this document.


Supported Platforms
-------------------

The library compiles and is tested for a Linux target (gcc compiler) and 
a Win32 target (VisualC6/7, xmingw32 cross-compiler). MacOS X target is 
reported to work (Project Builder). QNX is reported to build well (see diff).

When using the cross-compiler (available on the author's homepage), 
the build process generates .DLLs. You can use the command line 'LIB.EXE' 
tool to generate VC6 compatible .LIB files for linking purposes. 

Other platforms might work but have not been tested by the author.

See below for build instructions.


Notes on Graphics Primitives
----------------------------

Care has been taken so that all routines are fully alpha-aware and can 
blend any primitive onto the target surface if ALPHA<255. Surface depths 
supported are 1,2,3 and 4 bytes per pixel. Surface locking is implemented
in each routine and the library should work well with hardware 
accelerated surfaces. 

Currently, The following Anti-Aliased drawing primitives are available:
- AA-line
- AA-polygon
- AA-circle
- AA-ellipse

[[[ Interface ]]]

See SDL_gfxPrimitives.h for all the drawing functions.


Notes on Rotozoomer
-------------------

The rotozoom code is not ASSEMBLY quality - but it should be fast enough 
even for some realtime effects if the CPU is good or bitmaps small.
With interpolation the routines are typically used for pre-rendering stuff 
in higher quality (i.e. smoothing) - that's also a reason why the API differs 
from SDL_BlitRect() and creates a new target surface each time rotozoom 
is called. The final rendering speed is dependent on the target surface
size as it is beeing xy-scanned when rendering the new surface.

Note also that the smoothing toggle is dependent on the input surface bit 
depth. 8bit surfaces will never be smoothed - only 32bit surfaces will.

Note that surfaces of other bit depth then 8 and 32 will be converted 
on the fly to a 32bit surface using a blit into a temporary surface. This 
impacts performance somewhat.


[[[ Interface ]]]

SDL_Surface * rotozoomSurface (SDL_Surface *src, double angle, double zoom, int smooth);

 Rotates and zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'angle' is the rotation in degrees. 'zoom' a scaling factor. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.


SDL_Surface * rotozoomSurfaceXY (SDL_Surface *src, double angle, double zoomx, double zoomy, int smooth);

 Rotates and zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'angle' is the rotation in degrees. 'zoomx' and 'zoomy' are scaling factors that
 can also be negative. In this case the corresponding axis is flipped.  If 'smooth' 
 is 1 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.

 Note: Flipping currently only works with antialiasing turned off.


SDL_Surface * zoomSurface (SDL_Surface *src, double zoomx, double zoomy, int smooth);

 Zoomes a 32bit or 8bit 'src' surface to newly created 'dst' surface.
 'zoomx' and 'zoomy' are scaling factors for width and height. If 'smooth' is 1
 then the destination 32bit surface is anti-aliased. If the surface is not 8bit
 or 32bit RGBA/ABGR it will be converted into a 32bit RGBA format on the fly.


Smoothing (interpolation) flags work only on 32bit surfaces:

 #define SMOOTHING_OFF		0
 #define SMOOTHING_ON		1


Notes on framerate functions
----------------------------

The framerate functions are used to insert delays into the graphics loop
to maintain a constant framerate.

The implementation is more sophisticated that the usual
	SDL_Delay(1000/FPS); 
call since these functions keep track of the desired game time per frame 
for a linearly interpolated sequence of future timing points of each frame. 
This is done to avoid rounding errors from the inherent instability in the 
delay generation and application.

i.e. the 100th frame of a game running at 50Hz will be accurately
2.00sec after the 1st frame (if the machine can keep up with the drawing).


[[[ Interface ]]]

The functions return 0 or 'value' for sucess and -1 for error. All functions
use a pointer to a framerate-manager variable to operate.

void SDL_initFramerate(FPSmanager * manager);

 Initialize the framerate manager, set default framerate of 30Hz and
 reset delay interpolation.


int SDL_setFramerate(FPSmanager * manager, int rate);

 Set a new framerate for the manager and reset delay interpolation.


int SDL_getFramerate(FPSmanager * manager);

 Get the currently set framerate of the manager.


void SDL_framerateDelay(FPSmanager * manager);

 Generate a delay to accomodate currently set framerate. Call once in the
 graphics/rendering loop. If the computer cannot keep up with the rate (i.e.
 drawing too slow), the delay is zero and the delay interpolation is reset.


Notes on imageFilter functions
------------------------------

The imagefilter functions are a collection of MMX optimized routines that
operate on continuous buffers of bytes - typically greyscale images from 
framegrabbers and such - performing functions such as image addition and 
binarization. All functions (almost .. not the the convolution routines) 
have a C implementation that is automatically used on systems without MMX 
capabilities.


[[[ Interface ]]]

See the extensive list of routines in SDL_imageFilters.h for info.



Installation and Test
---------------------

To compile the library your need the SDL 1.2 installed from source or 
installed with the 'devel' RPM package. For example on Mandrake, run:
	urpmi  libSDL1.2-devel

The run
	./autogen.sh	(optional)
	./configure
	make
	make install
	ldconfig

to compile and install the library. The default location for the 
installation is /usr/local/lib and /usr/local/include. The libary 
path might need to be added to the file
	/etc/ld.so.conf

Run the shell script 'nodebug.sh' before make, to patch the makefile 
for optimized compilation:
	./autogen.sh	(optional)
	./configure
	./nodebug.sh
	make
	make install
	ldconfig

To create a Windows DLL using VisualC:
	unzip -a VisualC6.zip
	vcvars32.bat
	copy VisualC/makefile
	nmake
or
	unzip -a VisualC7.zip
and open the project file.


To create a Windows DLL using the xmingw32 cross-compiler:
	cross-configure
	cross-make
	cross-make install


To build without MMX code enabled (i.e. PPC architecture):
	./configure --disable-mmx
	make
	make install


To build on QNX6, patch first using:
	patch -p0 <QNX.diff


To build on MacOS X with Project Builder, follow these steps:
    * Update your developer tools to the lastest version (December 2002 as
	of this revision).
    * Install the SDL Developers framework for Mac OS X.
    * Download the latest SDL_gfx source distribution and extract the
	archive in a convenient location.
    * Extract the included OSX-PB.tgz archive into the
	top directory of the SDL_gfx distribution (from step 3). This will create a
	PB that contains the project files.
    * The project has targets for the SDL_gfx framework and the four test
	programs. All can be built using the 'deployment' or 'development' build
	styles. 



Test Programs
-------------

Change to the ./Test directory and run
	./configure
	make
to create several test programs for the libraries functions. This requires
the library to be compiled and installed.


See the source code .c files for sample code.


Thanks
------

Thanks to 'AppWares Development Group' for supporting this project - please 
visit http://www.appwares.com for more information.


Contributors
------------

* Fix for filledbox by Ingo van Lil, inguin at gmx.de - thanks Ingo.

* Non-alpha line drawing code adapted from routine 
  by Pete Shinners, pete at shinners.org - thanks Pete.

* More fixes by Karl Bartel, karlb at gmx.net - thanks Karl.

* Much testing and suggestions for fixes from Danny van Bruggen,
  danny at froukepc.dhs.org - thanks Danny.

* Original AA-circle/-ellipse code idea from Stephane Magnenat, 
  nct at wg0.ysagoon.com - thanks Stephane.

* Faster blending routines contributed by Anders Lindström,
  cal at swipnet.se - thanks Anders.

* New AA-circle/-ellipse code based on ideas from Anders Lindström - 
  thanks Anders.

* VisualC makefile contributed by Danny van Bruggen, 
  danny at froukepc.dhs.org - thanks Danny.

* VisualC7 project file contributed by James Turk, 
  jturk at conceptofzero.com - thanks James.

* Project Builder package contributed by Thomas Tongue, 
  TTongue at imagiware.com - Thanks Thomas.

* Fix for filledPolygon contributed by Kentaro Fukuchi 
  fukuchi at is.titech.ac.jp - Thanks Kentaro.

* QNX6 patch contributed by Mike Gorchak,
  mike at malva.ua - Thanks Mike.

* Pie idea contributed by Eike Lange,
  eike.lange at uni-essen.de - Thanks Eike.

* Dynamic font setup by Todor Prokopov,
  koprok at dir.bg - Thanks Todor.

* Horizontal/Vertical flipping code by Victor (Haypo) 
  Stinner, victor.stinner at haypocalc.com - Thanks Victor.

* OSX build fixes by Michael Wybrow, 
  mjwybrow at cs.mu.oz.au - Thanks Michael.

* gcc3.4 build fixes by Dries Verachtert, 
  dries at ulyssis.org - Thanks Dries.
