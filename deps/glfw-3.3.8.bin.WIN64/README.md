# GLFW binaries for 64-bit Windows

This archive contains documentation, headers, pre-compiled static libraries,
import libraries and DLLs for GLFW 3.3.8.

Binaries for the following compilers are included

 - Visual C++ 2022 (built with 17.3.0 preview 4.0)
 - Visual C++ 2019 (built with 16.11.17)
 - Visual C++ 2017 (built with 15.9.49)
 - Visual C++ 2015 (built with 14.0.25431.01)
 - Visual C++ 2013 (built with 12.0.40629.00)
 - Visual C++ 2012 (built with 11.0.61219.00)
 - MinGW-w64 (built with GCC 8.1.0)


## Binaries for Visual C++

All binaries for Visual C++ 2017 and earlier are compatible with Windows XP, but
this is not supported by Visual C++ 2019.


### GLFW as a static library

To use GLFW as a static library, link against `glfw3.lib` if your application
is using the Multithreaded DLL runtime library, or `glfw3_mt.lib` if it is
using the static Multithreaded runtime library.  The remaining files in the same
directory are not needed.

The static libraries are built in release mode and do not contain debug
information but can still be linked with the debug versions of the runtime
library.


