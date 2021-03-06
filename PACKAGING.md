Packaging GG
============

This document is for Linux packagers.  If you want to make a Windows or Mac OS
X installer, build the "package" target in either the MSVC or the XCode
project created by CMake.  See INSTALLING for information on how to create
these project files using CMake.

First, you need to build GG.  Once this is done, you can configure the
build to build whatever kind of RPM/Deb/whatever you like.  The build is set
up to always make .tgz and .tar.bz2 tarballs, and to detect if it can make
RPMs and Debs, and to do so if it can.  If you want to specify another sort of
package FOO, add a -DCPACK_GENERATOR=FOO to the command lines below.  You also
need to select a type of package to make (e.g. GiGiDevel), to specify the
contents of the package.  The package types are:

NoPackage
This is the default.  Everything is included in the installation set.  This is
useful because CMake generates packages based on the installation set (it
doesn't see any difference).  To build a useful package, you usually will not
want to select this one.

Full
This includes just the shared libraries of GiGi.  Documentation, headers, etc.,
are not included.

FullDevel
This includes everything *except* the shared libraries of GiGi.  This includes
documentation, headers, pkg-config files, and static libs (if built).

GiGi
This includes just the GiGi shared library.  Documentation, headers, etc., are
not included.

GiGiDevel
This includes a pkg-config file and static lib (if built) for the GiGi
library.  Because this is the main GG lib, includes documentation and headers
for all libs are included as well.

To build a package, configure cmake from the command line defining the package
type you want, and then build the "make package" target, e.g.:

> cmake [path] -DPACKAGE_TYPE=GiGi && make package

To build a full set of binary and -devel RPMs for GG and all its driver libs,
you might use a bash script like this:

> for p in 'GiGi' ; \
> do cmake [path] -DPACKAGE_TYPE=GiGi && make package ; \
> done

A note about dependencies...  I tried to define the RPM and Deb dependencies
that seem to be needed to build/distribute GG at the time of this writing.  I
didn't actually try to install the resulting Debs/RPMs on a real system.  I'm
not a packager, and don't understand all the issues involved in packaging, and
I realize that the dependencies may differ wildly from distro to distro.  To
change the dependencies to suit your needs, look near the end of the top-level
CMakeLists.txt file, and change the dependencies being added to
CPACK_RPM_PACKAGE_REQUIRES or CPACK_DEBIAN_PACKAGE_DEPENDS.
