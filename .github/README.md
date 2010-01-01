# Bitset Library

This library provides bitsets of arbitrary run-time length, including logical operations.

## Licence

The content of the Bitset repository is free software; you can redistribute it and/or modify it under the terms of the [GNU General Public License](http://www.gnu.org/licenses/gpl-2.0.txt) as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

The content is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

## Quick Start

*   Building requires [SCons 2](http://scons.org/) and [buildtools](https://github.com/gcrossland/buildtools). Ensure that the contents of buildtools is available on PYTHONPATH e.g. `export PYTHONPATH=/path/to/buildtools` (or is in the SCons site_scons dir).
*   The library depends on [Core](https://github.com/gcrossland/Core). Build this first.
*   From the working directory (or archive) root, run SCons to make a release build, specifying the compiler to use and where to find it e.g. `scons CONFIG=release TOOL_GCC=/usr/bin`.
    *   The library files are deployed to the library cache dir, which is (by default) under buildtools.
*   Run the tests by running the executable produced by the build and checking that it completes successfully e.g. `./bitset || echo "tests failed"`.
    *   If a test fails, the test executable will abort. To get more information (particularly a stack trace), run a debug build with a debugger.
