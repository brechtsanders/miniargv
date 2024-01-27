# miniargv

Lightweight cross-platform C/C++ library for processing command line arguments and displaying command line help. Support to load settings from environment variables is also included.

## Features
 * cross-platform (tested on Windows, Linux and macOS)
 * low footprint (small code footprint and very low memory requirements)
 * no external dependencies (requires no other libraries)
 * zero copy approach: no memory is allocated or copied (except when reading from configuration files)
 * data passed to `main()` is processed using callback functions
   * `argv` as passed to `int main (int argc, char *argv[])`
   * `envp` as passed to `int main (int argc, char *argv[], char *envp[])`
 * values passed to callback functions live as long as `main()`, so no memory allocation and copying (like `strdup()`) needed
 * no cleanup functions needed before exiting the program (except when reading from configuration files)
 * possibility to read settings from configuration file (does require memory allocation and copying and cleanup afterwards)
 * partial loading of configuration file based on section
 * automatic generation of command line help and example configuration files with automatic formatting and line wrapping
 * definitions for processing and displaying help are done in a common data structure
 * basic error checking to report unknown arguments
 * does not include constraints checking (e.g. mandatory arguments or arguments that may not occur multiple times)

## Documentation
Please see the [online documentation](https://brechtsanders.github.io/miniargv/miniargv_8h.html) (generated with Doxygen).

## Installation
Download and extract the source code and use `make` to build and install:
```shell
make install
```
The default install location is `/usr/local` but you can override this by setting `PREFIX` to a different location:
```shell
make install PREFIX=/usr/custom
```
On Windows you can run the above commands from the [MSYS2](https://msys2.org/) shell using the [MinGW-w64](https://www.mingw-w64.org/) compiler.

## Example
#### **`example.c`**
```C
#include <stdlib.h>
#include <stdio.h>
#include <miniargv.h>

int main (int argc, char *argv[], char *envp[])
{
  //variables to be configured via command line or environment
  int showhelp = 0;
  int number = 0;

  //definition of command line arguments
  const miniargv_definition argdef[] = {
    {'h', "help", NULL, miniargv_cb_increment_int, &showhelp, "show command line help", NULL},
    {'n', "number", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
    MINIARGV_DEFINITION_END
  };

  //definition of environment variables
  const miniargv_definition envdef[] = {
    {0, "NUMBER", "N", miniargv_cb_set_int, &number, "set number to N", NULL},
    MINIARGV_DEFINITION_END
  };

  //parse program arguments
  if (miniargv_process(argv, envp, argdef, envdef, NULL, NULL) != 0)
    return 1;
 
  //show help if requested
  if (showhelp) {
    printf("Usage: ");
    miniargv_arg_list(argdef, 1);
    printf("\n");
    miniargv_help(argdef, envdef, 0, 0);
    return 0;
  }
 
  //show values
  printf("number = %i\n", number);
  return 0;
}
```
To build the example above:
```shell
gcc -o example example.c -lminiargv
```
Then run the example to test it:
```
# ./example -h
Usage: [-h] [-n N]
Command line arguments:
  -h, --help             show command line help
  -n N, --number=N       set number to N
Environment variables:
  NUMBER=N               set number to N

# ./example -n 42
number = 42

# NUMBER=64 ./example
number = 64
```

## GitHub Actions CI
[![GitHub-CI for miniargv](https://github.com/brechtsanders/miniargv/workflows/GitHub-CI%20for%20miniargv/badge.svg)](https://github.com/brechtsanders/miniargv/actions)
[![Doxygen Action for miniargv](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml/badge.svg)](https://github.com/brechtsanders/miniargv/actions/workflows/miniargv-doxygen.yml)

## Links
The official repository can be found at: https://github.com/brechtsanders/miniargv

The documentation can be found at: https://brechtsanders.github.io/miniargv/

## License

[MIT](LICENSE)
