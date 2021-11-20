# miniargv

Lightweight cross-platform C/C++ library for processing command line arguments and displaying command line help.

This library is designed to be low footprint and uses a zero copy approach.
No memory is allocated or copied.
The parsed data points directly to the to the `argv` variables as passed to `int main (int argc, char *argv[])`.
Callback functions are used by the parser to allow the application to process the command line arguments.

## GitHub Actions CI
[![GitHub-CI for miniargv](https://github.com/brechtsanders/miniargv/workflows/GitHub-CI%20for%20miniargv/badge.svg)](https://github.com/brechtsanders/miniargv/actions)
[![Doxygen Action for miniargv](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml/badge.svg)](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml)

## Links
The official repository can be found at: https://github.com/brechtsanders/miniargv

The documentation can be found at: https://brechtsanders.github.io/miniargv/miniargv_8h.html