Getting started on mac ...

Download + extract wxWidgets 3.0.0
Put this "next to" the waxe root, or set the WXROOT variable appropriately, by default, name
"wxwidgets-static"
Configure + build static libraries:


./configure --disable-shared --disable-svg with_libtiff=no with_regex=no with_expat=no --enable-stc --disable-debug_flag  --with-opengl CXXFLAGS=-fvisibility=hidden OBJCXXFLAGS=-fvisibility=hidden OBJCFLAGS=-fvisibility=hidden

To build 32 bit versions:

setenv MYFLAGS "-fvisibility=hidden -arch i386"

./configure --disable-shared --disable-svg with_libtiff=no with_regex=no with_expat=no --enable-stc --disable-debug_flag  --with-opengl CXXFLAGS="$MYFLAGS" OBJCXXFLAGS="$MYFLAGS" OBJCFLAGS="$MYFLAGS" CFLAGS="$MYFLAGS" LDFLAGS="-arch i386" 

I had to patch src/richtext/richtextbuffer.cpp with:
 #if wxUSE_XML
around line 11055

make

cp lib/wx/include/osx_cocoa-unicode-static-3.0/wx/setup.h include/wx/setup.h



Linux - cross compile for 32 Bits:

./configure  --disable-shared --with-opengl CC="cc -m32" CXX="g++ -m32" --build=i486-pc-linux-gnu --with-gtk with_libtiff=no with_expat=no with_regex=no --without-gtkprint


Lunux 64:

./configure -q --disable-shared --with-opengl CC="cc -fpic -fPIC" CXX="g++ -fpic -fPIC" --with-gtk with_libtiff=no with_expat=no with_regex=no --without-gtkprint

make

copy setup file from lib file


Building on Windows:
Download wxWidgets 3.0.0 and extract next to the waxe directory
 cd buid/msw
 edit Config.vc:
     BUILD = release
	  MONOLITHIC = 0
	  USE_OPENGL = 1
	  RUNTIME_LIBS = static

nmake -f makefile.vc

cp  lib/vc_lib/mswu/wx/setup.h include/msvc/setup.h



